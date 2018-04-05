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
#include "cxMesh.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxCenterlineRegistration.h"
#include <vtkImageData.h>

typedef boost::shared_ptr<cx::CenterlineRegistration> CenterlineRegistrationPtr;


namespace cxtest {


TEST_CASE("CenterlineRegistration: execute", "[unit][org.custusx.registration.method.centerline]")
{
    cx::LogicManager::initialize();

    CenterlineRegistrationPtr centerlineRegistration;
    centerlineRegistration = CenterlineRegistrationPtr(new cx::CenterlineRegistration());
    REQUIRE(centerlineRegistration);

    centerlineRegistration->UpdateScales(true, true, true, true, true, true);

    QString filenameCenterline1 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size0.vtk";
    QString filenameCenterline2 = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size1.vtk";

    //create a new patient
	QString info;
	cx::DataPtr dataCenterline1 = cx::logicManager()->getPatientModelService()->importData(filenameCenterline1, info);
	cx::DataPtr dataCenterline2 = cx::logicManager()->getPatientModelService()->importData(filenameCenterline2, info);

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

    cx::LogicManager::shutdown();
}

}; // end cxtest namespace
