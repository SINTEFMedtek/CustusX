#include "cxtestViewManagerMock.h"

#include <QWidget>

#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"
#include "cxtestViewCollectionWidgetMixedMock.h"
#include "cxSettings.h"
#include "cxLogger.h"
#include "cxRenderLoop.h"

namespace cxtest
{

ViewManagerMock::ViewManagerMock(cx::VisServicesPtr backend) :
	cx::ViewManager(backend)
{
	cx::reportDebug("Constructing ViewManagerMock");
}

ViewManagerMock::~ViewManagerMock()
{
	cx::reportDebug("Destructing ViewManagerMock");
}

ViewManagerMockPtr ViewManagerMock::create(cx::VisServicesPtr backend)
{
	cx::reportDebug("Creating ViewManagerMock");
	ViewManagerMockPtr retval;
	retval.reset(new ViewManagerMock(backend));
	return retval;
}

QWidget *ViewManagerMock::createLayoutWidget(QWidget* parent, int index)
{
	cx::reportDebug("createLayoutWidget - START");
	if (index >= mLayoutWidgets.size())
		return NULL;
	CX_ASSERT(index < mLayoutWidgets.size()); // removed: must be allowed to iterate until NULL
	if (!mLayoutWidgets[index])
	{
		bool optimizedViews = cx::settings()->value("optimizedViews").toBool();

		if (optimizedViews)
		{
			cx::report("creating optimized wiew layout");
			//mLayoutWidgets[index] = ViewCollectionWidget::createOptimizedLayout();
			mLayoutWidgets[index] = new LayoutWidgetUsingViewWidgetsMock(parent);
		}
		else
		{
			cx::report("creating classic wiew layout");
			//mLayoutWidgets[index] = ViewCollectionWidget::createViewWidgetLayout();
			mLayoutWidgets[index] = new ViewCollectionWidgetMixedMock(parent);
		}
		connect(mLayoutWidgets[index].data(), &QObject::destroyed, this, &ViewManagerMock::layoutWidgetDestroyed);
		mRenderLoop->addLayout(mLayoutWidgets[index]);

		this->rebuildLayouts();
	}
	cx::reportDebug("createLayoutWidget - END");
	return mLayoutWidgets[index];
}

std::vector<QPointer<cx::ViewCollectionWidget> > ViewManagerMock::getViewCollectionWidgets() const
{
	return mLayoutWidgets;
}

}
