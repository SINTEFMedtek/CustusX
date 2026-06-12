/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxRegistrationTransform.h"

namespace cxtest
{

TEST_CASE("RegistrationHistory: addParentSpace sets parent", "[unit][resource][core][registration]")
{
	cx::RegistrationHistory history;

	history.addParentSpace("parent1");

	CHECK(history.getCurrentParentSpace().mUid == "parent1");
}

TEST_CASE("RegistrationHistory: addParentSpace ignores no-op when parent is unchanged", "[unit][resource][core][registration]")
{
	cx::RegistrationHistory history;

	history.addParentSpace("parent1");
	history.addParentSpace("parent1");

	CHECK(history.getParentSpaces().size() == 1);
}

TEST_CASE("RegistrationHistory: addParentSpace allows reverting to a previous parent", "[unit][resource][core][registration]")
{
	// Regression test: setting a parent whose UID was previously used must succeed.
	// Bug: the old UID-only duplicate check silently ignored such calls, leaving
	// the more recent parent active even though the user explicitly chose to revert.
	cx::RegistrationHistory history;

	history.addParentSpace("parent1");
	CHECK(history.getCurrentParentSpace().mUid == "parent1");

	history.addParentSpace("parent2");
	CHECK(history.getCurrentParentSpace().mUid == "parent2");

	history.addParentSpace("parent1");
	CHECK(history.getCurrentParentSpace().mUid == "parent1");
}

TEST_CASE("RegistrationHistory: addParentSpace builds history with multiple entries", "[unit][resource][core][registration]")
{
	cx::RegistrationHistory history;

	history.addParentSpace("parent1");
	history.addParentSpace("parent2");
	history.addParentSpace("parent1");

	CHECK(history.getParentSpaces().size() == 3);
}

} // namespace cxtest
