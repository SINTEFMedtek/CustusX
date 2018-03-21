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
#include "cxAccusurf.h"
#include <vtkImageData.h>

typedef boost::shared_ptr<class cx::Accusurf> AccusurfPtr;

namespace cxtest {


TEST_CASE("AccusurfFilter: execute", "[unit][org.custusx.filter.accusurf]")
{
    cx::LogicManager::initialize();

    //setup filter
    AccusurfPtr accusurf = AccusurfPtr(new cx::Accusurf());
    REQUIRE(accusurf);

    QString filenameVolume = cx::DataLocations::getTestDataPath()+"/Person5/person5_flair_unsigned.mhd";

    QString filenameCenterline = cx::DataLocations::getTestDataPath()+"/testing/Centerline/US_aneurism_cl_size0.vtk";

    //create a new patient
	QString info;
	cx::DataPtr dataVolume = cx::logicManager()->getPatientModelService()->importData(filenameVolume, info);
	cx::DataPtr dataCenterline = cx::logicManager()->getPatientModelService()->importData(filenameCenterline, info);

    REQUIRE(dataVolume);
    REQUIRE(dataCenterline);

    cx::MeshPtr mesh = boost::dynamic_pointer_cast<cx::Mesh>(dataCenterline);
    cx::ImagePtr image = boost::dynamic_pointer_cast<cx::Image>(dataVolume);

	vtkPolyDataPtr route_d_image = mesh->getTransformedPolyDataCopy((image->get_rMd().inverse())*mesh->get_rMd());
    accusurf->setRoutePositions(route_d_image);
    accusurf->setInputImage(image);
    accusurf->setThickness(15, 5);
    vtkImageDataPtr accusurfImage = accusurf->createAccusurfImage();

    REQUIRE(accusurfImage);

    cx::LogicManager::shutdown();
}

}; // end cxtest namespace
