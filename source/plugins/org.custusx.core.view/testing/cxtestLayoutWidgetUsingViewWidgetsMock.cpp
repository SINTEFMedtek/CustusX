#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"

namespace cxtest {

LayoutWidgetUsingViewWidgetsMock::LayoutWidgetUsingViewWidgetsMock(QWidget *parent) :
	cx::LayoutWidgetUsingViewWidgets(parent)
{}

LayoutWidgetUsingViewWidgetsMock::~LayoutWidgetUsingViewWidgetsMock()
{}

std::vector<cx::ViewWidget*> LayoutWidgetUsingViewWidgetsMock::getViewWidgets() const
{
    return mViews;
}
}
