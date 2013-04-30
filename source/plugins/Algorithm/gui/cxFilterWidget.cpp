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
#include "cxFilterPresetWidget.h"
#ifdef CX_USE_TSF
#include "cxTubeSegmentationFilter.h"
#endif //CX_USE_TSF

#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxDataSelectWidget.h"
#include "cxThresholdPreview.h"
#include "cxSelectDataStringDataAdapter.h"

namespace cx
{

OptionsWidget::OptionsWidget(QWidget* parent) :
		mShowAdvanced(false)
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

	this->showAdvanced(showAdvanced);
}

QString OptionsWidget::getCurrentUid()
{
	return mStackedLayout->currentWidget()->objectName();
}

void OptionsWidget::showAdvanced(bool show)
{
	mShowAdvanced = show;
	this->rebuild();
}

void OptionsWidget::rebuild()
{
	this->clear();
	this->populate(mShowAdvanced);
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

	std::map<QString, QWidget*> groupWidgets;
	QWidget* otherWidget = NULL;
	for (unsigned i = 0; i < mOptions.size(); ++i)
	{
		if(showAdvanced || (!showAdvanced && !mOptions[i]->getAdvanced()))
		{
			QWidget* groupWidget = NULL;
			QGridLayout* groupLayout = NULL;

			//make new group if needed
			QString groupName = mOptions[i]->getGroup();
			if(groupName.isEmpty())
				groupName = "other";
			std::map<QString, QWidget*>::iterator it = groupWidgets.find(groupName);
			if(it == groupWidgets.end())
			{
				groupWidget = new QWidget(widget);
				groupWidget->setObjectName(groupName);
				groupLayout = new QGridLayout(groupWidget);
				groupLayout->setMargin(groupLayout->margin()/2);
				QWidget* temp = this->createGroupHeaderWidget(groupName);
				groupLayout->addWidget(temp,0,0,1,2);
				layout->addWidget(groupWidget);
				groupWidgets[groupName] = groupWidget;
				if(groupName == "other")
					otherWidget = temp;
			}
			else
			{
				groupWidget = it->second;
				groupLayout = (QGridLayout*) groupWidget->layout();
			}

			//count groupwidgets items to determine row
			int itemsInGroup = groupLayout->count();

			//make dataadaptewidget and add to existing group
			blockSignals(true);
			createDataWidget(groupWidget, mOptions[i], groupLayout, ++itemsInGroup);
			blockSignals(false);
		}
	}

	//hide group header if only one the "other" group exists
	if((groupWidgets.size() == 1) && (otherWidget != NULL))
		otherWidget->hide();

	mStackedLayout->setCurrentWidget(widget);
}

QWidget* OptionsWidget::createGroupHeaderWidget(QString title)
{
	QWidget* retval = new QWidget(this);
	QVBoxLayout* layout = new QVBoxLayout(retval);
	layout->setMargin(0);
	layout->setSpacing(0);

	QLabel* label = new QLabel(title);
	QFont font = label->font();
	font.setPointSize(8);
	label->setFont(font);
	layout->addWidget(label);
	layout->addWidget(BaseWidget::createHorizontalLine());

	return retval;
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
	mPresetWidget = new FilterPresetWidget(this);
	mAdvancedButton = new QCheckBox("Show &advanced options", this);
	connect(mAdvancedButton, SIGNAL(stateChanged(int)), this, SLOT(showAdvancedOptions(int)));

	topLayout->addWidget(this->wrapInGroupBox(mInputsWidget, "Input"));
	topLayout->addWidget(this->wrapInGroupBox(mOutputsWidget, "Output"));
	topLayout->addWidget(mPresetWidget);
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

void FilterSetupWidget::rebuildOptions()
{
	if(mOptionsWidget)
		mOptionsWidget->rebuild();
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
	connect(mCurrentFilter.get(), SIGNAL(changed()), this, SLOT(rebuildOptions()));

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

		//presets
		if(mCurrentFilter->hasPresets())
		{
			connect(mPresetWidget, SIGNAL(presetSelected(QString)), mCurrentFilter.get(), SLOT(requestSetPresetSlot(QString)));
			//mPresetWidget->setPresets(mCurrentFilter->getPresets());
			mPresetWidget->setFilter(mCurrentFilter);
			mCurrentFilter->requestSetPresetSlot("default");
			mPresetWidget->show();
		} else
			mPresetWidget->hide();
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
	if(mOptionsGroupBox->isHidden())
	{
		mOptionsGroupBox->show();
		mAdvancedButton->show();
		mPresetWidget->showDetailed(true);
	}
	else
	{
		mOptionsGroupBox->hide();
		mAdvancedButton->hide();
		mPresetWidget->showDetailed(false);
	}
}

} // namespace cx
