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
#include "cxClippingWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "cxStringPropertyBase.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxDefinitionStrings.h"
#include "cxInteractiveClipper.h"
#include "cxSelectDataStringProperty.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVisServices.h"
#include "cxStringProperty.h"
#include "cxLogger.h"
#include "cxHelperWidgets.h"

#include <boost/foreach.hpp>

namespace cx
{

StringPropertyClipPlane::StringPropertyClipPlane(InteractiveClipperPtr clipper) :
	mInteractiveClipper(clipper)
{
	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QString StringPropertyClipPlane::getDisplayName() const
{
	return "Slice Plane";
}
bool StringPropertyClipPlane::setValue(const QString& value)
{
	PLANE_TYPE plane = string2enum<PLANE_TYPE> (value);
	if (plane == mInteractiveClipper->getSlicePlane())
		return false;
	mInteractiveClipper->setSlicePlane(plane);
	return true;
}
QString StringPropertyClipPlane::getValue() const
{
	return qstring_cast(mInteractiveClipper->getSlicePlane());
}
QString StringPropertyClipPlane::getHelp() const
{
	return "Chose the slice plane to clip with.";
}
QStringList StringPropertyClipPlane::getValueRange() const
{
	std::vector<PLANE_TYPE> planes = mInteractiveClipper->getAvailableSlicePlanes();
	QStringList retval;
	for (unsigned i = 0; i < planes.size(); ++i)
		retval << qstring_cast(planes[i]);
	return retval;
}

void StringPropertyClipPlane::setClipper(InteractiveClipperPtr clipper)
{
	mInteractiveClipper = clipper;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ClippingWidget::ClippingWidget(VisServicesPtr services, QWidget* parent) :
	BaseWidget(parent, "ClippingWidget", "Clip"),
	mServices(services)
{
	connect(mServices->view().get(), &ViewService::activeLayoutChanged, this, &ClippingWidget::setupUI);
	this->setupUI();
}

void ClippingWidget::setupUI()
{
	if (mInteractiveClipper)
		return;

	mInteractiveClipper.reset(new InteractiveClipper(mServices));

	if (!mInteractiveClipper)
		return;

	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SLOT(clipperChangedSlot()));

	mDataAdapter = StringPropertySelectData::New(mServices->patient());
	LabeledComboBoxWidget* imageCombo = new LabeledComboBoxWidget(this, mDataAdapter);
	connect(mDataAdapter.get(), SIGNAL(changed()), this, SLOT(imageChangedSlot()));

	this->setToolTip("Interactive volume clipping");

	QVBoxLayout* layout = new QVBoxLayout(this);

	QGroupBox* activeClipGroupBox = new QGroupBox("Interactive clipper");
	activeClipGroupBox->setToolTip(this->toolTip());
	layout->addWidget(activeClipGroupBox);
	QVBoxLayout* activeClipLayout = new QVBoxLayout(activeClipGroupBox);

	mPlaneAdapter = StringPropertyClipPlane::New(mInteractiveClipper);
	LabeledComboBoxWidget* combo = new LabeledComboBoxWidget(this, mPlaneAdapter);

	mUseClipperCheckBox = new QCheckBox("Use Clipper");
	mUseClipperCheckBox->setToolTip("Turn on interactive clipping for the selected volume.");
	connect(mUseClipperCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(useClipper(bool)));
	activeClipLayout->addWidget(mUseClipperCheckBox);
	activeClipLayout->addWidget(imageCombo);
	activeClipLayout->addWidget(combo);
	mInvertPlaneCheckBox = new QCheckBox("Invert plane");
	mInvertPlaneCheckBox->setToolTip("Use the inverse (mirror) of the selected slice plane.");
	connect(mInvertPlaneCheckBox, SIGNAL(toggled(bool)), mInteractiveClipper.get(), SLOT(invertPlane(bool)));
	activeClipLayout->addWidget(mInvertPlaneCheckBox);

	QPushButton* saveButton = new QPushButton("Save clip plane");
	saveButton->setToolTip("Save the interactive plane as a clip plane in the selected volume.");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonClickedSlot()));
	//saveButton->setEnabled(false);
	QPushButton* clearButton = new QPushButton("Clear saved planes");
	clearButton->setToolTip("Remove all saved clip planes from the selected volume");
	connect(clearButton, SIGNAL(clicked()), this, SLOT(clearButtonClickedSlot()));
	//clearButton->setEnabled(false);
	activeClipLayout->addWidget(saveButton);
	layout->addWidget(clearButton);

	layout->addStretch();

	this->clipperChangedSlot();
}

void ClippingWidget::clipperChangedSlot()
{
	mUseClipperCheckBox->setChecked(mInteractiveClipper->getUseClipper());
	mInvertPlaneCheckBox->setChecked(mInteractiveClipper->getInvertPlane());
	if (mInteractiveClipper->getData())
		mDataAdapter->setValue(mInteractiveClipper->getData()->getUid());
}

void ClippingWidget::imageChangedSlot()
{
	mInteractiveClipper->setData(mServices->patient()->getData(mDataAdapter->getValue()));
}

void ClippingWidget::clearButtonClickedSlot()
{
	mInteractiveClipper->clearClipPlanesInVolume();
}

void ClippingWidget::saveButtonClickedSlot()
{
	mInteractiveClipper->saveClipPlaneToVolume();
}

}
