#include "cxtestViewCollectionWidgetMixedMock.h"

namespace cxtest
{

ViewCollectionWidgetMixedMock::ViewCollectionWidgetMixedMock(QWidget *parent) :
	cx::ViewCollectionWidgetMixed(parent)
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
