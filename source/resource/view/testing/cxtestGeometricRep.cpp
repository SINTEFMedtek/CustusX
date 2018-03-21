/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

void loadMeshAndTestRenderingWithTexture(QString& meshFile)
{
	//This test that the input file can be loaded, that it can be displayed and that it can be displayed with a texture without crashing or giving errors.
	//Manual testing is still needed to verify that the mesh and texture are displayed correctly with colour and the correct image and so on.
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();
	cxtest::ViewsFixture fixture;

	INFO("Import an image and a mesh.");
	QString fileNameImage = cx::DataLocations::getTestDataPath()+"/testing/videographics/testImage01.png";

	QString info;
	cx::DataPtr dataImage = cx::logicManager()->getPatientModelService()->importData(fileNameImage, info);
	cx::DataPtr dataMesh = cx::logicManager()->getPatientModelService()->importData(meshFile, info);
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

} //namespace


TEST_CASE("Visual rendering: Load and show mesh with texture, stl",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/funnel_Y_axis.stl";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtk",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/kaisa_skin.vtk";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtp",  "[unit][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/out.vtp";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

} //namespace cxtest
