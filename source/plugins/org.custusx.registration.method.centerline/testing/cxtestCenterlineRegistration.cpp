/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDataLocations.h"
#include "cxPatientModelService.h"
#include "cxCenterlineRegistration.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxVisServices.h"

typedef boost::shared_ptr<cx::CenterlineRegistration> CenterlineRegistrationPtr;


namespace cxtest {


// This test just use two random centerlines and register them to each other.
// The test only verifies that the code is running without crashing, and that all objects are created.
TEST_CASE("CenterlineRegistration: execute", "[integration][org.custusx.registration.method.centerline]")
{
	cxtest::SessionStorageTestFixture storageFixture;
	storageFixture.loadSession1();//Create test patient folder

    CenterlineRegistrationPtr centerlineRegistration;
    centerlineRegistration = CenterlineRegistrationPtr(new cx::CenterlineRegistration());
    REQUIRE(centerlineRegistration);

    centerlineRegistration->UpdateScales(true, true, true, true, true, true);

    QString filenameCenterline1 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size0.vtk";
    QString filenameCenterline2 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size1.vtk";

	QString info;
	cx::DataPtr dataCenterline1 = storageFixture.mServices->patient()->importData(filenameCenterline1, info);
	cx::DataPtr dataCenterline2 = storageFixture.mServices->patient()->importData(filenameCenterline2, info);

    REQUIRE(dataCenterline1);
    REQUIRE(dataCenterline2);

    cx::MeshPtr mesh1 = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline1);
    cx::MeshPtr mesh2 = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline2);

    vtkPointsPtr centerlinePoints1 = centerlineRegistration->processCenterline(mesh1->getVtkPolyData(), cx::Transform3D::Identity());
    centerlineRegistration->SetFixedPoints(centerlinePoints1);

    vtkPointsPtr centerlinePoints2 = centerlineRegistration->processCenterline(mesh2->getVtkPolyData(), cx::Transform3D::Identity());
    centerlineRegistration->SetMovingPoints(centerlinePoints2);

    cx::Transform3D rMpr = centerlineRegistration->FullRegisterMoving(cx::Transform3D::Identity());
    REQUIRE(rMpr.data());
}

}; // end cxtest namespace
