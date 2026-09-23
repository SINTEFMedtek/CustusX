'''
Regression tests for cx.build.cxTestRunner.classifyCatchResult() and
TestRunner.runCatch()'s crash-recovery return value.

CustusX#46: a plain Catch assertion failure (a small positive returncode)
was previously indistinguishable, in the CI script, from a passing run --
_catch_has_failed() (now classifyCatchResult()) only recognised a crash
(negative returncode, or "Segmentation fault" in stdout), so the CI job
never noticed an ordinary test failure. These tests pin down the classifier
that decides TEST_RESULT_PASSED / TEST_RESULT_FAILED / TEST_RESULT_CRASHED.

Run with:
    cd install && python3 -m unittest discover -s cx/build/testing -v
'''
import os
import sys
import unittest
from unittest import mock

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

from cx.build import cxTestRunner


class _FakeShellResult:
    def __init__(self, returncode, stdout=''):
        self.returncode = returncode
        self.stdout = stdout


class TestClassifyCatchResult(unittest.TestCase):
    def test_zero_returncode_is_passed(self):
        result = cxTestRunner.classifyCatchResult(_FakeShellResult(returncode=0, stdout='All tests passed'))
        self.assertEqual(result, cxTestRunner.TEST_RESULT_PASSED)

    def test_positive_returncode_is_failed_not_crashed(self):
        # Catch's own convention: number of failed assertions, not a crash.
        result = cxTestRunner.classifyCatchResult(_FakeShellResult(returncode=3, stdout='3 assertions failed'))
        self.assertEqual(result, cxTestRunner.TEST_RESULT_FAILED)

    def test_negative_returncode_is_crashed(self):
        result = cxTestRunner.classifyCatchResult(_FakeShellResult(returncode=-11, stdout=''))
        self.assertEqual(result, cxTestRunner.TEST_RESULT_CRASHED)

    def test_segfault_text_with_positive_returncode_is_crashed(self):
        result = cxTestRunner.classifyCatchResult(_FakeShellResult(returncode=139, stdout='Segmentation fault (core dumped)'))
        self.assertEqual(result, cxTestRunner.TEST_RESULT_CRASHED)

    def test_missing_stdout_attribute_does_not_raise(self):
        class _NoStdout:
            def __init__(self, returncode):
                self.returncode = returncode
        result = cxTestRunner.classifyCatchResult(_NoStdout(returncode=2))
        self.assertEqual(result, cxTestRunner.TEST_RESULT_FAILED)


class RunCatchCrashRecoveryTest(unittest.TestCase):
    '''
    A monolithic 400+-test single-process Catch run occasionally crashes
    from accumulated cross-test state - not any single test's own
    correctness - and every such crash has been confirmed to disappear when
    the same tests are re-run one-per-process via ctest. runCatch() must
    report whatever that ctest retry actually found, not the stale crash
    classification, so a fully-clean retry doesn't still mark the CI job as
    failed (CustusX#46).
    '''
    def setUp(self):
        self.runner = cxTestRunner.TestRunner(target_platform=None)
        self._patchers = [
            mock.patch.object(cxTestRunner, 'shell'),
            mock.patch.object(self.runner, '_getCatchExecutable', return_value='/fake/Catch'),
            mock.patch.object(self.runner, '_createCatchBaseFilenameFromTag', return_value='catch.unit.testresults'),
        ]
        for patcher in self._patchers:
            patcher.start()
            self.addCleanup(patcher.stop)
        cxTestRunner.shell.run.return_value = _FakeShellResult(returncode=-11, stdout='')

    def test_returns_retrys_passed_result_when_monolithic_run_crashed_but_retry_is_clean(self):
        with mock.patch.object(self.runner, 'runCatchTestsWrappedInCTestGenerateJUnit',
                                return_value=cxTestRunner.TEST_RESULT_PASSED) as mock_retry:
            result = self.runner.runCatch(path='/fake', tag='[unit]', outpath='/fake/out')

        self.assertEqual(result, cxTestRunner.TEST_RESULT_PASSED)
        mock_retry.assert_called_once_with('[unit]', '/fake', '/fake/out')

    def test_returns_retrys_failed_result_when_a_test_genuinely_fails_even_in_isolation(self):
        with mock.patch.object(self.runner, 'runCatchTestsWrappedInCTestGenerateJUnit',
                                return_value=cxTestRunner.TEST_RESULT_FAILED) as mock_retry:
            result = self.runner.runCatch(path='/fake', tag='[unit]', outpath='/fake/out')

        self.assertEqual(result, cxTestRunner.TEST_RESULT_FAILED)
        mock_retry.assert_called_once()

    def test_does_not_retry_when_the_monolithic_run_did_not_crash(self):
        cxTestRunner.shell.run.return_value = _FakeShellResult(returncode=0, stdout='')
        with mock.patch.object(self.runner, 'runCatchTestsWrappedInCTestGenerateJUnit') as mock_retry:
            result = self.runner.runCatch(path='/fake', tag='[unit]', outpath='/fake/out')

        self.assertEqual(result, cxTestRunner.TEST_RESULT_PASSED)
        mock_retry.assert_not_called()


if __name__ == '__main__':
    unittest.main()
