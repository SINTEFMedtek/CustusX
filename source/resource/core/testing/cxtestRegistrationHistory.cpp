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

TEST_CASE("RegistrationHistory: setParentSpace keeps entry when called with current parent", "[unit][resource][core][registration]")
{
	// Regression test: setParentSpace clears mParentSpaces then delegates to addParentSpace.
	// If the new UID matched the stale cache, addParentSpace used to silently skip the push,
	// leaving mParentSpaces empty after the clear.
	cx::RegistrationHistory history;

	history.setParentSpace("parent1");
	history.setParentSpace("parent1");

	CHECK(history.getParentSpaces().size() == 1);
	CHECK(history.getCurrentParentSpace().mUid == "parent1");
}

TEST_CASE("RegistrationHistory: updateParentSpace preserves entry when UID is unchanged", "[unit][resource][core][registration]")
{
	// Regression test: updateParentSpace erases the old entry then calls addParentSpace.
	// If the replacement UID matched the cache, the push used to be silently skipped,
	// leaving the entry permanently deleted from the history.
	cx::RegistrationHistory history;

	history.addParentSpace("parent1");
	QDateTime oldTimestamp = history.getParentSpaces().back().mTimestamp;
	cx::ParentSpace replacement("parent1", oldTimestamp.addMSecs(1), "Updated");

	history.updateParentSpace(oldTimestamp, replacement);

	CHECK(history.getParentSpaces().size() == 1);
	CHECK(history.getCurrentParentSpace().mUid == "parent1");
}

TEST_CASE("RegistrationHistory: addParentSpace does not duplicate entry in playback mode", "[unit][resource][core][registration]")
{
	// Regression test: with a historical active time pinned, mParentSpaceCache reflects an older
	// entry while mParentSpaces.back() holds the latest. addParentSpace must compare against
	// back(), not the cache, or it pushes a duplicate of the already-latest entry.
	cx::RegistrationHistory history;

	QDateTime t1 = QDateTime::fromSecsSinceEpoch(1000);
	QDateTime t2 = QDateTime::fromSecsSinceEpoch(2000);
	QDateTime t3 = QDateTime::fromSecsSinceEpoch(3000);

	history.addParentSpace(cx::ParentSpace("parent1", t1, "Set"));
	history.addParentSpace(cx::ParentSpace("parent2", t2, "Set"));

	history.setActiveTime(t1); // cache = "parent1", but latest entry is still "parent2"

	history.addParentSpace(cx::ParentSpace("parent2", t3, "Set")); // already the latest — should be a no-op

	CHECK(history.getParentSpaces().size() == 2);
}

} // namespace cxtest
