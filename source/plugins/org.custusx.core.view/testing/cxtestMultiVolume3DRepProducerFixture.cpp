/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxtestMultiVolume3DRepProducerFixture.h"
#include "cxReporter.h"
#include "cxView.h"

namespace cxtest
{

MultiVolume3DRepProducerFixture::MultiVolume3DRepProducerFixture()
{
	cx::Reporter::initialize();
}

MultiVolume3DRepProducerFixture::~MultiVolume3DRepProducerFixture()
{
	cx::Reporter::shutdown();
}

void MultiVolume3DRepProducerFixture::initializeVisualizerAndImages(QString type, int imageCount)
{
	mBase.setVisualizerType(type);
	for (unsigned i=0; i<imageCount; ++i)
	{
		cx::ImagePtr image = cxtest::Utilities::create3DImage();
		mBase.addImage(image);
		mImages.push_back(image);
	}
}



} // namespace cxtest


