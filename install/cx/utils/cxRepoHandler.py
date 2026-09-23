from __future__ import print_function
#####################################################
# 
# Used by both CustusX and applications built on top of it (wrapper applications).
# Intended to be copied into the repos of wrapper applications.
#
#####################################################

import os.path
import os
import sys
import subprocess
import pprint
import argparse
import shutil

def returnCode():
    return 0

def localChangesCode():
    return 2

def runShell(cmd, path):
    '''
    simple shell implementation.
    Return value is last stdout line, None if failure.
    Note: May return an empty string that indicated success, but evaluates to False.
    '''
    if not os.path.exists(path):
        os.makedirs(path)
    print('[shell cmd] %s [%s]' % (cmd, path))
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=path)
    out, err = p.communicate("") # wait for process to complete
    if type(out) is bytes:
        out = out.decode()
    if type(err) is bytes:
        err = err.decode()
    if err:
        print(err.strip())
    if out:
        print(out.strip())
    if p.returncode == 0:
        return out.strip()
    if "error: The following untracked working tree files would be overwritten" in err:
        return returnCode()
    if "local changes to the following files would be overwritten" in err:
        return localChangesCode()
    return None

def getBranchForRepo(path, fallback=None):
    'Find the git branch in the repo in the input path, None if detached'
    branch = runShell('git rev-parse --abbrev-ref HEAD', path)
    if branch=="HEAD":
        return fallback
    return branch


