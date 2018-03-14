/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_CONSOLE_WIDTH 400
#include "catch.hpp"

#include "cxApplication.h"
#include "cxtestCatchImpl.h"
#include "boost/function.hpp"

namespace cxtest
{

CatchImpl::CatchImpl()
{
	mSession.reset(new Catch::Session());
}

int CatchImpl::run(int argc, char* argv[])
{
	cx::Application app( argc, argv );

	int returnCode = mSession->applyCommandLine( argc, argv );
	if( returnCode != 0 )
		return returnCode;

	if (this->shouldListTestsInCustomXml())
	{
		this->listTestsAsXml();
		return 0;
	}

	if (this->countTests()==0)
	{
		std::cout << "No tests matching filter, returning failure." << std::endl;
		return 1;
	}

	returnCode = mSession->run();
	return returnCode;
}

bool CatchImpl::shouldListTestsInCustomXml()
{
	return mSession->configData().listTests
			&& (mSession->configData().reporterName=="xml");
}

void CatchImpl::listTestsAsXml()
{
	std::vector<Catch::TestCase> tests = this->getMatchingTests();

	std::cout << "<?xml version=\"1.0\"?>" << std::endl;
	std::cout << "<!-- List of catch tests, custom made by CustusX -->" << std::endl;
	std::cout << "<tests>" << std::endl;
	for (unsigned i=0; i<tests.size(); ++i)
	{
		Catch::TestCaseInfo info = tests[i].getTestCaseInfo();
		std::cout << "  <test name=\"" << info.name << "\" tags=\"" << info.tagsAsString << "\"/>" << std::endl;
	}
	std::cout << "</tests>" << std::endl;
}

int CatchImpl::countTests()
{
	return this->getMatchingTests().size();
}

std::vector<Catch::TestCase> CatchImpl::getMatchingTests()
{
	std::vector<Catch::TestCase> retval;
	std::vector<Catch::TestCase> const& allTests = Catch::getRegistryHub().getTestCaseRegistry().getAllTests();
	std::vector<Catch::TestCase>::const_iterator it = allTests.begin(), itEnd = allTests.end();

	for(; it != itEnd; ++it )
		if( matchesFilters( mSession->config().filters(), *it ) )
			retval.push_back(*it);
	return retval;
}

} /* namespace cxtest */
