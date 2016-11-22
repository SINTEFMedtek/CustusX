
#include "cxViewWidget.h"
#include "cxViewCollectionWidgetUsingViewWidgets.h"

namespace cxtest
{

class LayoutWidgetUsingViewWidgetsMock : public cx::LayoutWidgetUsingViewWidgets
{
	Q_OBJECT
public:
	LayoutWidgetUsingViewWidgetsMock(cx::ViewServicePtr viewService, QWidget *parent);
	virtual ~LayoutWidgetUsingViewWidgetsMock();

	std::vector<cx::ViewWidget*> getViewWidgets() const;
};

}