class RepoHandler(object):
    '''
    Handle git repository URL, tag, branch when checking out and updating the repo
    tasks:
     - checkout/set correct URL, configurable from code and command line
     - checkout correct tag, configurable from command line
     - checkout correct branch configurable from code and command line
     -  fallback policy when branch does not exist
    '''
    pass

    def __init__(self, silent=True):
        self.silent = silent
        self.default_branch = None
        self.fallback_branch = 'develop'
        self.args = self._parseArgs()

    def setRepoInfo(self, url_base=None, url_name=None, root_path=None, repo_path=None):
        'Set repo properties. Those not set cannot be used and attempt to do so will cause failure.'
        self.url_base = url_base
        self.url_name = url_name
        self.root_path = root_path
        self.repo_path = repo_path

    def is_git_directory(self, path = '.'):
        return subprocess.call(['git', '-C', path, 'status'], stderr=subprocess.STDOUT, stdout = open(os.devnull, 'w')) == 0

    def cloneRepoWithPrompt(self):
        '''
        Checkout the repository, clone if needed.
        requires:
         - url
         - checkout folder
         - root path
        '''
        if self.is_git_directory(self.repo_path):
            return
        pathfound = os.path.exists(self.repo_path)
        if pathfound:
            print("Not a git repo, removing folder and contents of %s." % self.repo_path)

        print('*** %s will be cloned in [%s]' % (self.getName(), self.root_path))
        doprompt = not (self.silent or self.args.silent_mode)
        self._promptToContinue(doprompt)

        if pathfound:
            shutil.rmtree(self.repo_path)

        self._cloneWithRetry()

    def _cloneWithRetry(self, attempts=3):
        '''
        A bare `git clone` has no resilience against a mid-transfer TLS drop
        (e.g. GnuTLS recv error), unlike GitLab Runner's own checkout of the
        pipeline repo, which retries automatically. Retry here too, falling
        back to HTTP/1.1 since HTTP/2 is the more likely side to drop the
        connection on some networks/proxies.
        '''
        for attempt in range(1, attempts + 1):
            http_fallback = '' if attempt == 1 else '-c http.version=HTTP/1.1 '
            cmd = 'git %sclone %s %s' % (http_fallback, self.getUrl(), self.repo_path)
            if runShell(cmd, self.root_path) is not None:
                return
            print('Clone attempt %d/%d failed for %s.' % (attempt, attempts, self.getName()))
            if os.path.exists(self.repo_path):
                shutil.rmtree(self.repo_path)
        sys.exit('ERROR: failed to clone %s after %d attempts.' % (self.getName(), attempts))

    def getUrl(self):
        url_base = self.url_base
        if self.args.gitrepo_main_site_base:
            url_base = self.args.gitrepo_main_site_base
        url = '%s/%s' % (url_base, self.url_name) 
        return url
    
    def getName(self):
        return self.repo_path.split('/')[-1]
        
    def syncToGitRef(self):
        '''
        Checkout the correct tag or branch    
        - if git_tag is set, use that, else fail
        - if main_branch is set, use that, else:
        -   try the default and fallback branches
        '''
        # --prune (not --prune-tags too): the branch-verification loop below
        # relies on refs/remotes/origin/<branch> accurately reflecting which
        # branches still exist on the remote, so a plain fetch's stale
        # remote-tracking refs need pruning. --prune-tags is deliberately
        # left out -- it would delete local tags that were never pushed,
        # which the tag-fallback check further down relies on still being
        # there.
        runShell('git fetch --prune', self.repo_path)

        tag = self.args.git_tag
        if tag:
            print('Checking out %s to tag=%s' % (self.getName(), tag))
            if runShell('git checkout %s' % tag, self.repo_path) is None:
                exit("tag checkout failed")
            return
        
        branches = [self.args.main_branch,
                    self.default_branch,
                    self.fallback_branch]
        branches = self.cleanBranchList(branches)

        print('Checkout {} to the first existing branch in list [{}]'.format(self.getName(), ','.join(branches)))

        for branch in branches:
            # A local branch of this name can exist (e.g. left over from an
            # earlier run on a long-lived build machine or CI runner) even
            # after its remote counterpart has been deleted or renamed. Don't
            # trust it just because `git checkout <branch>` trivially succeeds
            # against it -- verify the remote branch is still there first
            # (reliable right after the --prune fetch above).
            is_branch = runShell('git rev-parse --verify refs/remotes/origin/%s' % branch, self.repo_path) is not None
            # main_branch can also be a tag name (e.g. a tag-triggered CI
            # build passes its own tag as the ref to check other repos out
            # to) -- those never exist under refs/remotes/origin/, so fall
            # back to checking for a tag of that name.
            # Verified against the remote (like is_branch above), not just the
            # local tag ref: `git fetch --prune` doesn't prune tags (deliberately
            # -- see the --prune-tags removal above, which protects not-yet-pushed
            # local tags), so a tag deleted/renamed on the remote would otherwise
            # still be found locally and trusted as a valid checkout target.
            is_tag = (not is_branch) and runShell('git ls-remote --exit-code --tags origin refs/tags/%s' % branch, self.repo_path) is not None
            if not (is_branch or is_tag):
                continue
            result = runShell('git checkout %s' % branch, self.repo_path)
            self.checkSuccess(result)
            if result is localChangesCode():
                # Warned already via checkSuccess(); this branch didn't work,
                # but don't abort the whole build over it -- try the next
                # candidate instead, same as any other checkout failure.
                continue
            if result is None:
                continue
            if is_branch:
                # Skip the merge entirely if we're already at the remote's
                # commit (the normal case right after a fresh CI checkout):
                # `git merge` still computes a merge-base even for a no-op,
                # and on a shallow clone (e.g. GitLab CI's GIT_DEPTH) two
                # independently-shallow-fetched refs for "the same branch"
                # can carry different synthetic grafted roots, making git
                # see them as having no common ancestor at all ("fatal:
                # refusing to merge unrelated histories") even though
                # they're actually identical commits.
                head_sha = runShell('git rev-parse HEAD', self.repo_path)
                remote_sha = runShell('git rev-parse origin/%s' % branch, self.repo_path)
                if head_sha is not None and remote_sha is not None and head_sha == remote_sha:
                    break
                # A workspace reused across many CI jobs on the same runner
                # (e.g. GitLab CI's GIT_CLONE_PATH pointing every job at the
                # same directory to speed up builds) can leave a local branch
                # whose history was shallow-grafted by one job's fetch, while
                # origin/<branch> was just shallow-fetched fresh by this job
                # from a different ref -- the two shallow boundaries don't
                # overlap, so git sees no common ancestor at all ("fatal:
                # refusing to merge unrelated histories") even though the
                # real history is shared. Fetch full history first so the
                # merge below can find it.
                if runShell('git rev-parse --is-shallow-repository', self.repo_path) == 'true':
                    runShell('git fetch --unshallow', self.repo_path)
                # Merge in any new remote commits instead of resetting to
                # origin -- a local commit made here but not yet pushed (e.g.
                # a release-branch fix or merge queued up before the next
                # `git push`) must survive this sync rather than silently
                # vanish the next time this repo gets synced.
                self._ensureGitIdentityForMerge(self.repo_path)
                merge_result = runShell('git merge origin/%s' % branch, self.repo_path)
                self.checkSuccess(merge_result)
                if merge_result is None:
                    # Unlike the checkout above, there's no next candidate to
                    # fall back to here -- we've already committed to this
                    # branch. A None result most likely means a real merge
                    # conflict: abort loudly instead of silently proceeding
                    # to configure/compile against a repo left mid-merge with
                    # unresolved conflict markers and a dangling MERGE_HEAD.
                    print('Resolve it manually in %s and re-run -- nothing was' % self.repo_path)
                    print('auto-aborted, so the mid-merge state and any local changes are still there.')
                    sys.exit(1)
            break

    def _ensureGitIdentityForMerge(self, path):
        '''
        `git merge` needs *an* identity to create the resulting merge commit,
        even for a real, non-conflicting merge of a legitimate local-only
        commit (see the "must survive this sync" comment above) -- a CI build
        image with no ~/.gitconfig has nowhere to get one from and fails with
        "Please tell me who you are" on the very first such merge, which
        looks like a merge conflict in the logs but isn't one. Set a local
        (repo-scoped, never --global), clearly-labelled fallback identity,
        only if none is configured anywhere already.
        '''
        if (runShell('git config user.name', path) is not None
                and runShell('git config user.email', path) is not None):
            return
        runShell('git config user.name "CustusX CI"', path)
        runShell('git config user.email "ci@custusx.no"', path)

    def checkSuccess(self, gitResult):
        if gitResult is returnCode():
            print('----------------------------------------------------------------------------')
            print('|                                     ^                                    |')
            print('| Delete the folder containing these files and the CustusX build folder.   |')
            print('----------------------------------------------------------------------------')
            print('===== Your local file structure of CustusX is not in synch with the file structure on the server ===')
            print('This is e.g. because a plugin have been moved from one repository to another on the server.')
            print('Take a backup first if you have been working with the files.')
            print('To come in sync:')
            print('- delete the folder containing the above mentioned files and the CustusX build folder.')
            print('- run the script again.')
            sys.exit(1)
        if gitResult is localChangesCode():
            print('----------------------------------------------------------------------------')
            print('|                                     ^                                    |')
            print('|      You have uncommitted local changes in %s' % self.repo_path)
            print('----------------------------------------------------------------------------')
            print('===== Could not switch %s to the branch/commit this build wanted =====' % self.getName())
            print('Your uncommitted changes were NOT touched or discarded -- git refused to')
            print('check out over them. Continuing the build with whatever is already checked')
            print('out there, which may not be what you expect. If that turns out wrong,')
            print('commit, stash, or discard your local changes in %s and re-run.' % self.repo_path)
            return
        if gitResult is None:
            # Any git failure runShell() didn't recognize as one of the two
            # specific cases above -- most notably a real merge conflict from
            # syncToGitRef()'s `git merge origin/<branch>` step. Only warn
            # here (like the local-changes case above): syncToGitRef()'s
            # checkout call site already has its own "try the next branch
            # candidate" fallback for a None result, which this must not
            # break. The merge call site has no such fallback and separately
            # aborts the build itself on a None result -- see there for why.
            print('----------------------------------------------------------------------------')
            print('|                                     ^                                    |')
            print('|      git command failed unexpectedly in %s' % self.repo_path)
            print('----------------------------------------------------------------------------')
            print('===== Could not sync %s to the branch/commit this build wanted =====' % self.getName())
            print('This may be a merge conflict (see git output above) or another git error')
            print('not specifically handled here.')

    def cleanBranchList(self, branches):
        retval = []
        for branch in branches:
            if branch and branch not in retval:
                if type(branch) is bytes:
                    branch = branch.decode()
                #print("{} is TYPE: {}".format(branch, type(branch)))
                retval.append(branch)
        return retval
        
    def setBranchDefault(self, branch):
        '''
        set a default branch to use if no main_branch is set
        '''
        self.default_branch = branch
    
    def setBranchFallback(self, branch):
        '''
        Set the branch to use if no other branches work
        '''
        self.fallback_branch = branch

    def _parseArgs(self):
        parser = argparse.ArgumentParser(add_help=False, conflict_handler='resolve')
        parser.add_argument('-g', '--git_tag', default=None, metavar='TAG', dest='git_tag')
        parser.add_argument('--main_branch', default=None, dest='main_branch')
        parser.add_argument('--gitrepo_main_site_base', default=None)
        # Matches cxInstallScript.py's own -s/--silent_mode flag (parse_known_args
        # ignores anything else on the command line, so this just picks up that
        # same global flag when present instead of always defaulting to False).
        parser.add_argument('-s', '--silent_mode', action='store_true', dest='silent_mode')
        args = parser.parse_known_args()[0]
        return args
    
    
    def _promptToContinue(self, do_it):
        if do_it:
            input("\nPress enter to continue or ctrl-C to quit:")
    
