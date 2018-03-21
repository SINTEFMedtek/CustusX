/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QTimer>
#include <QApplication>
#include <QFile>
#include "cxtestTestVideoConnectionWidget.h"
#include "cxtestQueuedSignalListener.h"
#include "cxDataLocations.h"

#include "cxDummyToolManager.h"
#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVideoService.h"
#include "cxUtilHelpers.h"

#include "cxSessionStorageService.h"


namespace cxtest
{

TEST_CASE("VideoConnectionWidget can stream", "[gui][not_win32][widget][streaming][integration]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	QString folder = cx::DataLocations::getTestDataPath() + "/temp/test.cx3";
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	services->session()->load(folder);
//	services->patient()->newPatient(folder);

	cx::DummyToolPtr tool = cx::DummyToolTestUtilities::createDummyTool(cx::DummyToolTestUtilities::createProbeDefinitionLinear());
	services->tracking()->runDummyTool(tool);

	REQUIRE((services->tracking()->getState()>=cx::Tool::tsTRACKING));
	waitForQueuedSignal(services->tracking().get(), SIGNAL(stateChanged()));

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/default_volume/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget(services);
	REQUIRE(widget->canStream(filename));

	services.reset();
	delete widget;
	cx::LogicManager::shutdown();
}

} //namespace cxtest
