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

#include "cxCompactFilterSetupWidget.h"

#include <QGroupBox>
#include "cxSelectDataStringProperty.h"
#include "cxWidgetObscuredListener.h"
#include "cxOptionsWidget.h"

namespace cx {

CompactFilterSetupWidget::CompactFilterSetupWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, XmlOptionFile options, bool addFrame) :
    BaseWidget(parent, "FilterSetupWidget", "FilterSetup")
{
	mFrame = NULL;

	QVBoxLayout* toptopLayout = new QVBoxLayout(this);
	toptopLayout->setMargin(0);

	mOptionsWidget = new OptionsWidget(viewService, patientModelService, this);

	if (addFrame)
	{
		mFrame = this->wrapInGroupBox(mOptionsWidget, "Algorithm");
		toptopLayout->addWidget(mFrame);
	}
	else
		toptopLayout->addWidget(mOptionsWidget);

	mObscuredListener.reset(new WidgetObscuredListener(this));
	connect(mObscuredListener.get(), SIGNAL(obscured(bool)), this, SLOT(obscuredSlot(bool)));

	mOptions = options;
}

void CompactFilterSetupWidget::obscuredSlot(bool obscured)
{
	if (mCurrentFilter)
		mCurrentFilter->setActive(!obscured);
}

QString CompactFilterSetupWidget::getHelpText() const
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

		std::vector<SelectDataStringPropertyBasePtr> inputTypes = mCurrentFilter->getInputTypes();
		std::vector<SelectDataStringPropertyBasePtr> outputTypes = mCurrentFilter->getOutputTypes();
		std::vector<PropertyPtr> options = mCurrentFilter->getOptions();

		std::vector<PropertyPtr> all;
		std::remove_copy(inputTypes.begin(), inputTypes.end(), std::back_inserter(all), inputTypes[0]);
		std::remove_copy(outputTypes.begin(), outputTypes.end(), std::back_inserter(all), outputTypes[0]);
		std::copy(options.begin(), options.end(), std::back_inserter(all));

		mOptionsWidget->setOptions(mCurrentFilter->getUid(), all, false);
		this->setToolTip(this->getHelpText());
	}
	else
		mOptionsWidget->setOptions("", std::vector<PropertyPtr>(), false);
}
} /* namespace cx */
