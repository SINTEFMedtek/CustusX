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
