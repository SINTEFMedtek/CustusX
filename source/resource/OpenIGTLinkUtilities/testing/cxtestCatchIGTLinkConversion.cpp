/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include "cxIGTLinkConversionImage.h"


#include "cxtestIGTLinkConversionFixture.h"

using namespace cx;

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: is created", "[unit][resource][OpenIGTLinkUtilities]")
{
	cx::IGTLinkConversion converter;
	CHECK(true);
}

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: Decode/encode image", "[unit][resource][OpenIGTLinkUtilities]")
{
	typedef std::vector<std::pair<Eigen::Array3i, int> > ValVectorType;
	ValVectorType values;
	values.push_back(std::make_pair(Eigen::Array3i( 0,20,2), 0));
	values.push_back(std::make_pair(Eigen::Array3i(10,20,0), 4));
	values.push_back(std::make_pair(Eigen::Array3i(20,20,2), 6));

	QDateTime time = QDateTime::currentDateTime();
	vtkImageDataPtr rawImage = cx::generateVtkImageData(Eigen::Array3i(100, 120, 10),
													cx::Vector3D(0.5, 0.6, 0.7),
													0);
	for (ValVectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
	{
		Eigen::Array3i p = iter->first;
		int val = iter->second;
		this->setValue(rawImage, p[0], p[1], p[2], val);
	}

	cx::ImagePtr input(new cx::Image("my_uid", rawImage));
	input->setAcquisitionTime(time);

	cx::IGTLinkConversionImage converter;
	igtl::ImageMessage::Pointer msg = converter.encode(input, pcsLPS);
	cx::ImagePtr output = converter.decode(msg);

	CHECK(output);
	CHECK(time == output->getAcquisitionTime());
	CHECK(input->getUid() == output->getUid());
	CHECK(cx::similar(cx::Vector3D(input->getBaseVtkImageData()->GetSpacing()), cx::Vector3D(output->getBaseVtkImageData()->GetSpacing())));
	REQUIRE(cx::similar(Eigen::Array3i(input->getBaseVtkImageData()->GetDimensions()), Eigen::Array3i(output->getBaseVtkImageData()->GetDimensions())));

	for (ValVectorType::iterator iter=values.begin(); iter!=values.end(); ++iter)
	{
		Eigen::Array3i p = iter->first;
		int val = iter->second;
		CHECK(this->getValue(input, p[0], p[1], p[2]) == val);
		CHECK(this->getValue(output, p[0], p[1], p[2]) == val);
	}
}

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: Decode/encode color image RGBA", "[unit][resource][OpenIGTLinkUtilities]")
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(255,  0,  0)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i(255,  0, 50)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i(255,  0, 62)));

	this->testDecodeEncodeColorImage(values, "RGBA");
}

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: Decode/encode color image BGR", "[unit][resource][OpenIGTLinkUtilities]")
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(  0,  0,255)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i( 50,  0,255)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i( 62,  0,255)));

	this->testDecodeEncodeColorImage(values, "BGR");
}

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: Decode/encode color image ARGB", "[unit][resource][OpenIGTLinkUtilities]")
{
	//testimage
	//	ptr[0] = 255;       // red
	//	ptr[1] = 0;         // green
	//	ptr[2] = x / 2;     // blue
	//	ptr[3] = 100;		// alpha

	Val3VectorType values;
	values.push_back(std::make_pair(Eigen::Array3i(  0,   0,  0), Eigen::Array3i(0,  0, 100)));
	values.push_back(std::make_pair(Eigen::Array3i(100,  50,  0), Eigen::Array3i(0, 50, 100)));
	values.push_back(std::make_pair(Eigen::Array3i(124,  20,  0), Eigen::Array3i(0, 62, 100)));

	this->testDecodeEncodeColorImage(values, "ARGB");
}

TEST_CASE_METHOD(IGTLinkConversionFixture, "IGTLinkConversion: Decode/encode ProbeDefinition", "[unit][resource][OpenIGTLinkUtilities]")
{
	// generate probe data input
	cx::ProbeDefinitionPtr input(new cx::ProbeDefinition());
	input->setType(cx::ProbeDefinition::tSECTOR);
	input->setOrigin_p(cx::Vector3D(50,0,0)); ///< probe origin in pixel space p. (upper-left corner origin)
	input->setSpacing(cx::Vector3D(0.5, 0.6, 1.0));
	input->setSize(QSize(300, 200));
	input->setClipRect_p(cx::DoubleBoundingBox3D(0, input->getSize().width(), 0, input->getSize().height(), 0, 0)); ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
	input->setSector(10, 30, M_PI/2, 2);

	// generate an image based on the probe data. Part of the data is sent over this channel.
	vtkImageDataPtr rawImage = cx::generateVtkImageData(Eigen::Array3i(input->getSize().width(), input->getSize().height(), 1),
																											input->getSpacing(),
																											0);
	cx::ImagePtr imageInput(new cx::Image("my_uid", rawImage));

	// convert the data to igtlink and back
	cx::IGTLinkConversion converter;
	cx::IGTLinkConversionImage imageconverter;
	cx::IGTLinkUSStatusMessage::Pointer msg = converter.encode(input);
	igtl::ImageMessage::Pointer imageMessage = imageconverter.encode(imageInput, pcsLPS);
	cx::ProbeDefinitionPtr output = converter.decode(msg, imageMessage, cx::ProbeDefinitionPtr(new cx::ProbeDefinition()));

	// compare input<->output
	CHECK(input->getType() == output->getType());
	CHECK(input->getDepthStart() == output->getDepthStart());
	CHECK(input->getDepthEnd() == output->getDepthEnd());
	//not supported CHECK(input->getCenterOffset() == output->getCenterOffset());
	CHECK(input->getWidth() == output->getWidth());
	CHECK(cx::similar(input->getClipRect_p(), output->getClipRect_p())); // only supported for cliprect equal to entire image size.
	CHECK(cx::similar(input->getOrigin_p(), output->getOrigin_p()));
	CHECK(cx::similar(input->getSpacing(), output->getSpacing()));
	CHECK(input->getSize().width() == output->getSize().width());
	CHECK(input->getSize().height() == output->getSize().height());
	//not supported CHECK(input->getTemporalCalibration() == output->getTemporalCalibration());
}

