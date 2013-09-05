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
#include "cxtestMultiVolume3DRepProducerFixture.h"



namespace cxtest
{

MultiVolume3DRepProducerFixture::MultiVolume3DRepProducerFixture()
{
	ssc::MessageManager::initialize();
}

MultiVolume3DRepProducerFixture::~MultiVolume3DRepProducerFixture()
{
	ssc::MessageManager::shutdown();
}

void MultiVolume3DRepProducerFixture::initializeVisualizerAndImages(QString type, int imageCount)
{
	mBase.setVisualizerType(type);
	for (unsigned i=0; i<imageCount; ++i)
	{
		ssc::ImagePtr image = cxtest::Utilities::create3DImage();
		mBase.addImage(image);
		mImages.push_back(image);
	}
}



} // namespace cxtest


