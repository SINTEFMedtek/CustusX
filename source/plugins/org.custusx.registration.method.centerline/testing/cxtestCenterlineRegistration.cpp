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
#include "cxData.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxCenterlineRegistration.h"
#include <vtkImageData.h>
#include "cxtest_org_custusx_registration_method_centerline_export.h"

typedef boost::shared_ptr<cx::CenterlineRegistration> CenterlineRegistrationPtr;

namespace cxtest {

class CXTEST_ORG_CUSTUSX_REGISTRATION_METHOD_CENTERLINE_EXPORT CenterlineRegistrationTest{}; //Needed for Windows linking


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
    cx::DataPtr dataCenterline1 = cx::patientService()->importData(filenameCenterline1, info);
    cx::DataPtr dataCenterline2 = cx::patientService()->importData(filenameCenterline2, info);

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
