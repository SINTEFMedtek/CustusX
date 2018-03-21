/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestUtilities.h"

#include "vtkImageData.h"
#include "cxImage.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxDataLocations.h"

#ifdef CX_WINDOWS
	#include <Windows.h>
#endif

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
	return create3DImage(dim, cx::Vector3D(1,1,1), voxelValue);
}

cx::ImagePtr Utilities::create3DImage(Eigen::Array3i dim, cx::Vector3D spacing, const unsigned int voxelValue)
{
	vtkImageDataPtr vtkImageData = cx::generateVtkImageData(dim, spacing, voxelValue);
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

unsigned int Utilities::getNumberOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold, int component)
{
	if (!image)
		return 0;

	unsigned char* ptr = reinterpret_cast<unsigned char*>(image->GetScalarPointer());
	unsigned int pixelCount = 0;
	for (unsigned i = 0; i < image->GetDimensions()[0]*image->GetDimensions()[1]*image->GetDimensions()[2]; ++i)
	{
		if (ptr[i*image->GetNumberOfScalarComponents()+component] > threshold)
			++pixelCount;
	}
	return pixelCount;
}

unsigned int Utilities::getNumberOfNonZeroVoxels(vtkImageDataPtr image)
{
	return getNumberOfVoxelsAboveThreshold(image, 0);
}

double Utilities::getFractionOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold, int component)
{
	unsigned int hits = getNumberOfVoxelsAboveThreshold(image, threshold, component);
	Eigen::Array3i dim(image->GetDimensions());
	unsigned int totalPixels = dim[0]*dim[1]*dim[2];
	if (totalPixels==0)
		return -1;
	return double(hits)/double(totalPixels);
}

void Utilities::sleep_sec(int seconds)
{
#ifndef CX_WINDOWS
		sleep(seconds); //seconds
#else
		Sleep(seconds*1000); //milliseconds
#endif
}

} /* namespace cxtest */
