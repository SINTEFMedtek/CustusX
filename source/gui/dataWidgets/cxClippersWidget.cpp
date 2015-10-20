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

#include "cxClippersWidget.h"
#include <QPushButton>
#include "cxLabeledComboBoxWidget.h"
#include "cxInteractiveClipper.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxClippers.h"

namespace cx
{

ClippersWidget::ClippersWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ClippersWidget", "Clippers"),
	mServices(services),
	mClippers(new Clippers(services))
{
	this->setupUI();
	connect(mClippers.get(), &Clippers::changed, this, &ClippersWidget::clippersChanged);

}

void ClippersWidget::initClipperSelector()
{
	XmlOptionFile mOptions = profile()->getXmlSettings().descend("clippers");

	QStringList range = mClippers->getClipperNames();
	mClipperSelector = StringProperty::initialize("clipperSelector", "Clipper", "Select clipper", "", range, mOptions.getElement());
	connect(mClipperSelector.get(), &Property::changed, this, &ClippersWidget::clipperChanged);
}

void ClippersWidget::setupUI()
{
	initClipperSelector();
	LabeledComboBoxWidget* clipperSelectorBox = new LabeledComboBoxWidget(this, mClipperSelector);

	QPushButton* newClipperButton = new QPushButton("Create clipper", this);
	newClipperButton->setToolTip("Create new clipper based on currently selected");

	mLayout = new QVBoxLayout(this);
	mLayout->addWidget(newClipperButton);
	mLayout->addWidget(clipperSelectorBox);
	mLayout->addStretch();

	connect(newClipperButton, &QPushButton::clicked, this, &ClippersWidget::newClipperButtonClicked);
}

void ClippersWidget::clippersChanged()
{
	CX_LOG_DEBUG() << "ClippersWidget::clippersChanged()";
	mClipperSelector->setValueRange(mClippers->getClipperNames());

	QStringList range = mClipperSelector->getValueRange();
	if(!range.contains(mClipperSelector->getValue()))
	{
		if(range.isEmpty())
			mClipperSelector->setValue("");
		else
			mClipperSelector->setValue(range.first());
	}
}

void ClippersWidget::clipperChanged()
{
	QString clipperName = mClipperSelector->getValue();
	if(clipperName.isEmpty())
		return;

	mCurrentClipper = mClippers->getClipper(clipperName);
}

void ClippersWidget::newClipperButtonClicked()
{
	InteractiveClipperPtr interactiveClipper = InteractiveClipperPtr(new InteractiveClipper(mServices));
	QString nameBase = getNameBaseOfCurrentClipper();
	QString clipperName = nameBase;

	int i = 2;
	while(mClippers->exists(clipperName))
		clipperName = nameBase + QString(" %1").arg(i++);

	mClippers->add(clipperName, interactiveClipper);

	QStringList range = mClipperSelector->getValueRange();
	range << clipperName;
	mClipperSelector->setValueRange(range);
	mClipperSelector->setValue(clipperName);
}

QString ClippersWidget::getNameBaseOfCurrentClipper()
{
	QString nameBase = mClipperSelector->getValue();
	nameBase.remove(QRegExp(" [0-9]{1,2}$"));

	return nameBase;
}

} //cx