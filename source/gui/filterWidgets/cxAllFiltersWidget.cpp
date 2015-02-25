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

#include "cxAllFiltersWidget.h"

#include "cxStringProperty.h"


#include "cxLabeledComboBoxWidget.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxLogger.h"
#include "cxDummyFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxContourFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxResampleImageFilter.h"
#include "cxFilterPresetWidget.h"
#include "cxDilationFilter.h"
#include "cxPluginFramework.h"
#include "cxLogicManager.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "cxNullDeleter.h"
#include "cxProfile.h"

namespace cx {

AllFiltersWidget::AllFiltersWidget(VisServicesPtr services, QWidget* parent) :
    BaseWidget(parent, "FilterWidget", "Configurable Filter")
{
	XmlOptionFile options = profile()->getXmlSettings().descend("filterwidget");
	mFilters.reset(new FilterGroup(options));
	mFilters->append(FilterPtr(new BinaryThresholdImageFilter(services)));
	mFilters->append(FilterPtr(new BinaryThinningImageFilter3DFilter(services)));
	mFilters->append(FilterPtr(new ContourFilter(services)));
	mFilters->append(FilterPtr(new SmoothingImageFilter(services)));
	mFilters->append(FilterPtr(new ResampleImageFilter(services)));
	mFilters->append(FilterPtr(new DilationFilter(services)));

	mServiceListener.reset(
			new ServiceTrackerListener<Filter>(
					LogicManager::getInstance()->getPluginContext(),
					boost::bind(&AllFiltersWidget::onServiceAdded, this, _1),
					boost::function<void(Filter*)>(),
					boost::bind(&AllFiltersWidget::onServiceRemoved, this, _1)));
	mServiceListener->open();

	QStringList availableFilters;
	std::map<QString,QString> names;
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		availableFilters << mFilters->get(i)->getUid();
		names[mFilters->get(i)->getUid()] = mFilters->get(i)->getName();
	}

	mFilterSelector = StringProperty::initialize("filterSelector",
	                                                        "Filter",
	                                                        "Select which filter to use.",
	                                                        availableFilters[0],
	                                                        availableFilters,
	                                                        options.getElement());
	mFilterSelector->setDisplayNames(names);
	connect(mFilterSelector.get(), &StringProperty::valueWasSet,
			this, &AllFiltersWidget::filterChangedSlot);

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	QHBoxLayout* filterLayout = new QHBoxLayout;
	topLayout->addLayout(filterLayout);

	filterLayout->addWidget(new LabeledComboBoxWidget(this, mFilterSelector));

	//Add detailed button
	QAction* detailsAction = this->createAction(this,
		  QIcon(":/icons/open_icon_library/system-run-5.png"),
		  "Details", "Show Details",
		  SLOT(toggleDetailsSlot()),
		  NULL);

	QToolButton* detailsButton = new QToolButton();
	detailsButton->setObjectName("DetailedButton");
	detailsButton->setDefaultAction(detailsAction);
//	editsLayout->addWidget(detailsButton, 0, 2);
	filterLayout->addWidget(detailsButton);

	//Add run button
	QAction* runAction = this->createAction(this,
	                                        QIcon(":/icons/open_icon_library/arrow-right-3.png"),
	                                        "Run Filter", "",
	                                        SLOT(runFilterSlot()),
	                                        NULL);

	CXSmallToolButton* button = new CXSmallToolButton();
	button->setObjectName("RunFilterButton");
	button->setDefaultAction(runAction);
	filterLayout->addWidget(button);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	topLayout->addWidget(mTimedAlgorithmProgressBar);

	mSetupWidget = new FilterSetupWidget(services, this, options, false);
	topLayout->addWidget(mSetupWidget);

	topLayout->addStretch();

	this->filterChangedSlot();
}

QString AllFiltersWidget::defaultWhatsThis() const
{
	return QString("<html>"
	               "<h3>Filter Widget.</h3>"
	               "<p>Select one type of filter.</p>"
	               "<p><i>Currently selected filter:</i></p>"
	               "<p>%1</p>"
	               "</html>").arg(mSetupWidget->defaultWhatsThis());
}

void AllFiltersWidget::filterChangedSlot()
{
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		if (mFilters->get(i)->getUid() == mFilterSelector->getValue())
		{
			mCurrentFilter = mFilters->get(i);
		}
	}

	mSetupWidget->setFilter(mCurrentFilter);
	mFilterSelector->setHelp(this->defaultWhatsThis());
}

void AllFiltersWidget::toggleDetailsSlot()
{
	mSetupWidget->toggleDetailed();

}

void AllFiltersWidget::runFilterSlot()
{
	if (!mCurrentFilter)
		return;
	if (mThread)
	{
		reportWarning(QString("Last operation on %1 is not finished. Could not start filtering")
		                                   .arg(mThread->getFilter()->getName()));
		return;
	}

	mThread.reset(new FilterTimedAlgorithm(mCurrentFilter));
	connect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
	mTimedAlgorithmProgressBar->attach(mThread);

	mThread->execute();
}

void AllFiltersWidget::finishedSlot()
{
	mTimedAlgorithmProgressBar->detach(mThread);
	disconnect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
	mThread.reset();
}

void AllFiltersWidget::onServiceAdded(Filter* service)
{
	mFilters->append(FilterPtr(service, null_deleter()));
}

void AllFiltersWidget::onServiceRemoved(Filter *service)
{
	mFilters->remove(service);
}


} /* namespace cx */
