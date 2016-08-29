#include "cxViewManager.h"

#include "cxVisServices.h"

#include "boost/shared_ptr.hpp"

namespace cxtest
{

typedef boost::shared_ptr<class ViewManagerMock> ViewManagerMockPtr;

class ViewManagerMock : public cx::ViewManager
{

public:
	static ViewManagerMockPtr create(cx::VisServicesPtr backend);

	ViewManagerMock(cx::VisServicesPtr backend);
	virtual ~ViewManagerMock();

	QWidget *createLayoutWidget(QWidget* parent, int index);
	std::vector<QPointer<cx::ViewCollectionWidget> > getViewCollectionWidgets() const;

};
}
