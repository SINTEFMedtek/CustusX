'''
Regression tests for cx.utils.cxCppBuilder.CppBuilder._gitFetchWithRetry().

No network access, no real git repos - runShell() is mocked so these only
exercise the retry/backoff/give-up logic itself (CustusX#46).

Run with:
    cd install && python3 -m unittest discover -s cx/utils/testing -v
or:
    python3 install/cx/utils/testing/test_cxCppBuilder.py
'''
import os
import sys
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


if __name__ == '__main__':
    unittest.main()
