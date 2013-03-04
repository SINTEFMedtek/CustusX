// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxFilterWidget.h"

#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscHelperWidgets.h"
#include "cxDataAdapterHelper.h"
#include "cxDataLocations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxDataInterface.h"

#include "cxDummyFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxContourFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxResampleImageFilter.h"
#ifdef CX_USE_TSF
#include "cxTubeSegmentationFilter.h"
#endif //CX_USE_TSF

#include "sscTypeConversions.h"
#include "cxDataSelectWidget.h"
#include "sscMessageManager.h"
#include "cxThresholdPreview.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{

OptionsWidget::OptionsWidget(QWidget* parent)
{
	this->setSizePolicy(this->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	mStackedLayout = new QStackedLayout(this);
	mStackedLayout->setMargin(0);
}

void OptionsWidget::setOptions(QString uid, std::vector<SelectDataStringDataAdapterBasePtr> options, bool showAdvanced)
{
	std::vector<DataAdapterPtr> converted;
	std::copy(options.begin(), options.end(), std::back_inserter(converted));
	this->setOptions(uid, converted, showAdvanced);
}

void OptionsWidget::setOptions(QString uid, std::vector<DataAdapterPtr> options, bool showAdvanced)
{
	// return if already on uid
	if (mStackedLayout->currentWidget() && (uid == mStackedLayout->currentWidget()->objectName()))
		return;

	mOptions = options;
	mUid = uid;

	this->clear();
	this->populate(showAdvanced);
}

QString OptionsWidget::getCurrentUid()
{
	return mStackedLayout->currentWidget()->objectName();
}

void OptionsWidget::showAdvanced(bool show)
{
	this->clear();
	this->populate(show);
}

void OptionsWidget::clear()
{
	QLayoutItem *child;
	while ((child = mStackedLayout->takeAt(0)) != 0)
	{
		// delete both the layoutitem AND the widget. Not auto done because layoutitem is no QObject.
		QWidget* widget = child->widget();
		delete child;
		delete widget;
	}
}

void OptionsWidget::populate(bool showAdvanced)
{
	// No existing found,
	// create a new stack element for this uid:
	QWidget* widget = new QWidget(this);
	widget->setObjectName(mUid);
	mStackedLayout->addWidget(widget);
	QGridLayout* layout = new QGridLayout(widget);
	layout->setMargin(layout->margin()/2);

	for (unsigned i = 0; i < mOptions.size(); ++i)
	{
		if(showAdvanced || (!showAdvanced && !mOptions[i]->getAdvanced()))
			createDataWidget(widget, mOptions[i], layout, i);
	}

	mStackedLayout->setCurrentWidget(widget);
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


FilterSetupWidget::FilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame) :
    BaseWidget(parent, "FilterSetupWidget", "FilterSetup")
{
	mFrame = NULL;

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->setMargin(0);

	QWidget* topWidget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
	topLayout->setMargin(0);

	if (addFrame)
	{
		mFrame = this->wrapInGroupBox(topWidget, "Algorithm");
		toptopLayout->addWidget(mFrame);
	}
	else
	{
		toptopLayout->addWidget(topWidget);
	}

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

	mOptions = options;

	mInputsWidget = new OptionsWidget(this);
	mOutputsWidget = new OptionsWidget(this);
	mOptionsWidget = new OptionsWidget(this);
	mAdvancedButton = new QCheckBox("Show &advanced options", this);
	connect(mAdvancedButton, SIGNAL(stateChanged(int)), this, SLOT(showAdvancedOptions(int)));

	topLayout->addWidget(this->wrapInGroupBox(mInputsWidget, "Input"));
	topLayout->addWidget(this->wrapInGroupBox(mOutputsWidget, "Output"));
	mOptionsGroupBox = this->wrapInGroupBox(mOptionsWidget, "Options");
	topLayout->addWidget(mOptionsGroupBox);
	topLayout->addWidget(mAdvancedButton);
}

void FilterSetupWidget::obscuredSlot(bool obscured)
{
	if (mCurrentFilter)
		mCurrentFilter->setActive(!obscured);
}

void FilterSetupWidget::showAdvancedOptions(int state)
{
	if(state > 0)
	{
		mInputsWidget->showAdvanced(true);
		mOutputsWidget->showAdvanced(true);
		mOptionsWidget->showAdvanced(true);
	}else{
		mInputsWidget->showAdvanced(false);
		mOutputsWidget->showAdvanced(false);
		mOptionsWidget->showAdvanced(false);
	}
}

QString FilterSetupWidget::defaultWhatsThis() const
{
	QString name("None");
	QString help("");
	if (mCurrentFilter)
	{
		name = mCurrentFilter->getName();
		help = mCurrentFilter->getHelp();
	}
	return QString("<html>"
	               "<h4>%1</h4>"
	               "<p>%2</p>"
	               "</html>").arg(name).arg(help);
}

void FilterSetupWidget::setFilter(FilterPtr filter)
{
	if (filter==mCurrentFilter)
		return;

	if (mCurrentFilter)
		mCurrentFilter->setActive(false);

	mCurrentFilter = filter;

	if (mFrame)
		mFrame->setTitle(mCurrentFilter->getName());

	if (mCurrentFilter)
	{
		mCurrentFilter->setActive(!mObscuredListener->isObscured());

		std::vector<SelectDataStringDataAdapterBasePtr> inputTypes = mCurrentFilter->getInputTypes();
		std::vector<SelectDataStringDataAdapterBasePtr> outputTypes = mCurrentFilter->getOutputTypes();
		std::vector<DataAdapterPtr> options = mCurrentFilter->getOptions();

		mInputsWidget->setOptions(mCurrentFilter->getUid(), mCurrentFilter->getInputTypes(), false);
		mOutputsWidget->setOptions(mCurrentFilter->getUid(), mCurrentFilter->getOutputTypes(), false);
		mOptionsWidget->setOptions(mCurrentFilter->getUid(), options, false);
	}
	else
	{
		mInputsWidget->setOptions("", std::vector<DataAdapterPtr>(), false);
		mOutputsWidget->setOptions("", std::vector<DataAdapterPtr>(), false);
		mOptionsWidget->setOptions("", std::vector<DataAdapterPtr>(), false);
	}
}

void FilterSetupWidget::toggleDetailed()
{
	//todo presetsWidget->toggleDetailed();
	if(mOptionsGroupBox->isHidden())
	{
		mOptionsGroupBox->show();
		mAdvancedButton->show();
	}
	else
	{
		mOptionsGroupBox->hide();
		mAdvancedButton->hide();
	}
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CompactFilterSetupWidget::CompactFilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame) :
    BaseWidget(parent, "FilterSetupWidget", "FilterSetup")
{
	mFrame = NULL;

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->setMargin(0);

	mOptionsWidget = new OptionsWidget(this);

	if (addFrame)
	{
		mFrame = this->wrapInGroupBox(mOptionsWidget, "Algorithm");
		toptopLayout->addWidget(mFrame);
	}
	else
	{
		toptopLayout->addWidget(mOptionsWidget);
	}

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

	mOptions = options;
}

void CompactFilterSetupWidget::obscuredSlot(bool obscured)
{
	if (mCurrentFilter)
		mCurrentFilter->setActive(!obscured);
}

QString CompactFilterSetupWidget::defaultWhatsThis() const
{
	QString name("None");
	QString help("");
	if (mCurrentFilter)
	{
		name = mCurrentFilter->getName();
		help = mCurrentFilter->getHelp();
	}
	return QString("<html>"
	               "<h4>%1</h4>"
	               "<p>%2</p>"
	               "</html>").arg(name).arg(help);
}

void CompactFilterSetupWidget::setFilter(FilterPtr filter)
{
	if (filter==mCurrentFilter)
		return;

	if (mCurrentFilter)
		mCurrentFilter->setActive(false);

	mCurrentFilter = filter;

	if (mFrame)
		mFrame->setTitle(mCurrentFilter->getName());

	if (mCurrentFilter)
	{
		mCurrentFilter->setActive(!mObscuredListener->isObscured());

		std::vector<SelectDataStringDataAdapterBasePtr> inputTypes = mCurrentFilter->getInputTypes();
		std::vector<SelectDataStringDataAdapterBasePtr> outputTypes = mCurrentFilter->getOutputTypes();
		std::vector<DataAdapterPtr> options = mCurrentFilter->getOptions();

		std::vector<DataAdapterPtr> all;
		std::remove_copy(inputTypes.begin(), inputTypes.end(), std::back_inserter(all), inputTypes[0]);
		std::remove_copy(outputTypes.begin(), outputTypes.end(), std::back_inserter(all), outputTypes[0]);
		std::copy(options.begin(), options.end(), std::back_inserter(all));

		mOptionsWidget->setOptions(mCurrentFilter->getUid(), all, false);
	}
	else
	{
		mOptionsWidget->setOptions("", std::vector<DataAdapterPtr>(), false);
	}
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

AllFiltersWidget::AllFiltersWidget(QWidget* parent) :
    BaseWidget(parent, "FilterWidget", "Configurable Filter")
{
	ssc::XmlOptionFile options = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("filterwidget");
	mFilters.reset(new FilterGroup(options));
//	mFilters->append(FilterPtr(new DummyFilter()));
	mFilters->append(FilterPtr(new BinaryThresholdImageFilter()));
	mFilters->append(FilterPtr(new BinaryThinningImageFilter3DFilter()));
	mFilters->append(FilterPtr(new ContourFilter()));
	mFilters->append(FilterPtr(new SmoothingImageFilter()));
	mFilters->append(FilterPtr(new ResampleImageFilter()));
#ifdef CX_USE_TSF
	mFilters->append(FilterPtr(new TubeSegmentationFilter()));
#endif //CX_USE_TSF

	QStringList availableFilters;
	std::map<QString,QString> names;
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		availableFilters << mFilters->get(i)->getUid();
		names[mFilters->get(i)->getUid()] = mFilters->get(i)->getName();
	}

	mFilterSelector = ssc::StringDataAdapterXml::initialize("filterSelector",
	                                                        "Filter",
	                                                        "Select which filter to use.",
	                                                        availableFilters[0],
	                                                        availableFilters,
	                                                        options.getElement());
	mFilterSelector->setDisplayNames(names);
	connect(mFilterSelector.get(), SIGNAL(valueWasSet()), this, SLOT(filterChangedSlot()));

	QVBoxLayout* topLayout = new QVBoxLayout(this);

	QHBoxLayout* filterLayout = new QHBoxLayout;
	topLayout->addLayout(filterLayout);

	filterLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFilterSelector));

	//Add detailed button
	QAction* detailsAction = this->createAction(this,
		  QIcon(":/icons/open_icon_library/png/64x64/actions/system-run-5.png"),
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
	                                        QIcon(":/icons/open_icon_library/png/64x64/actions/arrow-right-3.png"),
	                                        "Run Filter", "",
	                                        SLOT(runFilterSlot()),
	                                        NULL);

	CXSmallToolButton* button = new CXSmallToolButton();
	button->setObjectName("RunFilterButton");
	button->setDefaultAction(runAction);
	filterLayout->addWidget(button);

	mTimedAlgorithmProgressBar = new cx::TimedAlgorithmProgressBar;
	topLayout->addWidget(mTimedAlgorithmProgressBar);

	mSetupWidget = new FilterSetupWidget(this, options, false);
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
		ssc::messageManager()->sendWarning(QString("Last operation on %1 is not finished. Could not start filtering")
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

} // namespace cx
