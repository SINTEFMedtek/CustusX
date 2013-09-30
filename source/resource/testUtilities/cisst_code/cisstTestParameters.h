/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: cisstTestParameters.h,v 1.6 2007/04/26 20:12:05 anton Exp $
  
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


#ifndef _cisstTestParameters_h
#define _cisstTestParameters_h


#include <string>
#include <list>
#include <iostream>


class cisstTestParameters
{
public:
    typedef enum { PRINT_HELP, RUN_TESTS, LIST_TESTS, GENERATE_CTEST_FILE } TestRunModeType;
    typedef std::list<std::string> TestNameContainerType;
private:
    TestNameContainerType TestNames;
    int NumTestInstances;
    int NumTestIterations;
    TestRunModeType TestRunMode;
    std::string ProgramName;

public:
    cisstTestParameters():
        TestNames(),
        NumTestInstances(1),
        NumTestIterations(1),
        TestRunMode(PRINT_HELP)
    {}

    void ParseCmdLine(int argc, char * argv[]);

    const TestNameContainerType & GetTestNames() const
    {
        return TestNames;
    }

    TestRunModeType GetTestRunMode() const
    {
        return TestRunMode;
    }

    int GetNumInstances() const
    {
        return NumTestInstances;
    }

    int GetNumIterations() const
    {
        return NumTestIterations;
    }

    std::string GetProgramName(void) const {
        return ProgramName;
    }

    static int PrintHelp(const char * programName);
};


#endif // _cisstTestParameters_h


// ****************************************************************************
//                              Change History
// ****************************************************************************
//
// $Log: cisstTestParameters.h,v $
// Revision 1.6  2007/04/26 20:12:05  anton
// All files in tests: Applied new license text, separate copyright and
// updated dates, added standard header where missing.
//
// Revision 1.5  2006/11/20 20:33:53  anton
// Licensing: Applied new license to tests.
//
// Revision 1.4  2005/12/08 17:12:50  anton
// Test main library: Added license.
//
// Revision 1.3  2005/09/26 15:41:48  anton
// cisst: Added modelines for emacs and vi.
//
// Revision 1.2  2005/06/03 01:22:03  anton
// Tests: Preliminary support for Dart2.
//
// Revision 1.1  2003/11/11 22:32:24  anton
// Created separate source and header files for the class cisstTestParameters
//
// ****************************************************************************
