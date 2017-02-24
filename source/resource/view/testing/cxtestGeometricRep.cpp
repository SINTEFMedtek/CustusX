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

#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxViewsFixture.h"
#include "cxGeometricRep.h"


TEST_CASE("Visual rendering: Load and show mesh",  "[unit][resource][visualization][jon2]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cxtest::ViewsFixture fixture;


	QString filenameImage = cx::DataLocations::getTestDataPath()+"/testing/videographics/testImage01.png";
	QString filenameCenterline = cx::DataLocations::getTestDataPath()+"/testing/videographics/funnel_Y_axis.stl";

	//create a new patient
	QString info;
	cx::DataPtr dataImage = cx::logicManager()->getPatientModelService()->importData(filenameImage, info);
	cx::DataPtr dataCenterline = cx::logicManager()->getPatientModelService()->importData(filenameCenterline, info);
	REQUIRE(dataImage);
	REQUIRE(dataCenterline);

	cx::ImagePtr image = boost::dynamic_pointer_cast<cx::Image>(dataImage);
	cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline);
	REQUIRE(image);
	REQUIRE(mesh);


	//mesh->updateVtkPolyDataWithTexture();


	cx::GeometricRepPtr p = cx::GeometricRep::New();
	p->setMesh(mesh);
	cx::ViewPtr view = fixture.addView(0,0);
	view->addRep(p);

	//CHECK(fixture.runWidget());
	CHECK(fixture.quickRunWidget());
	view->removeReps();

	mesh->getTextureData().getTextureImage()->setValue(image->getUid());

	view->addRep(p);
	CHECK(fixture.quickRunWidget());
	view->removeReps();

	mesh->getTextureData().getTextureImage()->setValue(NULL);

	view->addRep(p);
	CHECK(fixture.quickRunWidget());
	view->removeReps();

	cx::LogicManager::shutdown();
}
