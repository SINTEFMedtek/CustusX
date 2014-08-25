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

#include "cxStringHelpers.h"
#include "catch.hpp"
#include <QStringList>
#include <QString>

TEST_CASE("splitStringContaingQuotes keeps elements with double quotes and a space", "[unit]")
{
	QString test("alpha beta \"gam ma\" delta");
	QStringList list = cx::splitStringContaingQuotes(test);
	CHECK(list.length() == 4);
	CHECK(list[0] == "alpha");
	CHECK(list[2] == "gam ma");
}

TEST_CASE("splitStringContaingQuotes keeps elements with double quotes and several spaces", "[unit]")
{
	QString test("alpha beta \"g a m ma\" delta");
	QStringList list = cx::splitStringContaingQuotes(test);
	CHECK(list.length() == 4);
	CHECK(list[0] == "alpha");
	CHECK(list[2] == "g a m ma");
}

TEST_CASE("splitStringContaingQuotes splits a simple list", "[unit]")
{
	QString test("alpha beta gamma delta");
	QStringList list = cx::splitStringContaingQuotes(test);
	CHECK(list.length() == 4);
	CHECK(list[0] == "alpha");
	CHECK(list[2] == "gamma");
}

TEST_CASE("splitStringContaingQuotes work with an element with double quotes and no space", "[unit]")
{
	QString test("alpha beta \"gamma\" delta");
	QStringList list = cx::splitStringContaingQuotes(test);
	CHECK(list.length() == 4);
	CHECK(list[0] == "alpha");
	CHECK(list[2] == "gamma");
}



