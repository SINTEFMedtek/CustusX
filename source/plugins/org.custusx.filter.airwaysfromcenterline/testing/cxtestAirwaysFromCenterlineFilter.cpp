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
#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxAirwaysFromCenterline.h"
#include <vtkImageData.h>
#include <vtkPolyData.h>

typedef boost::shared_ptr<class cx::AirwaysFromCenterline> AirwaysFromCenterlinePtr;

namespace cxtest {


TEST_CASE("AirwaysFromCenterline: execute", "[unit][org.custusx.filter.airwaysfromcenterline]")
{
    cx::LogicManager::initialize();

    //setup filter
    AirwaysFromCenterlinePtr airwaysFromCenterline = AirwaysFromCenterlinePtr(new cx::AirwaysFromCenterline());
    REQUIRE(airwaysFromCenterline);

    QString filenameCenterline = cx::DataLocations::getTestDataPath()+"/testing/Lung/Thorax__1_0_I30f_tsf_cl1.vtk";

    //create a new patient
	QString info;
    cx::DataPtr dataCenterline = cx::logicManager()->getPatientModelService()->importData(filenameCenterline, info);

    REQUIRE(dataCenterline);

    cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline);

    vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());
    airwaysFromCenterline->processCenterline(centerline_r);

    vtkPolyDataPtr outputAirwayMesh = airwaysFromCenterline->generateTubes();

    cx::MeshPtr outputCenterline = cx::MeshPtr(new cx::Mesh("test"));
    outputCenterline->setVtkPolyData(airwaysFromCenterline->getVTKPoints());

    REQUIRE(outputAirwayMesh);
    REQUIRE(outputCenterline->getVtkPolyData());


    cx::LogicManager::shutdown();
}

}; // end cxtest namespace
