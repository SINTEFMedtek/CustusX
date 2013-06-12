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

namespace cxtest
{

vtkImageDataPtr Utilities::create3DVtkImageData()
{
	vtkImageDataPtr vtkImageData = vtkImageDataPtr::New();

	vtkImageData->SetDimensions(10, 10, 10);
	vtkImageData->SetNumberOfScalarComponents(1);

	int* dims = vtkImageData->GetDimensions();

	for (int z  = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				vtkImageData->SetScalarComponentFromDouble(x, y, z, 0, 2.0);
			}
		}
	}
//	vtkImageData->Print(std::cout);

	return vtkImageData;
}

ssc::ImagePtr Utilities::create3DImage()
{
	QString imagesUid("TESTUID");
	vtkImageDataPtr vtkImageData = create3DVtkImageData();
	ssc::ImagePtr image(new ssc::Image(imagesUid, vtkImageData));

	return image;
}

} /* namespace cxtest */
