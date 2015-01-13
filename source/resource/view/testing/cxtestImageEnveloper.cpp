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
#include "catch.hpp"
#include "cxImageEnveloper.h"
#include "cxtestUtilities.h"
#include "cxImage.h"
#include "vtkImageData.h"
#include "cxTypeConversions.h"
#include "cxRegistrationTransform.h"
#include "cxVolumeHelpers.h"
#include "cxImageParameters.h"

namespace cxtest
{

namespace  {


Eigen::Array3i getDim(cx::ImagePtr input)
{
	return Eigen::Array3i(input->getBaseVtkImageData()->GetDimensions());
}

cx::Vector3D getSpacing(cx::ImagePtr input)
{
	return cx::Vector3D(input->getBaseVtkImageData()->GetSpacing());
}

cx::ImagePtr createExpectedImage(cx::ImageParameters params)
{
	vtkImageDataPtr imageData = cx::generateVtkImageData(params.getDim(), params.getSpacing(), 0, 1);

	cx::ImagePtr retval(new cx::Image("expected_image", imageData));
	retval->get_rMd_History()->setRegistration(params.m_rMd);
	retval->get_rMd_History()->setParentSpace(params.mParentVolume);
	retval->setAcquisitionTime(QDateTime::currentDateTime());
	retval->setModality("SC");
	return retval;
}

//cx::ImagePtr createExpectedImage(Eigen::Array3i dim, cx::Vector3D spacing, QString parent, cx::Transform3D rMd)
//{
//	vtkImageDataPtr imageData = cx::generateVtkImageData(dim, spacing, 0, 1);

//	cx::ImagePtr retval(new cx::Image("expected_image", imageData));
//	retval->get_rMd_History()->setRegistration(rMd);
//	retval->get_rMd_History()->setParentSpace(parent);
//	retval->setAcquisitionTime(QDateTime::currentDateTime());
//	retval->setModality("SC");
//	return retval;
//}

void checkImages(cx::ImagePtr input, cx::ImagePtr expected)
{
	{
		INFO(getDim(input) << " == " << getDim(expected));
		CHECK(cx::similar(getDim(input), getDim(expected)));
	}
	{
		INFO(input->get_rMd() << "\n == \n" << expected->get_rMd());
		CHECK(cx::similar(input->get_rMd(), expected->get_rMd()));
	}
	{
		INFO(getSpacing(input) << " == " << getSpacing(expected));
		CHECK(cx::similar(getSpacing(input), getSpacing(expected)));
	}
	CHECK(input->getParentSpace() == expected->getParentSpace());
	// removed: image is not necessarily black. The size is what matters.
	//CHECK(cxtest::Utilities::getFractionOfVoxelsAboveThreshold(input->getBaseVtkImageData(), 0) == Approx(0));
}

} // namespace


TEST_CASE("ImageEnveloper: One image", "[unit][resource][visualization]")
{
	unsigned int size = 3;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200);

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();

	enveloper->setImages(std::vector<cx::ImagePtr>(1, image));
	cx::ImagePtr box = enveloper->getEnvelopingImage();

