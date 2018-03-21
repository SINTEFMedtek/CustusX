/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterSetupWidget.h"

#include <QGroupBox>
#include <QCheckBox>
#include "cxFilterPresetWidget.h"
#include "cxWidgetObscuredListener.h"
#include "cxVisServices.h"
#include "cxLogger.h"

namespace cx
{
FilterSetupWidget::FilterSetupWidget(VisServicesPtr services, QWidget* parent, XmlOptionFile options, bool addFrame) :
	BaseWidget(parent, "FilterSetupWidget", "FilterSetup"),
	mServices(services)
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

	mInputsWidget = new OptionsWidget(mServices->view(), mServices->patient(), this);
	mOutputsWidget = new OptionsWidget(mServices->view(), mServices->patient(), this);
	mOptionsWidget = new OptionsWidget(mServices->view(), mServices->patient(), this);
	mPresetWidget = new FilterPresetWidget(this);
    mAdvancedButton = new QCheckBox("Show advanced options", this);
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

QString FilterSetupWidget::generateHelpText() const
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

void FilterSetupWidget::setVisibilityOfOptionsAndAdvancedOptions()
{
    mAdvancedButton->setVisible(mOptionsWidget->hasAdvancedOptions());
    this->showAdvancedOptions(mAdvancedButton->isChecked());
    mOptionsGroupBox->setVisible(mOptionsWidget->hasOptions());
}

void FilterSetupWidget::setFilter(FilterPtr filter)
{
	if (filter==mCurrentFilter)
		return;

	if (mCurrentFilter)
		mCurrentFilter->setActive(false);

	mCurrentFilter = filter;
	connect(mCurrentFilter.get(), &Filter::changed, this, &FilterSetupWidget::rebuildOptions);

	if (mFrame)
		mFrame->setTitle(mCurrentFilter->getName());

	if (mCurrentFilter)
	{
		mCurrentFilter->setActive(!mObscuredListener->isObscured());

		mInputsWidget->setOptions(mCurrentFilter->getUid(), mCurrentFilter->getInputTypes(), false);
		mOutputsWidget->setOptions(mCurrentFilter->getUid(), mCurrentFilter->getOutputTypes(), false);
        mOptionsWidget->setOptions(mCurrentFilter->getUid(), mCurrentFilter->getOptions(), false);

		//presets
		if(mCurrentFilter->hasPresets())
		{
			connect(mPresetWidget, &PresetWidget::presetSelected, mCurrentFilter.get(), &Filter::requestSetPresetSlot);
			//mPresetWidget->setPresets(mCurrentFilter->getPresets());
			mPresetWidget->setFilter(mCurrentFilter);
			mCurrentFilter->requestSetPresetSlot("default");
			mPresetWidget->show();
		} else
			mPresetWidget->hide();

		this->setObjectName(mCurrentFilter->getType());

        this->setVisibilityOfOptionsAndAdvancedOptions();
	}
	else
	{
		mInputsWidget->setOptions("", std::vector<PropertyPtr>(), false);
		mOutputsWidget->setOptions("", std::vector<PropertyPtr>(), false);
		mOptionsWidget->setOptions("", std::vector<PropertyPtr>(), false);
	}

	this->setToolTip(this->generateHelpText());
}

void FilterSetupWidget::toggleDetailed()
{
	if(mOptionsGroupBox->isHidden())
	{
        if(mOptionsWidget->hasOptions())
        {
            mOptionsGroupBox->show();
            if(mOptionsWidget->hasAdvancedOptions())
                mAdvancedButton->show();
        }
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
