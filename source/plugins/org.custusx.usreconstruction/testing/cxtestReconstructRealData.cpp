/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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
	REQUIRE(output->getModality() == cx::imUS);
	REQUIRE( output->getRange() != 0);//Just check if the output volume is empty

	vtkImageDataPtr volume = output->getGrayScaleVtkImageData();
	unsigned char* volumePtr = reinterpret_cast<unsigned char*>(volume->GetScalarPointer());
	REQUIRE(volumePtr); //Check if the pointer != NULL
}

void ReconstructRealTestData::validateAngioData(cx::ImagePtr angioOut)
{
	this->validateData(angioOut);

	CHECK(angioOut->getImageType() == cx::istANGIO);

	this->checkSamples(angioOut, getCrossVolumeSamples(), 1, true);
	this->checkSamples(angioOut, getLightVolumeSamples(), 100);
	this->checkSamples(angioOut, getDarkVolumeSamples(), 1, true);
}

void ReconstructRealTestData::validateBModeData(cx::ImagePtr bmodeOut)
{
	this->validateData(bmodeOut);

	REQUIRE(bmodeOut->getImageType() == cx::istUSBMODE);

	//All samples had to be moved abount -3 voxels along y and z axis after changing OutputVolumeParams.

	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
//	CHECK(this->getValue(bmodeOut, 38, 146, 146) > 200);
//	CHECK(this->getValue(bmodeOut, 94, 148, 135) > 200);
//	CHECK(this->getValue(bmodeOut, 144, 152, 130) > 200);
//	CHECK(this->getValue(bmodeOut, 237, 161, 119) > 190);
//	CHECK(this->getValue(bmodeOut, 278, 160, 113) > 200);
//	CHECK(this->getValue(bmodeOut, 248, 149, 200) > 200);

	//	// black points at random positions outside cross
//	CHECK(this->getValue(bmodeOut, 242, 125, 200) == 1);
//	CHECK(this->getValue(bmodeOut, 233, 138, 141) == 1);
//	// one sample in a flash and a black sample just outside it.
//	CHECK(this->getValue(bmodeOut, 143, 152, 172)  > 1); // correction
//	CHECK(this->getValue(bmodeOut, 179, 142, 170) == 1); //
//	// two samples in a flash and three black samples just outside it.
//	CHECK(this->getValue(bmodeOut, 334,  96,  86) > 170 );
//	CHECK(this->getValue(bmodeOut, 319,  95,  85) > 200 );
//	CHECK(this->getValue(bmodeOut, 316, 105,  72) == 1);
//	CHECK(this->getValue(bmodeOut, 317,  98,  44) == 1);
//	CHECK(this->getValue(bmodeOut, 316, 108,  65) == 1);

	this->checkSamples(bmodeOut, getCrossVolumeSamples(), 190);
	this->checkSamples(bmodeOut, getLightVolumeSamples(), 100);
	this->checkSamples(bmodeOut, getDarkVolumeSamples(), 1, true);
}

std::vector<int> ReconstructRealTestData::getCrossVolumeSamples()
{
	// this is the wire phantom cross: fire samples along one line and one on the other.
	// visible in bmode, invisible in angio.
	int array[] = {38,  143, 143,//alone lines in wire phantom
								 94,  145, 132,
								 144, 149, 127,
								 237, 158, 116,
								 278, 157, 110,
								 242, 146, 198,//needed new line point
								};
	std::vector<int> retval(array, array + sizeof(array) / sizeof(int));
	return retval;
}

std::vector<int> ReconstructRealTestData::getLightVolumeSamples()
{
	int array[] = { 143, 149, 169,//first flash
									334,  93,  83,//second flash
									319,  92,  82};
	std::vector<int> retval(array, array + sizeof(array) / sizeof(int));
	return retval;
}

std::vector<int> ReconstructRealTestData::getDarkVolumeSamples()
{
	int array[] = {242, 122, 197,// black points at random positions outside cross
								 233, 135, 138,
								 179, 139, 167,//just outside first flash
								 316, 102,  69,//just outside second flash
								 317,  95,  41,
								 316, 105,  62};
	std::vector<int> retval(array, array + sizeof(array) / sizeof(int));
	return retval;
}

void ReconstructRealTestData::checkSamples(cx::ImagePtr image, std::vector<int> samples, int threshold, bool matchThresholdExactly)
{
	for(std::vector<int>::iterator it = samples.begin(); it != samples.end(); )
	{
		int x = *it++;
		int y = *it++;
		int z = *it++;
		INFO("x: " << x << " y: " << y << " z: " << z);
		if(matchThresholdExactly)
			CHECK(this->getValue(image, x, y, z) == threshold);
		else
			CHECK(this->getValue(image, x, y, z) > threshold);
	}
}

int ReconstructRealTestData::getValue(cx::ImagePtr data, int x, int y, int z)
{
	vtkImageDataPtr volume = data->getGrayScaleVtkImageData();
	int val = (int)*reinterpret_cast<unsigned char*>(volume->GetScalarPointer(x,y,z));
	return val;
}


} // namespace cxtest

