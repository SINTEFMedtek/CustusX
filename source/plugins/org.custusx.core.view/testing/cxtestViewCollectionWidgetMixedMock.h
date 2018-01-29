#include "cxViewCollectionWidgetMixed.h"

namespace cxtest
{

class ViewCollectionWidgetMixedMock : public cx::ViewCollectionWidgetMixed
{
	Q_OBJECT
public:
	ViewCollectionWidgetMixedMock(cx::RenderWindowFactoryPtr factory, QWidget *parent);
	virtual ~ViewCollectionWidgetMixedMock();

	cx::ViewCollectionWidget *getViewCollectionWidget();
};

}
