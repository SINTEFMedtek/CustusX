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



