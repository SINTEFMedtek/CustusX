/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstTestMain.cpp,v 1.12 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Anton Deguet
  Created on: 2003-07-28

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cppunit/Test.h>
#include <cppunit/TestSuite.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/RepeatedTest.h>
#include <cppunit/ui/text/TestRunner.h>

#if defined(ONLY_HEADLESS_TESTS)
#include <QCoreApplication>
#else
#include <QApplication>
#include "sscApplication.h"
#endif

#include <string>
#include <list>
#include <iostream>

#include "cisstTestParameters.h"


CppUnit::Test* FindTestInTestSuite(CppUnit::Test* tests, const std::string& name) {
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    CppUnit::Test* testFound = NULL;

    // it's a suite, check all components
    if (testSuite != NULL) {
        if (testSuite->getName() == name) {
            return testSuite;
        } else {
            std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
            std::vector<CppUnit::Test*>::iterator testIterator;
            for (testIterator = allTestsVector.begin();
                 testIterator != allTestsVector.end();
                 testIterator++) {
                testFound = FindTestInTestSuite(*testIterator, name);
                // abort the search if found
                if (testFound) {
                    return testFound;
                }
            }
        }
    } else {
        // it's a test, get the name and test
        if (tests->getName() == name) {
            return tests;
        }
    }
    return NULL;
} 


CppUnit::Test * InstantiateTests(const cisstTestParameters::TestNameContainerType & testNames)
{
    CppUnit::Test * wholeRegistry = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    if (testNames.empty())
        return wholeRegistry;
    
    CppUnit::TestSuite * testSuite = new CppUnit::TestSuite("");
    cisstTestParameters::TestNameContainerType::const_iterator nameIterator
        = testNames.begin();
    while (nameIterator != testNames.end()) {
        CppUnit::Test * test = FindTestInTestSuite(wholeRegistry, (*nameIterator));
        if (test != NULL) {
            testSuite->addTest(test);
        } else {
            std::cerr << "Failed to instantiate " << (*nameIterator) << std::endl;
        }
        
        ++nameIterator;
    }
    return testSuite;
}


/*! List all available tests in a given test suite. */
int ListAllTestsInTestSuite(CppUnit::Test* tests) {
    int count = 0;
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    // it's a suite, check all components
    if (testSuite != NULL) {
        std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
        std::vector<CppUnit::Test*>::iterator testIterator;
        for (testIterator = allTestsVector.begin();
             testIterator != allTestsVector.end();
             testIterator++) {
            count += ListAllTestsInTestSuite(*testIterator);
        }
    } else {
        // it's a test, get the name
        count++;
        std::cout << tests->getName() << std::endl;
    }
    return count;
}


/*! Recursion used to fill the CMake/ctest compatible list of tests. */
int GenerateCTestFile(CppUnit::Test* tests, const std::string& programName) {
    int count = 0;
    // try to see if this is a TestSuite
    CppUnit::TestSuite* testSuite = dynamic_cast<CppUnit::TestSuite *>(tests);
    // it's a suite, check all components
    if (testSuite != NULL) {
        std::vector<CppUnit::Test*> allTestsVector = testSuite->getTests();
        std::vector<CppUnit::Test*>::iterator testIterator;
        for (testIterator = allTestsVector.begin();
             testIterator != allTestsVector.end();
             testIterator++) {
            count += GenerateCTestFile(*testIterator, programName);
        }
    } else {
        // it's a test, add it to the list
        count++;
        std::cout << "ADD_TEST(\""
                  << tests->getName() 
                  << "\" " << programName
                  << " -r -t "
                  << tests->getName()
                  << ")" << std::endl;
        // original cisst command:
//        std::cout << "ADD_TEST(\"C++: "
//                  << tests->getName() << "-i5-o5"
//                  << "\" " << programName
//                  << " -r -i 5 -o 5 -t "
//                  << tests->getName()
//                  << ")" << std::endl;
    }
    return count;
}


int main(int argc, char *argv[])
{
    cisstTestParameters testParameters;
    testParameters.ParseCmdLine(argc, argv);

    if (testParameters.GetTestRunMode() == cisstTestParameters::PRINT_HELP) {
        return cisstTestParameters::PrintHelp(argv[0]);
    }

    CppUnit::TestSuite * allTests = new CppUnit::TestSuite("All Tests");
    int instanceCounter;
    for (instanceCounter = 0; instanceCounter < testParameters.GetNumInstances(); ++instanceCounter) {
        allTests->addTest( InstantiateTests(testParameters.GetTestNames()) );
    }
    
    if (testParameters.GetTestRunMode() == cisstTestParameters::LIST_TESTS) {
        ListAllTestsInTestSuite(allTests);
        return 0;
    }

    if (testParameters.GetTestRunMode() == cisstTestParameters::GENERATE_CTEST_FILE) {
        GenerateCTestFile(allTests, testParameters.GetProgramName());
    	std::cout << "#generating done!" << std::endl;
        return 0;
    }

    if (testParameters.GetTestRunMode() == cisstTestParameters::RUN_TESTS) {
    	
#if defined(ONLY_HEADLESS_TESTS)
        QCoreApplication app(argc, argv); /// added by sonowand - needed to run Qt Code.
#else
		ssc::Application app(argc, argv); /// custom app handling exceptions
//    	QApplication app(argc, argv); /// added by sonowand - needed to run Qt Code.
#endif
    	
        CppUnit::RepeatedTest * repeatedTest =
            new CppUnit::RepeatedTest(allTests, testParameters.GetNumIterations());
        CppUnit::TextUi::TestRunner runner;
        runner.addTest(repeatedTest);
        bool wasSuccessful = runner.run();
        if (wasSuccessful) {
            return 0;
        } else {
            return 1;
        }
    }

    return cisstTestParameters::PrintHelp(argv[0]);
}

//
// ****************************************************************************
//                              Change History
// ****************************************************************************
//
//  MODIFIED by Sonowand for integration into the SSC library.
//
//
//
//
//
// $Log: cisstTestMain.cpp,v $
// Revision 1.12  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.11  2006/11/20 20:33:53  anton
// Licensing: Applied new license to tests.
//
// Revision 1.10  2005/12/08 17:12:50  anton
// Test main library: Added license.
//
// Revision 1.9  2005/12/01 03:37:01  anton
// cisstTestMain.cpp: Changed to 5 iterations, 5 instantiations for Dart
// testing (was 10, 10)
//
// Revision 1.8  2005/09/26 15:41:48  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.7  2005/09/21 18:24:49  anton
// cisstTestMain.cpp: Removed single instantiation, single iteration from CTest
// tests.  Now uses 10 iterations, 10 instantiations to improve random coverage
// and get more significant running times.
//
// Revision 1.6  2005/06/14 17:42:02  anton
// cisstTestMain.cpp: Added prefix "C++: " to test names to increase readability
//
// Revision 1.5  2005/06/03 01:22:03  anton
// Tests: Preliminary support for Dart2.
//
// Revision 1.4  2004/11/18 21:54:46  anton
// cisstTests: Added the possibility to run all the tests for a given suite.
//
// Revision 1.3  2003/11/11 22:32:07  anton
// Created separate source and header files for the class cisstTestParameters
//
// Revision 1.2  2003/08/26 18:50:19  ofri
// Modified the interface of cisstTestMain to support multiple instantiations and
// iterations of test cases.
//
// Revision 1.1.1.1  2003/07/31 18:20:33  anton
// Creation
//
// ****************************************************************************
