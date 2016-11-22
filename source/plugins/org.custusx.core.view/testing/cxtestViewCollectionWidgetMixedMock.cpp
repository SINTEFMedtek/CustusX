#include "cxtestViewCollectionWidgetMixedMock.h"

namespace cxtest
{

ViewCollectionWidgetMixedMock::ViewCollectionWidgetMixedMock(cx::ViewServicePtr viewService, QWidget *parent) :
	cx::ViewCollectionWidgetMixed(viewService, parent)
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
