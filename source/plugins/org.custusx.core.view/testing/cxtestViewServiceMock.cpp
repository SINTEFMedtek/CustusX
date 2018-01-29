#include "cxtestViewServiceMock.h"

#include "cxLogger.h"
#include "cxSettings.h"
#include "cxtestLayoutWidgetUsingViewWidgetsMock.h"
#include "cxtestViewCollectionWidgetMixedMock.h"
#include "cxRenderLoop.h"

namespace cxtest
{

ViewServiceMock::ViewServiceMock(ctkPluginContext *context) :
	ViewImplService(context)
{
}

ViewServiceMock::~ViewServiceMock()
{
}

QWidget *ViewServiceMock::createLayoutWidget(QWidget* parent, int index)
{
	if (index >= mLayoutWidgets.size())
		return NULL;

	if (!mLayoutWidgets[index])
	{
		bool optimizedViews = cx::settings()->value("optimizedViews").toBool();

		if (optimizedViews)
		{
			mLayoutWidgets[index] = new LayoutWidgetUsingViewWidgetsMock(mRenderWindowFactory, parent);
		}
		else
		{
			mLayoutWidgets[index] = new ViewCollectionWidgetMixedMock(mRenderWindowFactory, parent);
		}
		connect(mLayoutWidgets[index].data(), &QObject::destroyed, this, &ViewServiceMock::layoutWidgetDestroyed);
		mRenderLoop->addLayout(mLayoutWidgets[index]);

		this->rebuildLayouts();
	}
	return mLayoutWidgets[index];
}

std::vector<QPointer<cx::ViewCollectionWidget> > ViewServiceMock::getViewCollectionWidgets() const
{
	return mLayoutWidgets;
}

QList<unsigned> ViewServiceMock::getAutoShowViewGroupNumbers()
{
	return this->getViewGroupsToAutoShowIn();
}

}//cxtest
