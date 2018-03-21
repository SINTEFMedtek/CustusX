/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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



