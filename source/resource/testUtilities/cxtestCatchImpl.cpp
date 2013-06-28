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

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "sscApplication.h"
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
	ssc::Application app( argc, argv );

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
