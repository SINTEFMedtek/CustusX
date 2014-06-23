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

#include "catch.hpp"
#include "cxImage.h"
#include "cxDataLocations.h"
#include "cxDataReaderWriter.h"
#include <vtkImageData.h>

namespace
{
cx::ImagePtr readTestImage()
{
	QString uid = "testImage";
	QString filename = cx::DataLocations::getTestDataPath()+"/Phantoms/BoatPhantom/MetaImage/baatFantom.mhd";

	//Copied from loadImageFromFile() in cxtestDicomConverter.cpp
	cx::ImagePtr image = cx::Image::create(uid,uid);
	cx::MetaImageReader().readInto(image, filename);
	return image;
}
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

} // namespace cxtest
