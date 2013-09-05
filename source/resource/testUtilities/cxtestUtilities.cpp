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

namespace cxtest
{

vtkImageDataPtr Utilities::create3DVtkImageData(Eigen::Array3i dim)
{
	return ssc::generateVtkImageData(dim, ssc::Vector3D(1,1,1), 2);
}

ssc::ImagePtr Utilities::create3DImage(Eigen::Array3i dim)
{
	vtkImageDataPtr vtkImageData = create3DVtkImageData(dim);
	QString unique_string = qstring_cast(reinterpret_cast<long>(vtkImageData.GetPointer()));
	QString imagesUid = QString("TESTUID_%2_%1").arg(unique_string);
	ssc::ImagePtr image(new ssc::Image(imagesUid, vtkImageData));

	return image;
}

} /* namespace cxtest */
