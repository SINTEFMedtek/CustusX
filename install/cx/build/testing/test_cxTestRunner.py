'''
Regression tests for cx.build.cxTestRunner.classifyCatchResult().

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


if __name__ == '__main__':
    unittest.main()
