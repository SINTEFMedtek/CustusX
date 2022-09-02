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
	CHECK(enum2string(cx::imUNKNOWN) == "UNKNOWN");
	CHECK(enum2string(cx::imCT) == "CT");
	CHECK(enum2string(cx::imMR) == "MR");
	CHECK(enum2string(cx::imUS) == "US");
	CHECK(enum2string(cx::imPET) == "PET");
	CHECK(enum2string(cx::imCOUNT) == "UNDEFINED");
}

TEST_CASE("ImageModality conversions, string to enum", "[unit][resource][core][enum]")
{
	CHECK(string2enum<cx::IMAGE_MODALITY>("CT") == cx::imCT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("MR") == cx::imMR);
	CHECK(string2enum<cx::IMAGE_MODALITY>("US") == cx::imUS);
	CHECK(string2enum<cx::IMAGE_MODALITY>("PET") == cx::imPET);

	CHECK(string2enum<cx::IMAGE_MODALITY>("MRI") == cx::imCOUNT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("") == cx::imCOUNT);
	CHECK(string2enum<cx::IMAGE_MODALITY>("test") == cx::imCOUNT);
}

TEST_CASE("ImageModality conversions, convertToModality function", "[unit][resource][core][enum]")
{
	CHECK(cx::convertToModality("CT") == cx::imCT);
	CHECK(cx::convertToModality("MR") == cx::imMR);
	CHECK(cx::convertToModality("US") == cx::imUS);
	CHECK(cx::convertToModality("PET") == cx::imPET);

	CHECK(cx::convertToModality("MRI") == cx::imMR);
	CHECK(cx::convertToModality("") == cx::imUNKNOWN);
	CHECK(cx::convertToModality("test") == cx::imUNKNOWN);
	CHECK(cx::convertToModality("UNDEFINED") == cx::imUNKNOWN);
}

TEST_CASE("Image sub type conversions, convertToImageSubType function", "[unit][resource][core][enum]")
{
	CHECK(cx::convertToImageSubType("") == cx::istEMPTY);
	CHECK(cx::convertToImageSubType("T1") == cx::istMRT1);
	CHECK(cx::convertToImageSubType("t1_image") == cx::istMRT1);
	CHECK(cx::convertToImageSubType("T2") == cx::istMRT2);
	CHECK(cx::convertToImageSubType("FLAIR") == cx::istMRFLAIR);
	CHECK(cx::convertToImageSubType("flair") == cx::istMRFLAIR);

	CHECK(cx::convertToImageSubType("b-mode") == cx::istUSBMODE);
	CHECK(cx::convertToImageSubType("b_mode") == cx::istUSBMODE);
	CHECK(cx::convertToImageSubType("bmode") == cx::istUSBMODE);
	CHECK(cx::convertToImageSubType("B-mode") == cx::istUSBMODE);
	CHECK(cx::convertToImageSubType("ultrasound B mode") == cx::istUSBMODE);

	CHECK(cx::convertToImageSubType("angio") == cx::istANGIO);
	CHECK(cx::convertToImageSubType("Angio") == cx::istANGIO);
	CHECK(cx::convertToImageSubType("Segmentation") == cx::istSEGMENTATION);
	CHECK(cx::convertToImageSubType("seg") == cx::istSEGMENTATION);
	CHECK(cx::convertToImageSubType("label") == cx::istSEGMENTATION);
}

TEST_CASE("Lung structures conversions", "[unit][resource][core][enum]")
{
	CHECK(enum2string(cx::lsUNKNOWN) == "UNKNOWN");
	CHECK(enum2string(cx::lsLUNG) == "Lungs");
	CHECK(enum2string(cx::lsLESIONS) == "Lesions");

	CHECK(string2enum<cx::LUNG_STRUCTURES>("Lungs") == cx::lsLUNG);
	CHECK(string2enum<cx::LUNG_STRUCTURES>("Lesions") == cx::lsLESIONS);
}

}//cxtest
