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

#include "cxFilterWidget.h"

#include <QGroupBox>
#include <QCheckBox>
#include "cxFilterPresetWidget.h"
#include "cxWidgetObscuredListener.h"
#include "cxVisServices.h"

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

	mInputsWidget = new OptionsWidget(mServices->visualizationService, mServices->getPatientService(), this);
	mOutputsWidget = new OptionsWidget(mServices->visualizationService, mServices->getPatientService(), this);
	mOptionsWidget = new OptionsWidget(mServices->visualizationService, mServices->getPatientService(), this);
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

//		std::vector<SelectDataStringPropertyBasePtr> inputTypes = mCurrentFilter->getInputTypes();
//		std::vector<SelectDataStringPropertyBasePtr> outputTypes = mCurrentFilter->getOutputTypes();
		std::vector<PropertyPtr> options = mCurrentFilter->getOptions();

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

		this->setObjectName(mCurrentFilter->getType());
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
