/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFiltersWidget.h"

#include "cxStringProperty.h"


#include "cxLabeledComboBoxWidget.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxLogger.h"
#include "cxDummyFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxContourFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxGenericScriptFilter.h"
#include "cxResampleImageFilter.h"
#include "cxColorVariationFilter.h"
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

FiltersWidget::FiltersWidget(VisServicesPtr services, QWidget* parent, QStringList wantedFilters, QString optionfileTag) :
	BaseWidget(parent, "algorithm_widgets_configurable_filter", "All Filters"),
    mWantedFilters(wantedFilters)
{
    XmlOptionFile options = profile()->getXmlSettings().descend(optionfileTag);
	mFilters.reset(new FilterGroup(options));

    this->appendFilters(services);

    this->setWindowTitleAndObjectNameBasedOnWantedFilters();

    this->configureFilterSelector(options);

    this->setupLayout(services, options);

	this->filterChangedSlot();
}

QString FiltersWidget::generateHelpText() const
{
	return QString("<html>"
	               "<h3>Filter Widget.</h3>"
	               "<p>Select one type of filter.</p>"
	               "<p><i>Currently selected filter:</i></p>"
	               "<p>%1</p>"
				   "</html>").arg(mSetupWidget->generateHelpText());
}

void FiltersWidget::filterChangedSlot()
{
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		if (mFilters->get(i)->getUid() == mFilterSelector->getValue())
		{
			mCurrentFilter = mFilters->get(i);
		}
	}

	mSetupWidget->setFilter(mCurrentFilter);
	mFilterSelector->setHelp(this->generateHelpText());
}

void FiltersWidget::toggleDetailsSlot()
{
	mSetupWidget->toggleDetailed();

}

void FiltersWidget::runFilterSlot()
{
	if (!mCurrentFilter)
		return;
	if (mThread)
	{
		reportWarning(QString("Last operation on %1 is not finished. Could not start filtering").arg(mThread->getFilter()->getName()));
		return;
	}

	mThread.reset(new FilterTimedAlgorithm(mCurrentFilter));
	connect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
	mTimedAlgorithmProgressBar->attach(mThread);

	mThread->execute();
}

void FiltersWidget::finishedSlot()
{
	mTimedAlgorithmProgressBar->detach(mThread);
	disconnect(mThread.get(), SIGNAL(finished()), this, SLOT(finishedSlot()));
	mThread.reset();
}

void FiltersWidget::onServiceAdded(Filter* service)
{
    this->appendFilterIfWanted(FilterPtr(service, null_deleter()));
}

void FiltersWidget::onServiceRemoved(Filter *service)
{
	mFilters->remove(service);
}

void FiltersWidget::appendFiltersThatAreNotServices(VisServicesPtr services)
{
    this->appendFilterIfWanted(FilterPtr(new BinaryThresholdImageFilter(services)));
    this->appendFilterIfWanted(FilterPtr(new BinaryThinningImageFilter3DFilter(services)));
    this->appendFilterIfWanted(FilterPtr(new ContourFilter(services)));
    this->appendFilterIfWanted(FilterPtr(new ColorVariationFilter(services)));
	this->appendFilterIfWanted(FilterPtr(new SmoothingImageFilter(services)));
	this->appendFilterIfWanted(FilterPtr(new GenericScriptFilter(services)));
    this->appendFilterIfWanted(FilterPtr(new ResampleImageFilter(services)));
    this->appendFilterIfWanted(FilterPtr(new DilationFilter(services)));
}

void FiltersWidget::appendFilterServices()
{
    mServiceListener.reset(
            new ServiceTrackerListener<Filter>(
                    LogicManager::getInstance()->getPluginContext(),
                    boost::bind(&FiltersWidget::onServiceAdded, this, _1),
                    boost::function<void(Filter*)>(),
                    boost::bind(&FiltersWidget::onServiceRemoved, this, _1)));
    mServiceListener->open();
}

