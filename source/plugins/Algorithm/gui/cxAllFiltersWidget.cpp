#include "cxAllFiltersWidget.h"

#include "cxStringDataAdapterXml.h"

#include "cxMessageManager.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDataLocations.h"
#include "cxTimedAlgorithmProgressBar.h"

#include "cxDummyFilter.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxContourFilter.h"
#include "cxSmoothingImageFilter.h"
#include "cxResampleImageFilter.h"
#include "cxFilterPresetWidget.h"
#include "cxDilationFilter.h"
#ifdef CX_USE_TSF
#include "cxTubeSegmentationFilter.h"
#endif //CX_USE_TSF
#ifdef CX_USE_LEVEL_SET
#include "cxLevelSetFilter.h"
#endif

namespace cx {

AllFiltersWidget::AllFiltersWidget(QWidget* parent) :
    BaseWidget(parent, "FilterWidget", "Configurable Filter")
{
	XmlOptionFile options = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("filterwidget");
	mFilters.reset(new FilterGroup(options));
	mFilters->append(FilterPtr(new BinaryThresholdImageFilter()));
	mFilters->append(FilterPtr(new BinaryThinningImageFilter3DFilter()));
	mFilters->append(FilterPtr(new ContourFilter()));
	mFilters->append(FilterPtr(new SmoothingImageFilter()));
	mFilters->append(FilterPtr(new ResampleImageFilter()));
	mFilters->append(FilterPtr(new DilationFilter()));
#ifdef CX_USE_TSF
	mFilters->append(FilterPtr(new TubeSegmentationFilter()));
#endif //CX_USE_TSF
#ifdef CX_USE_LEVEL_SET
	mFilters->append(FilterPtr(new LevelSetFilter()));
#endif

	QStringList availableFilters;
	std::map<QString,QString> names;
	for (unsigned i=0; i<mFilters->size(); ++i)
	{
		availableFilters << mFilters->get(i)->getUid();
		names[mFilters->get(i)->getUid()] = mFilters->get(i)->getName();
	}

	mFilterSelector = StringDataAdapterXml::initialize("filterSelector",
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

	filterLayout->addWidget(new LabeledComboBoxWidget(this, mFilterSelector));

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
		messageManager()->sendWarning(QString("Last operation on %1 is not finished. Could not start filtering")
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
} /* namespace cx */
