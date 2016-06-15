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

//Test: Run test as integration to get more output for debugging (in addition to unit)
TEST_CASE("VideoConnectionWidget can stream", "[unit][gui][not_win32][widget][streaming][integration]")
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

	QString filename = cx::DataLocations::getTestDataPath() + "/testing/TubeSegmentationFramework/Default.mhd";
	REQUIRE(QFile::exists(filename));

	TestVideoConnectionWidget* widget = new TestVideoConnectionWidget(services);
	REQUIRE(widget->canStream(filename));

	services.reset();
	delete widget;
	cx::LogicManager::shutdown();
}

} //namespace cxtest
