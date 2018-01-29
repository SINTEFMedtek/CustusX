#include "cxViewImplService.h"

#include "boost/shared_ptr.hpp"
#include "cxViewCollectionWidget.h"

namespace cxtest
{
typedef boost::shared_ptr<class ViewServiceMock> ViewServiceMockPtr;

class ViewServiceMock : public cx::ViewImplService
{

public:
	ViewServiceMock(ctkPluginContext* context);
	virtual ~ViewServiceMock();

	virtual QWidget* createLayoutWidget(QWidget* parent, int index);

	std::vector<QPointer<cx::ViewCollectionWidget> > getViewCollectionWidgets() const;

	QList<unsigned> getAutoShowViewGroupNumbers();
};
}
