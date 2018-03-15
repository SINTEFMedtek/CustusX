/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTCATCHIMPL_H_
#define CXTESTCATCHIMPL_H_

#include "boost/shared_ptr.hpp"
#include "catch.hpp"

#include "cxcatch_export.h"

namespace Catch
{
class Session;
}

namespace cxtest
{

/**
 * Wrapper for Catch framework.
 *
 * Contains a few addons:
 * - if "--list-tests" AND "--reporter xml" is input,
 *   override normal behaviour to output a xml-formatted name list.
 *   (used in scripting - the default list format is not parse-friendly)
 * - if no tests are run, return failure.
 *   (required by ctest: a config error in ctest-catch should give a failure)
 *
 * \author christiana
 * \date Jun 28, 2013
 */
class CXCATCH_EXPORT CatchImpl
{
public:
	CatchImpl();
	int run(int argc, char* argv[]);
private:
	bool shouldListTestsInCustomXml();
	void listTestsAsXml();
	int countTests();
	std::vector<Catch::TestCase> getMatchingTests();

	boost::shared_ptr<Catch::Session> mSession;
};

} /* namespace cxtest */
#endif /* CXTESTCATCHIMPL_H_ */
