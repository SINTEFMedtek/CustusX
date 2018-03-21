/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "catch.hpp"
#include "cxMultiViewCache.h"
#include "cxViewWidget.h"
#include "cxViewCollectionWidget.h"
#include <vtkRenderWindow.h>

namespace cxtest {

TEST_CASE("MultiViewCache init", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);
}

TEST_CASE("MultiViewCache retrieveView 3D", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);

	viewCache->clearCache();

	boost::shared_ptr<cx::ViewCollectionWidget> mainWidget;
	mainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout(factory).data());

	cx::ViewWidget* view = viewCache->retrieveView(mainWidget.get(), cx::View::VIEW_3D, false);
	REQUIRE(view);
}

TEST_CASE("MultiViewCache retrieveView 2D", "[opengl][resource][visualization][integration]")
{
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::MultiViewCachePtr viewCache = cx::MultiViewCache::create(factory);
	REQUIRE(viewCache);

	viewCache->clearCache();

	boost::shared_ptr<cx::ViewCollectionWidget> mainWidget;
	mainWidget.reset(cx::ViewCollectionWidget::createViewWidgetLayout(factory).data());

	cx::ViewWidget* view = viewCache->retrieveView(mainWidget.get(), cx::View::VIEW_2D, false);
	REQUIRE(view);
}

} //cxtest
