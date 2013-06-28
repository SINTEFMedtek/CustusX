// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXTESTCATCHIMPL_H_
#define CXTESTCATCHIMPL_H_

#include "boost/shared_ptr.hpp"
#include "catch.hpp"

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
class CatchImpl
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
