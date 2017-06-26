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


#include "cxtestAirwaysFixture.h"

#include "catch.hpp"

#include <QFile>
#include "cxLogicManager.h"
#include "cxDataLocations.h"
#include "cxSessionStorageService.h"
#include "cxAirwaysFilterService.h"
#include "cxtestVisServices.h"
#include "cxSelectDataStringPropertyBase.h"

namespace cxtest
{

AirwaysFixture::AirwaysFixture()
{
}

AirwaysFixture::~AirwaysFixture()
{
}

void AirwaysFixture::testLungAirwaysCT()
{
	runFilter(QString("pat011"));
}

void AirwaysFixture::runFilter(const QString& preset)
{
	cx::DataLocations::setTestMode();

    cxtest::TestVisServicesPtr dummyservices = cxtest::TestVisServices::create();

	QString filename = cx::DataLocations::getExistingTestData("testing/airwaysegmentation", preset+".mhd");
    {
        INFO("Using "+filename.toStdString()+" as testdata.");
        REQUIRE(QFile::exists(filename));
    }
	dummyservices->session()->load(cx::DataLocations::getTestDataPath()+ "/temp/airwaysegmentation/");
    QString info;
    cx::DataPtr input_image = dummyservices->patient()->importData(filename, info);
    {
        INFO("Image used is called "+input_image->getName().toStdString());
        REQUIRE(input_image);
    }

    cx::AirwaysFilterPtr airways = cx::AirwaysFilterPtr(new cx::AirwaysFilter(dummyservices));
    REQUIRE(airways);
    airways->getInputTypes();
    airways->getOutputTypes();
    airways->getOptions();

    //set input
    std::vector <cx::SelectDataStringPropertyBasePtr> input = airways->getInputTypes();
    {
        INFO("Number of inputs has changed.");
        REQUIRE(input.size() == 1);
    }
    {
        INFO("Setting input from image with uid: "+input_image->getUid().toStdString()+" and name: "+input_image->getName().toStdString());
        REQUIRE(input[0]->setValue(input_image->getUid()));
        REQUIRE(input[0]->getData());
    }
    {
        INFO("The name of the input data should be "+preset.toStdString());
        REQUIRE(input[0]->getData()->getName() == preset);
    }

    //execute
    {
        INFO("Preprocessing Airways.");
        REQUIRE(airways->preProcess());
    }

    {
        INFO("Executed Airways on "+filename.toStdString());
        REQUIRE(airways->execute());
    }
    {
        INFO("Post processing data from Airways.");
        REQUIRE(airways->postProcess());
    }

    //check output
    std::vector < cx::SelectDataStringPropertyBasePtr > output = airways->getOutputTypes();
    {
        INFO("Number of outputs has changed.");
		REQUIRE(output.size() == 3);
    }
    {
        INFO("Centerline.");
        REQUIRE(output[0]->getData());
    }
    {
        INFO("Segmented surface.");
        REQUIRE(output[1]->getData());
    }
	// ToDo: test Lung sack, the new 3rd output?
	// Erik is testing the algorithms so we only need a test that things are running, i.e. the airway seg.
	// So maybe not needed.

}


}
