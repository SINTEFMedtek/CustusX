/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxManualToolAdapter.h"

namespace cxtest
{

namespace
{
int countVisibleSignals(cx::ToolPtr adapter, cx::ManualToolPtr toolToChange)
{
	int count = 0;
	QMetaObject::Connection connection = QObject::connect(adapter.get(), &cx::Tool::toolVisible, [&count](bool) { ++count; });
	toolToChange->setVisible(!toolToChange->getVisible());
	QObject::disconnect(connection);
	return count;
}
}

TEST_CASE("ManualToolAdapter: Created from uid forwards signals from its own base", "[unit][resource][core]")
{
	cx::ManualToolAdapterPtr adapter(new cx::ManualToolAdapter("ManualTool"));
	CHECK(adapter->getUid() == "ManualTool");

	cx::ManualToolPtr other(new cx::ManualTool("other"));
	adapter->setBase(other);
	CHECK(countVisibleSignals(adapter, other) == 1);
}

TEST_CASE("ManualToolAdapter: Created from base tool uses and forwards that base", "[unit][resource][core]")
{
	cx::ManualToolPtr base(new cx::ManualTool("base"));
	cx::ManualToolAdapterPtr adapter(new cx::ManualToolAdapter(base));
	CHECK(adapter->getUid() == "base_manual");
	CHECK(countVisibleSignals(adapter, base) == 1);

	cx::ManualToolPtr other(new cx::ManualTool("other"));
	adapter->setBase(other);
	CHECK(countVisibleSignals(adapter, base) == 0);
	CHECK(countVisibleSignals(adapter, other) == 1);

	adapter->setBase(cx::ToolPtr());
	CHECK(countVisibleSignals(adapter, other) == 0);
	CHECK(countVisibleSignals(adapter, base) == 1);
}

} // namespace cxtest
