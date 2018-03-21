/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
