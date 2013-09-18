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

} /* namespace cxtest */
