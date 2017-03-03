/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include <cxToolFilterWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "cxEnumConverter.h"
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
