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

}

ViewManagerMock::~ViewManagerMock()
{
}

ViewManagerMockPtr ViewManagerMock::create(cx::VisServicesPtr backend)
{
	ViewManagerMockPtr retval;
	retval.reset(new ViewManagerMock(backend));
	return retval;
}

QWidget *ViewManagerMock::createLayoutWidget(QWidget* parent, int index)
{
	if (index >= mLayoutWidgets.size())
		return NULL;

	if (!mLayoutWidgets[index])
	{
		bool optimizedViews = cx::settings()->value("optimizedViews").toBool();

		if (optimizedViews)
		{
			mLayoutWidgets[index] = new LayoutWidgetUsingViewWidgetsMock(mBackend->view(), parent);
		}
		else
		{
			mLayoutWidgets[index] = new ViewCollectionWidgetMixedMock(mBackend->view(), parent);
		}
		connect(mLayoutWidgets[index].data(), &QObject::destroyed, this, &ViewManagerMock::layoutWidgetDestroyed);
		mRenderLoop->addLayout(mLayoutWidgets[index]);

		this->rebuildLayouts();
	}
	return mLayoutWidgets[index];
}

std::vector<QPointer<cx::ViewCollectionWidget> > ViewManagerMock::getViewCollectionWidgets() const
{
	return mLayoutWidgets;
}

}
