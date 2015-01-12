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
