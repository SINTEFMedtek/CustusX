// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxtestReconstructRealData.h"
#include "catch.hpp"
#include <vtkImageData.h>
#include "cxDataLocations.h"

namespace cxtest
{

QString ReconstructRealTestData::getSourceFilename() const
{
	QString filename = cx::DataLocations::getTestDataPath() +
			"/testing/"
			"US_Acquisition_angio_lab/US-Acq_03_20121024T132330.mhd";
	return filename;
}

void ReconstructRealTestData::validateData(cx::ImagePtr output)
{
	REQUIRE(output->getModality().contains("US"));
	REQUIRE( output->getRange() != 0);//Just check if the output volume is empty

	vtkImageDataPtr volume = output->getGrayScaleVtkImageData();
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
	REQUIRE(volumePtr); //Check if the pointer != NULL
}

void ReconstructRealTestData::validateAngioData(cx::ImagePtr angioOut)
{
	this->validateData(angioOut);

	CHECK(angioOut->getImageType().contains("Angio"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(angioOut, 38, 146, 146) == 1);
	CHECK(this->getValue(angioOut, 94, 148, 135) == 1);
	CHECK(this->getValue(angioOut, 144, 152, 130) == 1);
	CHECK(this->getValue(angioOut, 237, 161, 119) == 1);
	CHECK(this->getValue(angioOut, 278, 160, 113) == 1);
	CHECK(this->getValue(angioOut, 248, 149, 200) == 1);

	// black points at random positions outside cross
	CHECK(this->getValue(angioOut, 242, 125, 200) == 1);
	CHECK(this->getValue(angioOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(angioOut, 143, 152, 172)  > 1); // correction
	CHECK(this->getValue(angioOut, 179, 142, 170) == 1); //
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(angioOut, 334,  96,  86) > 200 );
	CHECK(this->getValue(angioOut, 319,  95,  85) > 200 );
	CHECK(this->getValue(angioOut, 316, 105,  72) == 1);
	CHECK(this->getValue(angioOut, 317,  98,  44) == 1);
	CHECK(this->getValue(angioOut, 316, 108,  65) == 1);
}

void ReconstructRealTestData::validateBModeData(cx::ImagePtr bmodeOut)
{
	this->validateData(bmodeOut);

	REQUIRE(bmodeOut->getImageType().contains("B-Mode"));

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	CHECK(this->getValue(bmodeOut, 38, 146, 146) > 200);
	CHECK(this->getValue(bmodeOut, 94, 148, 135) > 200);
	CHECK(this->getValue(bmodeOut, 144, 152, 130) > 200);
	CHECK(this->getValue(bmodeOut, 237, 161, 119) > 190);
	CHECK(this->getValue(bmodeOut, 278, 160, 113) > 200);
	CHECK(this->getValue(bmodeOut, 248, 149, 200) > 200);

	// black points at random positions outside cross
	CHECK(this->getValue(bmodeOut, 242, 125, 200) == 1);
	CHECK(this->getValue(bmodeOut, 233, 138, 141) == 1);
	// one sample in a flash and a black sample just outside it.
	CHECK(this->getValue(bmodeOut, 143, 152, 172)  > 1); // correction
	CHECK(this->getValue(bmodeOut, 179, 142, 170) == 1); //
	// two samples in a flash and three black samples just outside it.
	CHECK(this->getValue(bmodeOut, 334,  96,  86) > 170 );
	CHECK(this->getValue(bmodeOut, 319,  95,  85) > 200 );
	CHECK(this->getValue(bmodeOut, 316, 105,  72) == 1);
	CHECK(this->getValue(bmodeOut, 317,  98,  44) == 1);
	CHECK(this->getValue(bmodeOut, 316, 108,  65) == 1);
}

int ReconstructRealTestData::getValue(cx::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}


} // namespace cxtest