void FiltersWidget::appendFilters(VisServicesPtr services)
{
    this->appendFiltersThatAreNotServices(services);
    this->appendFilterServices();
}

void FiltersWidget::appendFilterIfWanted(FilterPtr filter)
{
    if(mWantedFilters.empty() || mWantedFilters.contains(filter->getName()))
        mFilters->append(filter);
}

void FiltersWidget::configureFilterSelector(XmlOptionFile options)
{
    QStringList availableFilters;
    std::map<QString,QString> names;
    for (unsigned i=0; i<mFilters->size(); ++i)
    {
        availableFilters << mFilters->get(i)->getUid();
        names[mFilters->get(i)->getUid()] = mFilters->get(i)->getName();
    }
    if(availableFilters.isEmpty())
    {
        availableFilters << "FILTER NOT FOUND";
    }
    mFilterSelector = StringProperty::initialize("filterSelector",
                                                "Filter",
                                                "Select which filter to use.",
                                                availableFilters[0],
                                                availableFilters,
                                                options.getElement());
    mFilterSelector->setDisplayNames(names);
    connect(mFilterSelector.get(), &StringProperty::valueWasSet, this, &FiltersWidget::filterChangedSlot);
}

void FiltersWidget::addDetailedButton(QHBoxLayout* filterLayout)
{
    QAction* detailsAction = this->createAction(this,
          QIcon(":/icons/open_icon_library/system-run-5.png"),
          "Details", "Toggle Details",
          SLOT(toggleDetailsSlot()),
          NULL);

    QToolButton* detailsButton = new QToolButton();
    detailsButton->setObjectName("DetailedButton");
    detailsButton->setDefaultAction(detailsAction);
    filterLayout->addWidget(detailsButton);
}

void FiltersWidget::addRunButton(QHBoxLayout* filterLayout)
{
    QAction* runAction = this->createAction(this,
                                            QIcon(":/icons/open_icon_library/arrow-right-3.png"),
                                            "Run Filter", "",
                                            SLOT(runFilterSlot()),
                                            NULL);

    CXSmallToolButton* button = new CXSmallToolButton();
    button->setObjectName("RunFilterButton");
    button->setDefaultAction(runAction);
    filterLayout->addWidget(button);
}

QHBoxLayout * FiltersWidget::addFilterSelector(QVBoxLayout* topLayout)
{
    QHBoxLayout* filterLayout = new QHBoxLayout;
    topLayout->addLayout(filterLayout);
	LabeledComboBoxWidget* filterSelectorWidget = new LabeledComboBoxWidget(this, mFilterSelector);
    filterSelectorWidget->showLabel(false);
    filterLayout->addWidget(filterSelectorWidget);

    return filterLayout;
}

void FiltersWidget::addProgressBar(QVBoxLayout* topLayout)
{
    mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
    topLayout->addWidget(mTimedAlgorithmProgressBar);
}

void FiltersWidget::addFilterWidget(XmlOptionFile options, VisServicesPtr services, QVBoxLayout* topLayout)
{
    mSetupWidget = new FilterSetupWidget(services, this, options, false);
    topLayout->addWidget(mSetupWidget);
}

void FiltersWidget::setWindowTitleAndObjectNameBasedOnWantedFilters()
{
    if(mWantedFilters.size() == 1)
    {
        this->setWindowTitle(mWantedFilters.first());
        this->setObjectName(mWantedFilters.first()+" Widget");
    }
    else if(mWantedFilters.empty())
    {
        this->setWindowTitle("All Filters");
        this->setObjectName("algorithm_widgets_configurable_filter");
    }
}

void FiltersWidget::setupLayout(VisServicesPtr services, XmlOptionFile options)
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    QHBoxLayout* filterLayout = addFilterSelector(topLayout);
    this->addDetailedButton(filterLayout);
    this->addRunButton(filterLayout);
    this->addProgressBar(topLayout);
    this->addFilterWidget(options, services, topLayout);
    topLayout->addStretch();
}

} /* namespace cx */
