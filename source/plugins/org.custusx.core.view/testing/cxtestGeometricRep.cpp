/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxViewsFixture.h"
#include "cxGeometricRep.h"
#include "cxView.h"
#include "cxPNGImageReader.h"
#include "cxStlMeshReader.h"
#include "cxXMLPolyDataMeshReader.h"
#include "cxPolyDataMeshReader.h"
#include "cxtestPatientModelServiceMock.h"


namespace cxtest
{

namespace
{

void loadMeshAndTestRenderingWithTexture(QString& meshFile)
{
	//This test that the input file can be loaded, that it can be displayed and that it can be displayed with a texture without crashing or giving errors.
	//Manual testing is still needed to verify that the mesh and texture are displayed correctly with colour and the correct image and so on.
	cx::DataLocations::setTestMode();
	cxtest::ViewsFixture fixture;

	cx::FileReaderWriterServicePtr pngImageReader = cx::FileReaderWriterServicePtr(new cx::PNGImageReader(fixture.getPatientModelService()));
	cx::FileReaderWriterServicePtr stlMeshReader = cx::FileReaderWriterServicePtr(new cx::StlMeshReader(fixture.getPatientModelService()));
	cx::FileReaderWriterServicePtr vtpMeshReader = cx::FileReaderWriterServicePtr(new cx::XMLPolyDataMeshReader(fixture.getPatientModelService()));
	cx::FileReaderWriterServicePtr vtkMeshReader = cx::FileReaderWriterServicePtr(new cx::PolyDataMeshReader(fixture.getPatientModelService()));
	fixture.addFileReaderWriter(pngImageReader);
	fixture.addFileReaderWriter(stlMeshReader);
	fixture.addFileReaderWriter(vtpMeshReader);
	fixture.addFileReaderWriter(vtkMeshReader);

	INFO("Import an image and a mesh.");
	QString fileNameImage = cx::DataLocations::getTestDataPath()+"/testing/videographics/testImage01.png";


	QString info;
	cx::DataPtr dataImage = fixture.getPatientModelService()->importDataMock(fileNameImage, info, fixture.getFileManager());
	REQUIRE(dataImage);
	cx::DataPtr dataMesh = fixture.getPatientModelService()->importDataMock(meshFile, info, fixture.getFileManager());
	REQUIRE(dataMesh);

	cx::ImagePtr image = boost::dynamic_pointer_cast<cx::Image>(dataImage);
	REQUIRE(image);
	cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataMesh);
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
	CHECK_FALSE(fixture.messageListenerContainErrors());
}

} //namespace


TEST_CASE("Visual rendering: Load and show mesh with texture, stl",  "[integration][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/funnel_Y_axis.stl";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtk",  "[integration][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/kaisa_skin.vtk";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

TEST_CASE("Visual rendering: Load and show mesh with texture, vtp",  "[integration][resource][visualization]")
{
	QString fileNameMesh = cx::DataLocations::getTestDataPath()+"/testing/videographics/out.vtp";
	loadMeshAndTestRenderingWithTexture(fileNameMesh);
}

} //namespace cxtest
