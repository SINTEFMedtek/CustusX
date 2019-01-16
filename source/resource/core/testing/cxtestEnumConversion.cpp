/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxDefinitions.h"
#include "cxCustomMetaImage.h"
#include "cxEnumConversion.h"

namespace cxtest
{

TEST_CASE("ImageModality conversions, emum to string", "[unit][resource][core][enum]")
{
	CHECK(enum2string(cx::modUNKNOWN) == "UNKNOWN");
	CHECK(enum2string(cx::modCT) == "CT");
	CHECK(enum2string(cx::modMR) == "MR");
	CHECK(enum2string(cx::modUS) == "US");
	CHECK(enum2string(cx::modPET) == "PET");
	CHECK(enum2string(cx::modCOUNT) == "UNDEFINED");
}

TEST_CASE("ImageModality conversions, string to enum", "[unit][resource][core][enum]")
{
	CHECK(string2enum<cx::IMAGE_MODALITY>("CT") == cx::modCT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("MR") == cx::modMR);
	CHECK(string2enum<cx::IMAGE_MODALITY>("US") == cx::modUS);
	CHECK(string2enum<cx::IMAGE_MODALITY>("PET") == cx::modPET);

	CHECK(string2enum<cx::IMAGE_MODALITY>("MRI") == cx::modCOUNT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("") == cx::modCOUNT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("test") == cx::modCOUNT);
}

TEST_CASE("ImageModality conversions, convertToModality function", "[unit][resource][core][enum]")
{
	CHECK(cx::convertToModality("CT") == cx::modCT);
	CHECK(cx::convertToModality("MR") == cx::modMR);
	CHECK(cx::convertToModality("US") == cx::modUS);
	CHECK(cx::convertToModality("PET") == cx::modPET);

	CHECK(cx::convertToModality("MRI") == cx::modMR);
	CHECK(cx::convertToModality("") == cx::modUNKNOWN);
	CHECK(cx::convertToModality("test") == cx::modUNKNOWN);
	CHECK(cx::convertToModality("UNDEFINED") == cx::modUNKNOWN);
}

TEST_CASE("Image sub type conversions, convertToImageSubType function", "[unit][resource][core][enum]")
{
	CHECK(cx::convertToImageSubType("") == cx::isEMPTY);
	CHECK(cx::convertToImageSubType("T1") == cx::isMRT1);
	CHECK(cx::convertToImageSubType("t1_image") == cx::isMRT1);
	CHECK(cx::convertToImageSubType("T2") == cx::isMRT2);
	CHECK(cx::convertToImageSubType("FLAIR") == cx::isMRFLAIR);
	CHECK(cx::convertToImageSubType("flair") == cx::isMRFLAIR);

	CHECK(cx::convertToImageSubType("b-mode") == cx::isUSBMODE);
	CHECK(cx::convertToImageSubType("b_mode") == cx::isUSBMODE);
	CHECK(cx::convertToImageSubType("bmode") == cx::isUSBMODE);
	CHECK(cx::convertToImageSubType("B-mode") == cx::isUSBMODE);
	CHECK(cx::convertToImageSubType("ultrasound B mode") == cx::isUSBMODE);

	CHECK(cx::convertToImageSubType("angio") == cx::isANGIO);
	CHECK(cx::convertToImageSubType("Angio") == cx::isANGIO);
	CHECK(cx::convertToImageSubType("Segmentation") == cx::isSEGMENTATION);
	CHECK(cx::convertToImageSubType("seg") == cx::isSEGMENTATION);
	CHECK(cx::convertToImageSubType("label") == cx::isSEGMENTATION);
}

}//cxtest
