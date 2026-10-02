"""
Tests for cx.utils.cxConvertCTest2JUnit (ctest Test.xml -> JUnit XML for the
GitLab test report), used when Catch tests run one test per process via ctest
(integration tests, and the retry after a crash).
"""
import os
import shutil
import tempfile
import unittest
import xml.etree.ElementTree as ET

try:
    import lxml.etree  # noqa: F401 -- the converter needs lxml for XSLT
    HAVE_LXML = True
except ImportError:
    HAVE_LXML = False

from cx.utils import cxConvertCTest2JUnit

# The Test.xml format written by current ctest (3.x): the execution time is in a
# <Value> child, with the file's indentation around it.
CTEST_XML = '''<?xml version="1.0" encoding="UTF-8"?>
<Site BuildName="linux" BuildStamp="20260925-1405-Experimental" Name="runner">
	<Testing>
		<Test Status="passed">
			<Name>Import MNI Tag Point file</Name>
			<Results>
				<NamedMeasurement type="numeric/double" name="Execution Time">
					<Value>1.61133</Value>
				</NamedMeasurement>
				<NamedMeasurement type="text/string" name="Completion Status">
					<Value>Completed</Value>
				</NamedMeasurement>
				<Measurement>
					<Value>All tests passed</Value>
				</Measurement>
			</Results>
		</Test>
		<Test Status="failed">
			<Name>Crashing test</Name>
			<Results>
				<NamedMeasurement type="text/string" name="Exit Code">
					<Value>SEGFAULT</Value>
				</NamedMeasurement>
				<NamedMeasurement type="text/string" name="Exit Value">
					<Value>11</Value>
				</NamedMeasurement>
				<NamedMeasurement type="numeric/double" name="Execution Time">
					<Value>2.5</Value>
				</NamedMeasurement>
				<Measurement>
					<Value>output</Value>
				</Measurement>
			</Results>
		</Test>
	</Testing>
</Site>
'''

# An older format, with the value directly in <NamedMeasurement>.
CTEST_XML_OLD = '''<?xml version="1.0" encoding="UTF-8"?>
<Site BuildName="linux" BuildStamp="stamp" Name="runner">
	<Testing>
		<Test Status="passed">
			<Name>Old style</Name>
			<Results>
				<NamedMeasurement type="numeric/double" name="Execution Time">0.75</NamedMeasurement>
				<Measurement><Value>ok</Value></Measurement>
			</Results>
		</Test>
	</Testing>
</Site>
'''


@unittest.skipUnless(HAVE_LXML, 'lxml is not installed')
class TestConvertCTest2JUnit(unittest.TestCase):

    def setUp(self):
        self.tmp = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.tmp)

    def _convert(self, ctestXml):
        ctestFile = os.path.join(self.tmp, 'Test.xml')
        junitFile = os.path.join(self.tmp, 'Test.junit.xml')
        with open(ctestFile, 'w') as f:
            f.write(ctestXml)
        cxConvertCTest2JUnit.convertCTestFile2JUnit(ctestFile, junitFile)
        return ET.parse(junitFile).getroot()

    def test_execution_time_is_a_plain_number(self):
        # GitLab's test report sums these times: whitespace around the number
        # can make a test count as 0 s there.
        suite = self._convert(CTEST_XML)
        times = [case.get('time') for case in suite.iter('testcase')]
        self.assertEqual(times, ['1.61133', '2.5'])

    def test_execution_time_old_ctest_format(self):
        suite = self._convert(CTEST_XML_OLD)
        self.assertEqual([case.get('time') for case in suite.iter('testcase')], ['0.75'])

    def test_counts_and_failures(self):
        suite = self._convert(CTEST_XML)
        self.assertEqual(suite.get('tests'), '2')
        cases = list(suite.iter('testcase'))
        self.assertEqual([case.get('name') for case in cases], ['Import MNI Tag Point file', 'Crashing test'])
        self.assertIsNone(cases[0].find('error'))
        error = cases[1].find('error')
        self.assertIsNotNone(error)
        self.assertEqual(error.get('message'), 'SEGFAULT (11)')


if __name__ == '__main__':
    unittest.main()
