/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

cx::ImagePtr readNIfTITestImage(QString uid, QString filename)
{
	cx::ImagePtr image = cx::Image::create(uid,uid);
	cx::NIfTIReader().readInto(image, filename);
	return image;
}

cx::ImagePtr readMhdTestImage(QString uid, QString filename)
{
	//Copied from loadImageFromFile() in cxtestDicomConverter.cpp
	cx::ImagePtr image = cx::Image::create(uid,uid);
	cx::MetaImageReader().readInto(image, filename);
	return image;
}

cx::ImagePtr readNIfTITestImage()
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/testing/NIfTI/Case1-T1.nii";
	return readNIfTITestImage(uid, filename);
}

cx::ImagePtr readMhdTestImage()
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/BoatPhantom/MetaImage/baatFantom.mhd";
	return readMhdTestImage(uid, filename);
}

cx::ImagePtr readKaisaTestImage()
{
	QString uid = "kaisaTestImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/MetaImage/Kaisa.mhd";
	return readMhdTestImage(uid, filename);
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
TEST_CASE("Can load NIfTi files as images", "[unit][nifti][core][resource]")
{
	cx::ImagePtr image = readNIfTITestImage();
	REQUIRE(((int)image->getBaseVtkImageData()->GetActualMemorySize()) > 0);
}

TEST_CASE("Can load Mhd files as images", "[unit][mhd][core][resource]")
{
	cx::ImagePtr image = readMhdTestImage();
	REQUIRE(((int)image->getBaseVtkImageData()->GetActualMemorySize()) > 0);
}

TEST_CASE("Image copy: id copied", "[unit][resource][core]")
{
	cx::ImagePtr image = readMhdTestImage();
	cx::ImagePtr imageCopy = image->copy();
	REQUIRE(image->getUid() == imageCopy->getUid());
}

TEST_CASE("Image copy: vtkImage copied", "[unit][resource][core]")
{
	cx::ImagePtr image = readMhdTestImage();
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
	cx::ImagePtr image = readMhdTestImage();
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
