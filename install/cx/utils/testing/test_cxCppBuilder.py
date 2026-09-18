'''
Regression tests for cx.utils.cxCppBuilder.CppBuilder._gitFetchWithRetry()
and CppBuilder.isAtTag().

No network access, no real git repos - runShell()/shell.evaluate() are
mocked so these only exercise the retry/backoff/give-up and
already-at-tag-skip logic itself (CustusX#46).

Run with:
    cd install && python3 -m unittest discover -s cx/utils/testing -v
or:
    python3 install/cx/utils/testing/test_cxCppBuilder.py
'''
import os
import shutil
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

from cx.utils import cxCppBuilder


class GitFetchWithRetryTest(unittest.TestCase):
    def setUp(self):
        self.builder = cxCppBuilder.CppBuilder()

    @mock.patch('cx.utils.cxCppBuilder.runShell')
    def test_succeeds_first_attempt_without_retry(self, mock_run_shell):
        mock_run_shell.return_value = True

        self.builder._gitFetchWithRetry()

        mock_run_shell.assert_called_once_with('git fetch', ignoreFailure=True)

    @mock.patch('cx.utils.cxCppBuilder.runShell')
    def test_retries_and_falls_back_to_http1_1_after_first_failure(self, mock_run_shell):
        mock_run_shell.side_effect = [False, True]

        self.builder._gitFetchWithRetry()

        self.assertEqual(mock_run_shell.call_count, 2)
        mock_run_shell.assert_any_call('git fetch', ignoreFailure=True)
        mock_run_shell.assert_any_call('git -c http.version=HTTP/1.1 fetch', ignoreFailure=True)

    @mock.patch('cx.utils.cxCppBuilder.runShell')
    def test_gives_up_and_exits_after_all_attempts_fail(self, mock_run_shell):
        mock_run_shell.return_value = False

        with self.assertRaises(SystemExit):
            self.builder._gitFetchWithRetry(attempts=3)

        self.assertEqual(mock_run_shell.call_count, 3)


class IsAtTagTest(unittest.TestCase):
    def setUp(self):
        self.builder = cxCppBuilder.CppBuilder()
        self.tmp = tempfile.mkdtemp(prefix='cxCppBuilder_test_')
        self.builder.setPaths(base=self.tmp, build=self.tmp, source=self.tmp)

    def tearDown(self):
        shutil.rmtree(self.tmp, ignore_errors=True)

    @mock.patch('cx.utils.cxCppBuilder.shell')
    def test_true_when_git_describe_matches_tag(self, mock_shell):
        mock_shell.evaluate.return_value = mock.Mock(stdout='v9.6.1\n')

        self.assertTrue(self.builder.isAtTag('v9.6.1'))

    @mock.patch('cx.utils.cxCppBuilder.shell')
    def test_false_when_git_describe_is_a_different_tag(self, mock_shell):
        mock_shell.evaluate.return_value = mock.Mock(stdout='v9.2.6\n')

        self.assertFalse(self.builder.isAtTag('v9.6.1'))

    @mock.patch('cx.utils.cxCppBuilder.shell')
    def test_false_when_git_describe_fails(self, mock_shell):
        mock_shell.evaluate.return_value = None

        self.assertFalse(self.builder.isAtTag('v9.6.1'))


if __name__ == '__main__':
    unittest.main()
