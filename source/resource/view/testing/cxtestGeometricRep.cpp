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
#include "cxView.h"


namespace cxtest
{

namespace
{
class loadMeshAndTestRenderingWithTexture
{
public:
	loadMeshAndTestRenderingWithTexture(QString& meshFile)
	{
		//This test that the input file can be loaded, that it can be displayed and that it can be displayed with a texture without crashing or giving errors.
		//Manual testing is still needed to verify that the mesh and texture are displayed correctly with colour and the correct image and so on.
		cx::DataLocations::setTestMode();
		cx::LogicManager::initialize();
		cxtest::ViewsFixture fixture;

		INFO("Import an image and a mesh.");
		QString fileNameImage = cx::DataLocations::getTestDataPath()+"/testing/videographics/testImage01.png";
		QString fileNameMesh = meshFile;

		QString info;
		cx::DataPtr dataImage = cx::logicManager()->getPatientModelService()->importData(fileNameImage, info);
		cx::DataPtr dataMesh = cx::logicManager()->getPatientModelService()->importData(fileNameMesh, info);
		REQUIRE(dataImage);
		REQUIRE(dataMesh);

		cx::ImagePtr image = boost::dynamic_pointer_cast<cx::Image>(dataImage);
		cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataMesh);
		REQUIRE(image);
		REQUIRE(mesh);

		INFO("Create a rep with the mesh and add it to a view.");
		cx::GeometricRepPtr p = cx::GeometricRep::New();
		p->setMesh(mesh);
		cx::ViewPtr view = fixture.addView(0,0);
		view->addRep(p);

		CHECK(fixture.quickRunWidget());
		view->removeReps();

		INFO("Add the image as texture to the mesh.");
		mesh->getTextureData().getTextureImage()->setValue(image->getUid());

		view->addRep(p);
		CHECK(fixture.quickRunWidget());
		view->removeReps();

		INFO("Remove the texture from the mesh mesh.");
		mesh->getTextureData().getTextureImage()->setValue(NULL);

		view->addRep(p);
		CHECK(fixture.quickRunWidget());
		view->removeReps();

		cx::LogicManager::shutdown();
	}


};

} //namespace


TEST_CASE("Visual rendering: Load and show mesh with texture, stl",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/funnel_Y_axis.stl";
	loadMeshAndTestRenderingWithTexture test(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtk",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/kaisa_skin.vtk";
	loadMeshAndTestRenderingWithTexture test(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtp",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/out.vtp";
	loadMeshAndTestRenderingWithTexture test(fileNameMesh);
}

} //namespace cxtest
