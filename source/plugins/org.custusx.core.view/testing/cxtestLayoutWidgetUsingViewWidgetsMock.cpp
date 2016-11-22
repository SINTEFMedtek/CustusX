#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"

namespace cxtest {

LayoutWidgetUsingViewWidgetsMock::LayoutWidgetUsingViewWidgetsMock(cx::ViewServicePtr viewService, QWidget *parent) :
	cx::LayoutWidgetUsingViewWidgets(viewService, parent)
{}

LayoutWidgetUsingViewWidgetsMock::~LayoutWidgetUsingViewWidgetsMock()
{}

std::vector<cx::ViewWidget*> LayoutWidgetUsingViewWidgetsMock::getViewWidgets() const
{
    return mViews;
}
}
