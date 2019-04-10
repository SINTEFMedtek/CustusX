/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxToolFilterWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "cxStateService.h"
#include "cxTrackingService.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxTrackerConfiguration.h"
#include "cxStringProperty.h"
#include "cxHelperWidgets.h"
#include "cxHelperWidgets.h"
#include "cxLogger.h"

namespace cx
{

ToolFilterGroupBox::ToolFilterGroupBox(TrackingServicePtr trackingService, QWidget* parent) :
	QGroupBox(parent),
	mTrackingService(trackingService)
{
  this->setTitle("Available tools");

	this->createAppSelector();

  mToolListWidget = new FilteringToolListWidget(trackingService, NULL);
  connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

  QVBoxLayout* layout = new QVBoxLayout(this);

  layout->addWidget(sscCreateDataWidget(this, mAppSelector));
  layout->addWidget(mToolListWidget);
}

void ToolFilterGroupBox::createAppSelector()
{
	QString defaultValue = "all";
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QStringList range = config->getAllApplications();
	range.prepend("all");
	mAppSelector = StringProperty::initialize("applications", "Application",
													"Display tools for a given applications",
													defaultValue,
													range,
													QDomNode());
	connect(mAppSelector.get(), SIGNAL(changed()), this, SLOT(filterSlot()));
}

ToolFilterGroupBox::~ToolFilterGroupBox()
{}

void ToolFilterGroupBox::setTrackingSystemSelector(StringPropertyBasePtr selector)
{
	if (mTrackingSystemSelector)
		disconnect(mTrackingSystemSelector.get(), SIGNAL(changed()), this, SLOT(filterSlot()));

	mTrackingSystemSelector = selector;

	if (mTrackingSystemSelector)
		connect(mTrackingSystemSelector.get(), SIGNAL(changed()), this, SLOT(filterSlot()));

	this->filterSlot();
}

void ToolFilterGroupBox::setClinicalApplicationSlot(QString val)
{
	TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	QStringList range = config->getAllApplications();
	for (int i=0; i<range.size(); ++i)
	{
		if (val.contains(range[i], Qt::CaseInsensitive))
			mAppSelector->setValue(range[i]);
	}
}

void ToolFilterGroupBox::filterSlot()
{
  QStringList applicationFilter;
  if (mAppSelector->getValue().contains("all", Qt::CaseInsensitive))
  {
	  TrackerConfigurationPtr config = mTrackingService->getConfiguration();
	  applicationFilter = config->getAllApplications();
	  applicationFilter << "all";
  }
  else
	  applicationFilter = QStringList() << mAppSelector->getValue();
  QStringList trackingSystemFilter = QStringList() << mTrackingSystemSelector->getValue();

  mToolListWidget->filterSlot(applicationFilter, trackingSystemFilter);
}

}//namespace cx
