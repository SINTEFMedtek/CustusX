#include "cxViewCollectionWidgetMixed.h"

namespace cxtest
{

class ViewCollectionWidgetMixedMock : public cx::ViewCollectionWidgetMixed
{
	Q_OBJECT
public:
	ViewCollectionWidgetMixedMock(cx::ViewServicePtr viewService, QWidget *parent);
	virtual ~ViewCollectionWidgetMixedMock();

	cx::ViewCollectionWidget *getViewCollectionWidget();
};

}
