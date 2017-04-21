#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"

namespace cxtest {

LayoutWidgetUsingViewWidgetsMock::LayoutWidgetUsingViewWidgetsMock(cx::RenderWindowFactoryPtr factory, QWidget *parent) :
	cx::LayoutWidgetUsingViewWidgets(factory, parent)
{}

LayoutWidgetUsingViewWidgetsMock::~LayoutWidgetUsingViewWidgetsMock()
{}

std::vector<cx::ViewWidget*> LayoutWidgetUsingViewWidgetsMock::getViewWidgets() const
{
    return mViews;
}
}
