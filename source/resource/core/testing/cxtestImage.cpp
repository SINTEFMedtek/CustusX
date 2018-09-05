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
#include "cxImageTF3D.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxVolumeHelpers.h"

#include "cxProfile.h"
#include "cxLogicManager.h"
#include "cxFileManagerServiceProxy.h"
#include "cxNIfTIReader.h"

namespace
{

cx::ImagePtr readNIfTITestImage(QString uid, QString filename, cx::FileManagerServicePtr filemanager)
{
	cx::ImagePtr image = cx::Image::create(uid,uid);
		filemanager->readInto(image, filename);
	return image;
}

cx::ImagePtr readMhdTestImage(QString uid, QString filename, cx::FileManagerServicePtr filemanager)
{
	cx::ImagePtr image = cx::Image::create(uid,uid);
	filemanager->readInto(image, filename);
	return image;
}

cx::ImagePtr readNIfTITestImage(cx::FileManagerServicePtr filemanager)
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/testing/NIfTI/Case1-T1.nii";
	return readNIfTITestImage(uid, filename, filemanager);
}

cx::ImagePtr readMhdTestImage(cx::FileManagerServicePtr filemanager)
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/BoatPhantom/MetaImage/baatFantom.mhd";
	return readMhdTestImage(uid, filename, filemanager);
}

cx::ImagePtr readKaisaTestImage(cx::FileManagerServicePtr filemanager)
{
	QString uid = "kaisaTestImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/Kaisa/MetaImage/Kaisa.mhd";
	return readMhdTestImage(uid, filename, filemanager);
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
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readNIfTITestImage(filemanager);
	REQUIRE(((int)image->getBaseVtkImageData()->GetActualMemorySize()) > 0);
	cx::LogicManager::shutdown();
}

TEST_CASE("Can load Mhd files as images", "[unit][mhd][core][resource]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readMhdTestImage(filemanager);
	REQUIRE(((int)image->getBaseVtkImageData()->GetActualMemorySize()) > 0);
	cx::LogicManager::shutdown();
}

TEST_CASE("Image copy: id copied", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readMhdTestImage(filemanager);
	cx::ImagePtr imageCopy = image->copy();
	REQUIRE(image->getUid() == imageCopy->getUid());

	cx::LogicManager::shutdown();
}

TEST_CASE("Image copy: vtkImage copied", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readMhdTestImage(filemanager);
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

	cx::LogicManager::shutdown();
}

TEST_CASE("Image copy: Voxels equal", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readMhdTestImage(filemanager);
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
	REQUIRE(voxelsAboveZero > 1000);

	cx::LogicManager::shutdown();
}

TEST_CASE("Image initial window imported", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readKaisaTestImage(filemanager);
	double windowWidth = image->getInitialWindowWidth();
	double windowLevel = image->getInitialWindowLevel();
	REQUIRE(windowWidth > 0);
	REQUIRE(windowLevel > 0);

	cx::LogicManager::shutdown();
}

TEST_CASE("Image initial window is kept after changing transfer function", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readKaisaTestImage(filemanager);
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

	cx::LogicManager::shutdown();
}

TEST_CASE("Image: Initial window is kept after using UnsignedDerivedImage", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readKaisaTestImage(filemanager);
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

	cx::LogicManager::shutdown();
}

TEST_CASE("Image: Initial window from mdh file is kept after using addXml and parseXml", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readKaisaTestImage(filemanager);
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();

	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.runAddAndParseXml(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	cx::LogicManager::shutdown();
}

TEST_CASE("Image: Changed initial window is kept after using addXml and parseXml", "[unit][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cx::ImagePtr image = readKaisaTestImage(filemanager);
	image->setInitialWindowLevel(20, 30);
	image->resetTransferFunctions();
	double initialWindowWidth = image->getInitialWindowWidth();
	double initialWindowlevel = image->getInitialWindowLevel();

	TransferFunctionPresetsHelper helper;
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	helper.runAddAndParseXml(image);
	helper.checkInitialWindow(image, initialWindowWidth, initialWindowlevel);

	cx::LogicManager::shutdown();
}

} // namespace cxtest
