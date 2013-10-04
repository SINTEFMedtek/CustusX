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

#include "cxtestUtilities.h"

#include "vtkImageData.h"
#include "sscImage.h"
#include "sscVolumeHelpers.h"
#include "sscTypeConversions.h"
#include "cxDataLocations.h"

namespace cxtest
{
/*
// --------------------------------------------------------
TestDataStorage* TestDataStorage::mInstance = NULL; ///< static member
// --------------------------------------------------------
void TestDataStorage::shutdown()
{
	delete mInstance;
	mInstance = NULL;
}
TestDataStorage* TestDataStorage::getInstance()
{
    if (!mInstance)
    	mInstance = new TestDataStorage();
	return mInstance;
}
// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
*/

QString Utilities::getDataRoot(QString suffix)
{
	QString root = cx::DataLocations::getTestDataPath();
	if (suffix.isEmpty())
		return root;
	else
		return QString("%1/%2").arg(root).arg(suffix);
}


vtkImageDataPtr Utilities::create3DVtkImageData(Eigen::Array3i dim, const unsigned int voxelValue)
{
	return cx::generateVtkImageData(dim, cx::Vector3D(1,1,1), voxelValue);
}

cx::ImagePtr Utilities::create3DImage(Eigen::Array3i dim, const unsigned int voxelValue)
{
	vtkImageDataPtr vtkImageData = create3DVtkImageData(dim, voxelValue);
	QString unique_string = qstring_cast(reinterpret_cast<long>(vtkImageData.GetPointer()));
	QString imagesUid = QString("TESTUID_%2_%1").arg(unique_string);
	cx::ImagePtr image(new cx::Image(imagesUid, vtkImageData));

	return image;
}

std::vector<cx::ImagePtr> Utilities::create3DImages(unsigned int count, Eigen::Array3i dim, const unsigned int voxelValue)
{
	std::vector<cx::ImagePtr> retval;
	for (unsigned i=0; i<count; ++i)
	{
		cx::ImagePtr image = cxtest::Utilities::create3DImage(dim, voxelValue);
		retval.push_back(image);
	}
	return retval;
}

unsigned int Utilities::getNumberOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold)
{
	if (!image)
		return 0;
	unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
	unsigned int pixelCount = 0;
	for (unsigned i = 0; i < image->GetDimensions()[0]*image->GetDimensions()[1]*image->GetDimensions()[2]; ++i)
	{
		if (ptr[i*image->GetNumberOfScalarComponents()] > threshold)
			++pixelCount;
	}
	return pixelCount;
}

unsigned int Utilities::getNumberOfNonZeroVoxels(vtkImageDataPtr image)
{
	return getNumberOfVoxelsAboveThreshold(image, 0);
//	if (!image)
//		return 0;
//	unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
//	unsigned int pixelCount = 0;
//	for (unsigned i = 0; i < image->GetDimensions()[0]*image->GetDimensions()[1]; ++i)
//	{
//		if (ptr[i*image->GetNumberOfScalarComponents()] != 0)
//			++pixelCount;
//	}
//	return pixelCount;
}

double Utilities::getFractionOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold)
{
	unsigned int hits = getNumberOfVoxelsAboveThreshold(image, threshold);
	Eigen::Array3i dim(image->GetDimensions());
	unsigned int totalPixels = dim[0]*dim[1]*dim[2];
	if (totalPixels==0)
		return -1;
	return double(hits)/double(totalPixels);
}

} /* namespace cxtest */
