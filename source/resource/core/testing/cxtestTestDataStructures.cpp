/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestTestDataStructures.h"


namespace cxtest
{
TestDataStructures::TestDataStructures()
{
	vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(2, 1);
	image1 = cx::ImagePtr(new cx::Image("imageUid1", dummyImageData, "imageName1"));
	image2 = cx::ImagePtr(new cx::Image("imageUid2", dummyImageData, "imageName2"));
	mesh1 = cx::Mesh::create("meshUid1","meshName1");
	trackedStream1 = cx::TrackedStream::create("trackedStreamUid1","trackedStreamName1");
}

}//cxtest