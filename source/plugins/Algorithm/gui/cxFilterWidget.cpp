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

#include "cxFilterPresetWidget.h"
#include "cxThresholdPreview.h"

namespace cx
{
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
