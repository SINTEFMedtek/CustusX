/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