	cx::ImageParameters expectedImageParameters(Eigen::Array3i(size, size, size),
												getSpacing(image),
												image->getUid(),
												image->get_rMd());

	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: Two nonoverlapping images", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	int shift = 10;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(2, Eigen::Array3i(size,size,size), 200);
	cx::Transform3D shiftTransform = cx::createTransformTranslate(cx::Vector3D(shift,shift,0));
	images[1]->get_rMd_History()->setRegistration(shiftTransform);

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	cx::ImageParameters expectedImageParameters(Eigen::Array3i(size+shift, size+shift, size),
																							getSpacing(images[0]),
																							images[0]->getUid(),
																							cx::Transform3D::Identity() );
	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: Two nonoverlapping images, negative shift", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	int shift = -10;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(2, Eigen::Array3i(size,size,size), 200);
	cx::Transform3D shiftTransform = cx::createTransformTranslate(cx::Vector3D(shift,shift,0));
	images[1]->get_rMd_History()->setRegistration(shiftTransform);

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	cx::ImageParameters expectedImageParameters(Eigen::Array3i(size-shift, size-shift, size),
																							getSpacing(images[0]),
																							images[0]->getUid(),
																							shiftTransform );

	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: Two overlapping images", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	int shift = 2;
	std::vector<cx::ImagePtr> images = cxtest::Utilities::create3DImages(2, Eigen::Array3i(size,size,size), 200);
	images[1]->get_rMd_History()->setRegistration(cx::createTransformTranslate(cx::Vector3D(shift,shift,0)));

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	cx::ImageParameters expectedImageParameters(Eigen::Array3i(size+shift, size+shift, size),
																							getSpacing(images[0]),
																							images[0]->getUid(),
																							images[0]->get_rMd()
																							);
	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: One image inside another", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	std::vector<cx::ImagePtr> images;
	images.push_back(cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200));
	size = 10;
	images.push_back(cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200));

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	cx::ImageParameters expectedImageParameters(Eigen::Array3i(size, size, size),
																							getSpacing(images[0]),
																							images[0]->getUid(),
																							images[0]->get_rMd()
																							);
	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: One high-res image inside another", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	std::vector<cx::ImagePtr> images;
	images.push_back(cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200));
	unsigned int highResSize = 10;
	double spacing = 0.1;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(highResSize,highResSize,highResSize), 200);
	image->getBaseVtkImageData()->SetSpacing(spacing, spacing, spacing);
	images.push_back(image);

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	double expectedExtent = size - 1;
	double expSize = expectedExtent/spacing + 1;
	cx::ImageParameters expectedImageParameters(Eigen::Array3i(expSize, expSize, expSize),
																							cx::Vector3D(spacing, spacing, spacing),
																							images[0]->getUid(),
																							images[0]->get_rMd());
	cx::ImagePtr expected = createExpectedImage(expectedImageParameters);
	checkImages(box, expected);
}

TEST_CASE("ImageEnveloper: One rotated high-res image inside another", "[unit][resource][visualization]")
{
	unsigned int size = 5;
	std::vector<cx::ImagePtr> images;
	images.push_back(cxtest::Utilities::create3DImage(Eigen::Array3i(size,size,size), 200));

	unsigned int highResSize = 11;
	double spacing = 0.1;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(highResSize,highResSize,highResSize), 200);
	image->getBaseVtkImageData()->SetSpacing(spacing, spacing, spacing);
	image->get_rMd_History()->setRegistration(cx::createTransformRotateZ(M_PI / 4.0));
	images.push_back(image);

	cx::ImageEnveloperPtr enveloper = cx::ImageEnveloper::create();
	enveloper->setImages(images);

	cx::ImagePtr box = enveloper->getEnvelopingImage();

	double extent = size-1;
	double shift = 1/sqrt(2.0);
//	Eigen::Array3d expectedExtent(extent + shift, extent, extent);
//	Eigen::Array3d expectedSpacing(spacing*shift, spacing*shift, spacing);
//	Eigen::Array3i expectedDims = cx::ImageEnveloper::getDimFromExtent(expectedExtent, expectedSpacing);

	cx::ImageParameters expectedParameters;
	expectedParameters.setSpacingKeepDim(Eigen::Array3d(spacing*shift, spacing*shift, spacing));
	expectedParameters.setDimKeepBoundsAlignSpacing(Eigen::Array3d(extent + shift, extent, extent));
	expectedParameters.mParentVolume = images[0]->getUid();
	expectedParameters.m_rMd = cx::createTransformTranslate(cx::Vector3D(-shift,0,0));

//	cx::ImagePtr expected = createExpectedImage(expectedDims,
//																							expectedSpacing.matrix(),
//																							images[0]->getUid(),
//																							shiftTransform);
	cx::ImagePtr expected = createExpectedImage(expectedParameters);

	checkImages(box, expected);
}


} // namespace cxtest


