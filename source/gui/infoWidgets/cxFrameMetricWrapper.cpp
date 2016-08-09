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

#include "cxFrameMetricWrapper.h"

#include <QHBoxLayout>
#include "cxLabeledComboBoxWidget.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxSpaceEditWidget.h"

namespace cx {

FrameMetricWrapper::FrameMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, cx::FrameMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mInternalUpdate = false;
//	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
//	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

FrameMetricWrapper::~FrameMetricWrapper()
{
//	disconnect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* FrameMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("frame_metric");
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mSpaceSelector = SpaceProperty::initialize("selectSpace",
											  "Space",
											  "Select coordinate system to store position in.");
	mSpaceSelector->setSpaceProvider(spaceProvider());
	hLayout->addWidget(new SpaceEditWidget(widget, mSpaceSelector));

	mFrameWidget = new Transform3DWidget(widget);
//	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
//	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));

	this->addColorWidget(topLayout);

//	this->dataChangedSlot();

	return widget;
}

QString FrameMetricWrapper::getValue() const
{
	return prettyFormat(mData->getRefCoord(), 1, 3);
}

DataMetricPtr FrameMetricWrapper::getData() const
{
	return mData;
}

QString FrameMetricWrapper::getType() const
{
	return "frame";
}

QString FrameMetricWrapper::getArguments() const
{
	return mData->getSpace().toString();
}


void FrameMetricWrapper::moveToToolPosition()
{
//	CoordinateSystem ref = CoordinateSystem::reference()
	Transform3D qMt = spaceProvider()->getActiveToolTipTransform(mData->getSpace(), true);
	mData->setFrame(qMt);
}

void FrameMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	CoordinateSystem space = mSpaceSelector->getValue();
	if (space.isValid())
		mData->setSpace(space);
}

//void FrameMetricWrapper::dataChangedSlot()
//{
//}

void FrameMetricWrapper::frameWidgetChangedSlot()
{
	if (mInternalUpdate)
		return;
	Transform3D matrix = mFrameWidget->getMatrix();
	mData->setFrame(matrix);
}

void FrameMetricWrapper::update()
{
	mInternalUpdate = true;
	mSpaceSelector->setValue(mData->getSpace());
	mFrameWidget->setMatrix(mData->getFrame());
	mInternalUpdate = false;
}



} //namespace cx
