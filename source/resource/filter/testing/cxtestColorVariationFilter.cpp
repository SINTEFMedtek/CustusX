/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxData.h"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxColorVariationFilter.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxVisServices.h"
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include "cxtestVisServices.h"


namespace cxtest {

TEST_CASE("ColorVariationFilter: execute", "[unit][ColorVariationFilter]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	storageFixture.loadSession1();//Create test patient folder

	//setup filter
	cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();
	cx::ColorVariationFilterPtr colorVariationFilter = cx::ColorVariationFilterPtr(new cx::ColorVariationFilter(dummyservices));
	REQUIRE(colorVariationFilter);

	QString filenameMesh = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/kaisa_skin.vtk";

	//create a new patient
	QString info;
	cx::DataPtr dataMesh = storageFixture.mServices->patient()->importData(filenameMesh, info);

	REQUIRE(dataMesh);

	cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataMesh);

	double globaleVariance = 50.0;
	double localeVariance = 5.0;
	int smoothingIterations = 5;
	
	cx::MeshPtr coloredMesh = colorVariationFilter->execute(mesh, globaleVariance, localeVariance, smoothingIterations);


	REQUIRE(coloredMesh);
	REQUIRE(coloredMesh->getTransformedPolyDataCopy(coloredMesh->get_rMd()));
}

}; // end cxtest namespace
