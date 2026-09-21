'''
Regression tests for cx.utils.cxRepoHandler.RepoHandler.syncToGitRef().

Each test builds a throwaway "upstream" repo and a clone of it under a temp
directory, exercises syncToGitRef() against the clone, and checks the
resulting git state. No network access, no fixtures shared with the real
CustusX/CustusS/Fraxinus repos.

Run with:
    cd install && python3 -m unittest discover -s cx/utils/testing -v
or:
    python3 install/cx/utils/testing/test_cxRepoHandler.py
'''
import os
import subprocess
import sys
import tempfile
import shutil
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

from cx.utils import cxRepoHandler


def _git(args, cwd):
    result = subprocess.run(['git'] + args, cwd=cwd, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError('git %s failed in %s:\n%s' % (' '.join(args), cwd, result.stderr))
    return result.stdout.strip()


def _init_repo(path):
    os.makedirs(path)
    _git(['init', '-q'], path)
    _git(['config', 'user.email', 'test@example.com'], path)
    _git(['config', 'user.name', 'Test'], path)


def _commit(path, filename='file.txt', content='content', message='commit'):
    with open(os.path.join(path, filename), 'w') as f:
        f.write(content)
    _git(['add', filename], path)
    _git(['commit', '-q', '-m', message], path)


def _current_branch(path):
    return _git(['rev-parse', '--abbrev-ref', 'HEAD'], path)


def _head_sha(path):
    return _git(['rev-parse', 'HEAD'], path)


def _make_handler(repo_path, main_branch=None, fallback_branch='develop', git_tag=None):
    '''
    RepoHandler._parseArgs() reads sys.argv (via parse_known_args, which
    ignores anything it doesn't recognize -- safe to call under a test
    runner's own argv). Override the fields under test directly afterward,
    same as cxCustusXFinder.py and friends do at the call site.
    '''
    handler = cxRepoHandler.RepoHandler(silent=True)
    handler.setRepoInfo(url_base=None, url_name=None, root_path=os.path.dirname(repo_path), repo_path=repo_path)
    handler.args.main_branch = main_branch
    handler.args.git_tag = git_tag
    handler.setBranchFallback(fallback_branch)
    return handler


class SyncToGitRefTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.mkdtemp(prefix='cxRepoHandler_test_')
        self.upstream = os.path.join(self.tmp, 'upstream')
        self.clone = os.path.join(self.tmp, 'clone')

    def tearDown(self):
        shutil.rmtree(self.tmp, ignore_errors=True)

    def _clone_upstream(self):
        _git(['clone', '-q', self.upstream, self.clone], self.tmp)
        # `git clone` doesn't carry over repo-local config, and a CI runner
        # (unlike a dev machine) typically has no global user.name/user.email
        # configured at all -- commits/tags made directly in the clone (not
        # just the upstream repo set up in setUp()) need their own identity.
        _git(['config', 'user.email', 'test@example.com'], self.clone)
        _git(['config', 'user.name', 'Test'], self.clone)

    def test_local_only_commit_survives_sync(self):
        '''
        A local commit made on the target branch but not yet pushed must
        still be there after syncToGitRef() -- this is the bug from
        `git checkout -B <branch> origin/<branch>` hard-resetting the branch
        instead of merging (CustusX#46).
        '''
        _init_repo(self.upstream)
        _commit(self.upstream)
        _git(['branch', '-m', 'release/v1'], self.upstream)
        self._clone_upstream()

        _commit(self.clone, filename='local_fix.txt', content='fix', message='local unpushed fix')
        local_sha = _head_sha(self.clone)

        handler = _make_handler(self.clone, main_branch='release/v1')
        handler.syncToGitRef()

        self.assertEqual(_head_sha(self.clone), local_sha)
        self.assertTrue(os.path.exists(os.path.join(self.clone, 'local_fix.txt')))

    def test_stale_local_branch_falls_back_to_next_candidate(self):
        '''
        A local branch whose remote counterpart has since been deleted must
        not be trusted just because `git checkout <branch>` would succeed
        against it -- syncToGitRef() should skip it and fall through to the
        next candidate branch instead.
        '''
        _init_repo(self.upstream)
        _commit(self.upstream)
        _git(['checkout', '-q', '-b', 'trunk'], self.upstream)
        _git(['checkout', '-q', '-b', 'release/v1'], self.upstream)
        _commit(self.upstream, filename='release.txt', content='release', message='release commit')
        _git(['checkout', '-q', '-b', 'develop', 'trunk'], self.upstream)
        _commit(self.upstream, filename='develop.txt', content='develop', message='develop commit')
        _git(['checkout', '-q', 'trunk'], self.upstream)
        self._clone_upstream()

        _git(['checkout', '-q', '-b', 'release/v1', 'origin/release/v1'], self.clone)
        _git(['branch', '-D', 'release/v1'], self.upstream)

        handler = _make_handler(self.clone, main_branch='release/v1', fallback_branch='develop')
        handler.syncToGitRef()

        self.assertEqual(_current_branch(self.clone), 'develop')

    def test_main_branch_can_be_a_tag(self):
        '''
        main_branch can be a tag name (e.g. a tag-triggered CI build passing
        its own tag as the ref another repo should check out) -- those never
        exist under refs/remotes/origin/, so syncToGitRef() must fall back
        to resolving it as a tag (CustusS's rc-tagged builds, CustusX#46).
        '''
        _init_repo(self.upstream)
        _commit(self.upstream)
        _git(['tag', '-a', 'v1.0-rc1', '-m', 'rc1'], self.upstream)
        tagged_sha = _head_sha(self.upstream)
        self._clone_upstream()

        handler = _make_handler(self.clone, main_branch='v1.0-rc1')
        handler.syncToGitRef()

        self.assertEqual(_head_sha(self.clone), tagged_sha)

    def test_uncommitted_local_changes_do_not_abort(self):
        '''
        Uncommitted local changes blocking a checkout should be left alone
        (not discarded) and syncToGitRef() should return without raising,
        rather than crashing the whole build over one repo's dirty state.
        '''
        _init_repo(self.upstream)
        _commit(self.upstream, filename='shared.txt', content='A')
        _git(['checkout', '-q', '-b', 'other'], self.upstream)
        _git(['checkout', '-q', '-b', 'develop'], self.upstream)
        with open(os.path.join(self.upstream, 'shared.txt'), 'w') as f:
            f.write('B')
        _git(['commit', '-q', '-am', 'develop change'], self.upstream)
        _git(['checkout', '-q', 'other'], self.upstream)
        self._clone_upstream()
        _git(['checkout', '-q', 'other'], self.clone)

        with open(os.path.join(self.clone, 'shared.txt'), 'w') as f:
            f.write('C (uncommitted)')

        handler = _make_handler(self.clone, main_branch='develop', fallback_branch='nonexistent-branch')
        handler.syncToGitRef()  # must not raise

        self.assertEqual(_current_branch(self.clone), 'other')
        with open(os.path.join(self.clone, 'shared.txt')) as f:
            self.assertEqual(f.read(), 'C (uncommitted)')

    def test_fetch_does_not_prune_local_only_tags(self):
        '''
        `git fetch --prune --prune-tags` deletes local-only tags that
        haven't been pushed yet -- e.g. a release tag created by cxRelease.py
        moments before its own push step runs. syncToGitRef() must only
        prune stale branches, never tags (CustusX#46).
        '''
        _init_repo(self.upstream)
        _commit(self.upstream)
        _git(['branch', '-m', 'release/v1'], self.upstream)
        self._clone_upstream()

        _git(['tag', '-a', 'v1.0-rc1', '-m', 'not yet pushed'], self.clone)

        handler = _make_handler(self.clone, main_branch='release/v1')
        handler.syncToGitRef()

        _git(['rev-parse', '--verify', 'refs/tags/v1.0-rc1'], self.clone)  # raises if missing

    def test_merge_skipped_when_already_at_remote_commit(self):
        '''
        On a fresh CI checkout, `git checkout <branch>` lands exactly on
        origin/<branch>'s commit -- syncToGitRef() must not then call
        `git merge` at all. `git merge` still computes a merge-base even
        for a no-op, and on a shallow clone (e.g. GitLab CI's GIT_DEPTH)
        two independently-shallow-fetched refs for "the same branch" can
        carry different synthetic grafted roots, making git see them as
        having no common ancestor ("fatal: refusing to merge unrelated
        histories") even though the commits are identical (CustusX#50).
        '''
        _init_repo(self.upstream)
        _commit(self.upstream)
        _git(['branch', '-m', 'release/v1'], self.upstream)
        self._clone_upstream()
        # Simulate the CI runner's own initial checkout: detached HEAD at
        # the same commit release/v1 already points to, no local branch of
        # that name yet -- same starting point as a real MR pipeline.
        _git(['checkout', '-q', '--detach', 'HEAD'], self.clone)

        calls = []
        real_run_shell = cxRepoHandler.runShell
        def spying_run_shell(cmd, path):
            calls.append(cmd)
            return real_run_shell(cmd, path)
        cxRepoHandler.runShell = spying_run_shell
        try:
            handler = _make_handler(self.clone, main_branch='release/v1')
            handler.syncToGitRef()
        finally:
            cxRepoHandler.runShell = real_run_shell

        self.assertFalse(any(c.startswith('git merge') for c in calls), calls)
        self.assertEqual(_current_branch(self.clone), 'release/v1')

    def test_merge_survives_shallow_grafted_unrelated_histories(self):
        '''
        A workspace reused across many CI jobs on the same runner (e.g.
        GitLab CI's GIT_CLONE_PATH) can leave a local branch shallow-grafted
        by one job's fetch, while origin/<branch> gets shallow-fetched fresh
        by a later job from a different ref -- the two shallow boundaries
        don't overlap, so a plain `git merge` fails with "fatal: refusing to
        merge unrelated histories" even though the real history is shared.
        syncToGitRef() must recover by unshallowing before merging
        (CustusX#50).
        '''
        _init_repo(self.upstream)
        for i in range(10):
            _commit(self.upstream, filename='f.txt', content=str(i), message='commit %d' % i)
        _git(['branch', '-m', 'release/v1'], self.upstream)

        # `--depth` is silently ignored for local-path clones (git optimizes
        # them via hardlinks) -- a file:// URL is needed to force a real
        # shallow clone, matching what a genuine network clone would do.
        upstream_url = 'file://' + self.upstream
        _git(['clone', '-q', '--depth', '1', '--branch', 'release/v1', upstream_url, self.clone], self.tmp)
        _git(['config', 'user.email', 'test@example.com'], self.clone)
        _git(['config', 'user.name', 'Test'], self.clone)
        self.assertEqual(_git(['rev-parse', '--is-shallow-repository'], self.clone), 'true')

        # Upstream advances well beyond the clone's shallow depth (simulating
        # weeks of later commits), then a differently-shallow-fetched ref is
        # pulled into the clone -- mirroring a fresh CI job's own shallow
        # MR-ref fetch landing in this same reused workspace.
        for i in range(10, 60):
            _commit(self.upstream, filename='f.txt', content=str(i), message='commit %d' % i)
        _git(['update-ref', 'refs/mr-head', 'HEAD'], self.upstream)
        _git(['fetch', '--depth', '20', 'origin', 'refs/mr-head:refs/remotes/origin/mr-head'], self.clone)
        _git(['checkout', '--detach', 'origin/mr-head'], self.clone)
        _git(['checkout', 'release/v1'], self.clone)
        _git(['fetch', '--depth', '20', 'origin', 'release/v1'], self.clone)

        # Sanity check: a plain merge really does fail with unrelated
        # histories here, confirming the repro before checking the fix.
        plain_merge = subprocess.run(['git', 'merge', 'origin/release/v1'], cwd=self.clone, capture_output=True, text=True)
        self.assertNotEqual(plain_merge.returncode, 0)
        self.assertIn('unrelated histories', plain_merge.stderr)
        # A rejected "unrelated histories" merge never enters an in-progress
        # merge state (no MERGE_HEAD), unlike a real conflicting merge --
        # nothing to abort here before retrying for real below.

        handler = _make_handler(self.clone, main_branch='release/v1')
        handler.syncToGitRef()  # must not raise or abort

        self.assertEqual(_git(['rev-parse', 'HEAD'], self.clone), _git(['rev-parse', 'origin/release/v1'], self.clone))
        self.assertEqual(_git(['rev-parse', '--is-shallow-repository'], self.clone), 'false')


if __name__ == '__main__':
    unittest.main()
