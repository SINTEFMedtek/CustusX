/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstTestParameters.cpp,v 1.9 2007/04/26 20:12:05 anton Exp $
  
  Author(s):  Anton Deguet, Ofri Sadowsky
  Created on: 2003-11-10

  (C) Copyright 2003-2007 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include "cisstTestParameters.h"
#include <string.h>
#include <stdlib.h>


void cisstTestParameters::ParseCmdLine(int argc, char * argv[])
{
    ProgramName = argv[0];
    const size_t size = ProgramName.size();
    size_t index;
    for(index = 0; index < size; index++) {
        if (ProgramName[index] == '\\') {
            ProgramName[index]= '/';
        }
    }

    while (argc != 1) {
        if (argc < 1) {
            TestRunMode = PRINT_HELP;
            return;
        }

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            TestRunMode = PRINT_HELP;
            return;
        }

        if (strcmp(argv[1], "--run") == 0 || strcmp(argv[1], "-r") == 0) {
            TestRunMode = RUN_TESTS;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0) {
            TestRunMode = LIST_TESTS;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--dart") == 0 || strcmp(argv[1], "-d") == 0) {
            TestRunMode = GENERATE_CTEST_FILE;
            ++argv;
            --argc;
            continue;
        }

        if (strcmp(argv[1], "--numinstances") == 0 || strcmp(argv[1], "-o") == 0) {
            NumTestInstances = atoi(argv[2]);
            argv += 2;
            argc -= 2;
            continue;
        }

        if (strcmp(argv[1], "--numiterations") == 0 || strcmp(argv[1], "-i") == 0) {
            NumTestIterations = atoi(argv[2]);
            argv += 2;
            argc -= 2;
            continue;
        }

        if (strcmp(argv[1], "--testname") == 0 || strcmp(argv[1], "-t") == 0) {
            TestNames.push_back( argv[2] );
            argv += 2;
            argc -= 2;
            continue;
        }

        TestRunMode = PRINT_HELP;
        return;
    }
}


/*! Print help message. */
int cisstTestParameters::PrintHelp(const char* programName) {
    std::cerr
        << programName << ": Usage" << std::endl
        << "-h, --help               print this message" << std::endl
        << "-l, --list               print the available test instances" << std::endl
        << "-d, --dart               print CMake/ctest commands in DartTestfile.txt format" << std::endl 
        << "-r, --run                run the available test instances" << std::endl
        << "-t, --testname [name]    add the specified test case or suite to the list" << std::endl
        << "-o, --numinstances [n]   specify the number of instances to create of each test" << std::endl
        << "-i, --numiterations [n]  specify the number of iterations for each test instance" << std::endl;

    std::cerr << std::endl << std::endl;
    std::cerr
        << "If no names are specified, all tests are instantiated." << std::endl
        << "Otherwise, the instantiated tests are those whose names or suite" << std::endl
        << "were listed and which appear in the test registry." << std::endl
        << "The number of instances and number of iterations is 1 by default" << std::endl;


    return 0;
}


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cisstTestParameters.cpp,v $
// Revision 1.9  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.8  2006/11/20 20:33:53  anton
// Licensing: Applied new license to tests.
//
// Revision 1.7  2005/12/08 17:12:50  anton
// Test main library: Added license.
//
// Revision 1.6  2005/09/26 15:41:48  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.5  2005/07/14 03:58:55  anton
// cisstTestParameters.cpp: Replace \ by / in path for CTest
//
// Revision 1.4  2005/06/03 01:22:03  anton
// Tests: Preliminary support for Dart2.
//
// Revision 1.3  2004/11/18 21:54:46  anton
// cisstTests: Added the possibility to run all the tests for a given suite.
//
// Revision 1.2  2004/05/11 16:35:08  kapoor
// Checked in **PREMATURE** dynamic object creation code. NO flames PLEASE
//
// Revision 1.1  2003/11/11 22:32:24  anton
// Created separate source and header files for the class cisstTestParameters
//
// ****************************************************************************
