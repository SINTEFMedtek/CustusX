/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestVisualizationHelper.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"


namespace cxtest{

VisualizationHelper::VisualizationHelper()
{
	services = cxtest::TestVisServices::create();

	cx::ViewPtr view = viewsFixture.addView(0, 0);

	viewWrapper.reset(new ViewWrapper2DFixture(view, services));
}

VisualizationHelper::~VisualizationHelper()
{
	services.reset();
}

ViewWrapper2DFixture::ViewWrapper2DFixture(cx::ViewPtr view, cx::VisServicesPtr services) :
	cx::ViewWrapper2D(view, services)
{}

void ViewWrapper2DFixture::emitPointSampled()
{
	this->samplePoint(cx::Vector3D(1, 1, 1));
}

}//namespace cxtest
