/*
 * cxViewCollectionLayout.cpp
 *
 *  Created on: Sep 16, 2014
 *      Author: christiana
 */

#include "cxViewCollectionLayout.h"

#include <QGridLayout>
#include "cxLogger.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "cxReporter.h"

#include "cxViewContainer.h"

namespace cx
{

LayoutWidgetUsingViewCollection::LayoutWidgetUsingViewCollection()
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);
	mViewContainer = new ViewContainer;
	layout->addWidget(mViewContainer);
}

LayoutWidgetUsingViewCollection::~LayoutWidgetUsingViewCollection()
{
}

ViewPtr LayoutWidgetUsingViewCollection::addView(View::Type type, LayoutRegion region)
{
	static int nameGenerator = 0;
	QString uid = QString("view-%1-%2")
			.arg(nameGenerator++)
			.arg(reinterpret_cast<long>(this));

	ViewItem* viewItem = mViewContainer->addView(uid,
											 region.pos.row, region.pos.col,
											 region.span.row, region.span.col,
											 uid);
	ViewPtr view = viewItem->getView();

	viewItem->getView()->setType(type);
	mViews.push_back(view);
	return view;
}

void LayoutWidgetUsingViewCollection::showViews()
{
}

void LayoutWidgetUsingViewCollection::clearViews()
{
	mViews.clear();
	mViewContainer->clear();
}


} /* namespace cx */
