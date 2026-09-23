'''
Regression test for CustusXTestInstallation.resetTestResults().

CustusX#46: test_results/ lives at a fixed path (source_custusx_path/test_results)
that persists across CI jobs on a long-lived runner - it is never recreated per
job. TestRunner.runCatch() only deletes the *crashed* run's own outfile before
retrying via ctest; the retry's own catch.*.ctest.junit.xml is never cleaned up
afterwards, so it lingers indefinitely and gets swept up by every later
test_results/*junit.xml artifact upload (CI's own glob) - polluting GitLab's
Tests tab with stale results (and a stale, lower test count) from a run that
happened weeks earlier, even when the current run is fully clean. This test
pins down that resetTestResults() actually clears such leftovers before a new
test run starts.

Run with:
    cd install && python3 -m unittest discover -s cx/build/testing -v
'''
import os
import shutil
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

from cx.build.cxCustusXTestInstallation import CustusXTestInstallation


class ResetTestResultsTest(unittest.TestCase):
    def setUp(self):
        self.source_custusx_path = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, self.source_custusx_path, ignore_errors=True)
        self.installation = CustusXTestInstallation(
            target_platform=None,
            root_dir=self.source_custusx_path,
            install_root=os.path.join(self.source_custusx_path, 'Installed'),
            system_base_name='CustusX',
            source_custusx_path=self.source_custusx_path)

    def _test_results_path(self):
        return os.path.join(self.source_custusx_path, 'test_results')

    def test_removes_stale_junit_files_left_over_from_a_previous_run(self):
        test_results = self._test_results_path()
        os.makedirs(test_results)
        stale_crash_retry = os.path.join(test_results, 'catch.unit.ex_unstable.testresults.ctest.junit.xml')
        stale_unit_result = os.path.join(test_results, 'catch.unit.ex_unstable.testresults.junit.xml')
        with open(stale_crash_retry, 'w') as f:
            f.write('<testsuite/>')
        with open(stale_unit_result, 'w') as f:
            f.write('<testsuite/>')

        self.installation.resetTestResults()

        self.assertFalse(os.path.exists(stale_crash_retry))
        self.assertFalse(os.path.exists(stale_unit_result))

    def test_does_not_raise_when_test_results_directory_does_not_exist_yet(self):
        self.assertFalse(os.path.exists(self._test_results_path()))
        self.installation.resetTestResults()  # must not raise on a cold checkout

    def test_leaves_unrelated_files_in_test_results_untouched(self):
        test_results = self._test_results_path()
        os.makedirs(test_results)
        unrelated_file = os.path.join(test_results, 'some_other_report.xml')
        with open(unrelated_file, 'w') as f:
            f.write('not a catch result')

        self.installation.resetTestResults()

        self.assertTrue(os.path.exists(unrelated_file))


if __name__ == '__main__':
    unittest.main()
