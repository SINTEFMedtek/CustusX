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
#include <vtkImageData.h>
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxDataReaderWriter.h"
#include "cxImageTF3D.h"
#include "cxTransferFunctions3DPresets.h"

#include "cxProfile.h"

namespace
{

cx::ImagePtr readTestImage(QString uid, QString filename)
{
	//Copied from loadImageFromFile() in cxtestDicomConverter.cpp
	cx::ImagePtr image = cx::Image::create(uid,uid);
	cx::MetaImageReader().readInto(image, filename);
	return image;
}

cx::ImagePtr readTestImage()
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/BoatPhantom/MetaImage/baatFantom.mhd";
	return readTestImage(uid, filename);
}

cx::ImagePtr readKaisaTestImage()
{
	QString uid = "kaisaTestImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/MetaImage/Kaisa.mhd";
	return readTestImage(uid, filename);
}

class TransferFunctionPresetsHelper
{
public:
	TransferFunctionPresetsHelper()
	{
	cx::XmlOptionFile preset(cx::DataLocations::findConfigFilePath("presets.xml", "/transferFunctions"));
	cx::XmlOptionFile custom = cx::profile()->getXmlSettings().descend("presetTransferFunctions");
	transferFunctionPresets = cx::TransferFunctions3DPresetsPtr(new cx::TransferFunctions3DPresets(preset, custom));
	REQUIRE(transferFunctionPresets);

	}
	void changeToNonExistingTransferFunction(cx::ImagePtr image)
	{
		QString transferFunctionName = "non existing transfer function";
		transferFunctionPresets->load3D(transferFunctionName, image);
	}

	void changeToExistingTransferFunction(cx::ImagePtr image)
	{
		QString transferFunctionName = "3D CT Abdomen";
		transferFunctionPresets->load3D(transferFunctionName, image);
	}

	void checkInitialWindow(cx::ImagePtr image, double initialWindowWidth, double initialWindowlevel, bool transferFunctionChanged = false)
	{
		CHECK(image->getInitialWindowWidth() == initialWindowWidth);
		CHECK(image->getInitialWindowLevel() == initialWindowlevel);
		cx::ImageTF3DPtr transferFunction = image->getTransferFunctions3D();
		if(transferFunctionChanged)
		{
			CHECK(image->getInitialWindowWidth() != transferFunction->getWindow());
			CHECK(image->getInitialWindowLevel() != transferFunction->getLevel());
		}
		else
		{
			CHECK(image->getInitialWindowWidth() == transferFunction->getWindow());
			CHECK(image->getInitialWindowLevel() == transferFunction->getLevel());
		}
	}

	void runAddAndParseXml(cx::ImagePtr image)
	{
		QDomDocument domdoc;
		QDomElement node = domdoc.createElement("Image test");
		image->addXml(node);
		image->parseXml(node);
	}

	private:
	cx::TransferFunctions3DPresetsPtr transferFunctionPresets;
};

} //namespace

namespace cxtest
{

TEST_CASE("Image copy: id copied", "[unit][resource][core]")
{
	cx::ImagePtr image = readTestImage();
	cx::ImagePtr imageCopy = image->copy();
	REQUIRE(image->getUid() == imageCopy->getUid());
}

TEST_CASE("Image copy: vtkImage copied", "[unit][resource][core]")
{
	cx::ImagePtr image = readTestImage();
	cx::ImagePtr imageCopy = image->copy();
	vtkImageDataPtr vtkImage = image->getBaseVtkImageData();
	vtkImageDataPtr vtkImageCopy = imageCopy->getBaseVtkImageData();
	REQUIRE(vtkImageCopy);
	REQUIRE(vtkImage != vtkImageCopy);

	int* dimsCopy = vtkImageCopy->GetDimensions();
	int* dims = vtkImage->GetDimensions();
	REQUIRE(dims[0] == dimsCopy[0]);
	REQUIRE(dims[1] == dimsCopy[1]);
	REQUIRE(dims[2] == dimsCopy[2]);
}

TEST_CASE("Image copy: Voxels equal", "[unit][resource][core]")
{
	cx::ImagePtr image = readTestImage();
	cx::ImagePtr imageCopy = image->copy();
	vtkImageDataPtr vtkImage = image->getBaseVtkImageData();
	vtkImageDataPtr vtkImageCopy = imageCopy->getBaseVtkImageData();

	unsigned char* voxelCopy = static_cast<unsigned char*>(vtkImageCopy->GetScalarPointer());
	unsigned char* voxel = static_cast<unsigned char*>(vtkImage->GetScalarPointer());
	int offset = 150*130*120;
	int voxelsAboveZero = 0;
	for (int i = 0; i < 5000; ++i)
	{
		if(*(voxel+i+offset) > 0)
			++voxelsAboveZero;
		REQUIRE(*(voxel+i+offset) == *(voxelCopy+i+offset));
	}
//	std::cout << "Voxels larger than zero: " << voxelsAboveZero << std::endl;
	REQUIRE(voxelsAboveZero > 1000);
}

TEST_CASE("Image initial window imported", "[unit][resource][core]")
{
	cx::ImagePtr image = readKaisaTestImage();
	double windowWidth = image->getInitialWindowWidth();
	double windowLevel = image->getInitialWindowLevel();
	REQUIRE(windowWidth > 0);
	REQUIRE(windowLevel > 0);
}

TEST_CASE("Image initial window is kept after changing transfer function", "[unit][resource][core]")
{
	cx::ImagePtr image = readKaisaTestImage();
	image->resetTransferFunctions();
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();
	REQUIRE(initialWindowWidth > 0);

	cx::ImageTF3DPtr transferFunction = image->getTransferFunctions3D();
	image->setTransferFunctions3D(transferFunction);

	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.changeToNonExistingTransferFunction(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.changeToExistingTransferFunction(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel, true);

	image->resetTransferFunctions();
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);
}

TEST_CASE("Image: Initial window is kept after using UnsignedDerivedImage", "[unit][resource][core]")
{
	cx::ImagePtr image = readKaisaTestImage();
	image->resetTransferFunctions();
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();

	cx::ImagePtr unsignedImage = image->getUnsigned(image);
	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.changeToNonExistingTransferFunction(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.changeToExistingTransferFunction(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel, true);

	image->resetTransferFunctions();
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);
}

TEST_CASE("Image: Initial window from mdh file is kept after using addXml and parseXml", "[unit][resource][core]")
{
	cx::ImagePtr image = readKaisaTestImage();
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();

	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.runAddAndParseXml(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);
}

TEST_CASE("Image: Changed initial window is kept after using addXml and parseXml", "[unit][resource][core]")
{
	cx::ImagePtr image = readKaisaTestImage();
	image->setInitialWindowLevel(20, 30);
	image->resetTransferFunctions();
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();

	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.runAddAndParseXml(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);
}

} // namespace cxtest
