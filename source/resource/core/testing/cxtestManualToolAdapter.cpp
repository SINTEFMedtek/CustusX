/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxManualToolAdapter.h"
#include "cxDummyTool.h"
#include "cxSliceProxy.h"
#include "cxSliceComputer.h"
#include "cxPatientModelService.h"

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

cx::SliceProxyPtr createProbeViewSliceProxy(cx::ToolPtr tool)
{
	cx::SliceProxyPtr proxy = cx::SliceProxy::create(cx::PatientModelService::getNullObject());
	proxy->setTool(tool);
	proxy->initializeFromPlane(cx::ptANYPLANE, false, true, 200, 0.5);
	return proxy;
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

TEST_CASE("ManualToolAdapter: A slice proxy following it views a probe base set later like one set before", "[unit][resource][core]")
{
	cx::DummyToolPtr probe(new cx::DummyTool("probe"));
	probe->setType(cx::Tool::TOOL_US_PROBE);
	probe->setProbeSector(cx::DummyToolTestUtilities::createProbeDefinition(cx::ProbeDefinition::tSECTOR, 100, 60));
	probe->setVisible(true);

	cx::ManualToolAdapterPtr baseSetBefore(new cx::ManualToolAdapter("before"));
	baseSetBefore->setBase(probe);
	cx::SliceProxyPtr proxyBefore = createProbeViewSliceProxy(baseSetBefore);

	// e.g. the EBUS simulator: the views follow the manual tool before it gets the configured probe
	cx::ManualToolAdapterPtr baseSetLater(new cx::ManualToolAdapter("later"));
	cx::SliceProxyPtr proxyLater = createProbeViewSliceProxy(baseSetLater);
	baseSetLater->setBase(probe);

	CHECK(cx::similar(proxyLater->getComputer().getPlane(), proxyBefore->getComputer().getPlane()));
}

} // namespace cxtest
