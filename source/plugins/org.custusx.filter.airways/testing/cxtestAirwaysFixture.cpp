/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
