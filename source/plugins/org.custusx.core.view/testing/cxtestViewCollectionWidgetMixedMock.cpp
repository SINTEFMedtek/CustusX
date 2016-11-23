#include "cxtestViewCollectionWidgetMixedMock.h"

namespace cxtest
{

ViewCollectionWidgetMixedMock::ViewCollectionWidgetMixedMock(cx::RenderWindowFactoryPtr factory, QWidget *parent) :
	cx::ViewCollectionWidgetMixed(factory, parent)
{

}

ViewCollectionWidgetMixedMock::~ViewCollectionWidgetMixedMock()
{

}

cx::ViewCollectionWidget *ViewCollectionWidgetMixedMock::getViewCollectionWidget()
{
	return mBaseLayout;
}

}
