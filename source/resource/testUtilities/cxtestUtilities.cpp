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
