/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTVISIALIZATIONHELPER_H
#define CXTESTVISIALIZATIONHELPER_H

#include "cxViewWrapper2D.h"
#include "cxViewsFixture.h"

namespace cxtest
{
typedef boost::shared_ptr<class ViewWrapper2DFixture> ViewWrapper2DFixturePtr;
struct ViewWrapper2DFixture : public cx::ViewWrapper2D
{
	ViewWrapper2DFixture(cx::ViewPtr view, cx::VisServicesPtr services);
	void emitPointSampled();
};

/** Visualization test class.
 * Uses ViewsFixture, so it can only be used by integration tests.
 */
class VisualizationHelper
{
public:
	VisualizationHelper();
	~VisualizationHelper();

	cx::VisServicesPtr services;
	ViewWrapper2DFixturePtr viewWrapper;
private:
	ViewsFixture viewsFixture;
};

}//namespace cxtest


#endif // CXTESTVISIALIZATIONHELPER_H
