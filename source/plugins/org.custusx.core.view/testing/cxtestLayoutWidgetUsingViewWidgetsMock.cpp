#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"

namespace cxtest {

LayoutWidgetUsingViewWidgetsMock::LayoutWidgetUsingViewWidgetsMock(QWidget *parent) :
	cx::LayoutWidgetUsingViewWidgets(parent)
{}

LayoutWidgetUsingViewWidgetsMock::~LayoutWidgetUsingViewWidgetsMock()
{}

std::vector<cx::ViewAndSlider> LayoutWidgetUsingViewWidgetsMock::getSliderWidgets() const
{
	return mSliderViews;
}
}
