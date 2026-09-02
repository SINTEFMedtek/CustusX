/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>

#include "cxLiverSegmentationWidget.h"
#include "cxLiverVisibilityWidget.h"

namespace cx
{

LiverWidget::LiverWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, this->getWidgetName(), "Liver")
{
	mTabWidget = new QTabWidget(this);
	mTabWidget->addTab(new LiverSegmentationWidget(services), "Segmentation");
	mTabWidget->addTab(new LiverVisibilityWidget(services), "Visibility");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(mTabWidget);
	this->setLayout(layout);
}

LiverWidget::~LiverWidget()
{
}

QString LiverWidget::getWidgetName()
{
	return "liver_widget";
}

} /* namespace cx */
