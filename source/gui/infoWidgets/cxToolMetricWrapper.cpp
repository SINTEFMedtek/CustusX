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

#include "cxToolMetricWrapper.h"
#include <QHBoxLayout>
#include "cxSpaceEditWidget.h"
#include "cxHelperWidgets.h"
#include "cxTrackingService.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"

//TODO: remove
#include "cxLegacySingletons.h"

namespace cx {

ToolMetricWrapper::ToolMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, cx::ToolMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mInternalUpdate = false;
//	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
//	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
//	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

ToolMetricWrapper::~ToolMetricWrapper()
{
//	disconnect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* ToolMetricWrapper::createWidget()
{
	this->initializeProperties();

	QWidget* widget = new QWidget;

	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	topLayout->setMargin(0);

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->setMargin(0);
	topLayout->addLayout(hLayout2);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	hLayout2->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mToolNameSelector));
	hLayout2->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mToolOffsetSelector));

	hLayout->addWidget(new SpaceEditWidget(widget, mSpaceSelector));

	QPushButton* sampleButton = new QPushButton("Sample");
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(resampleMetric()));
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	mFrameWidget = new Transform3DWidget(widget);
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	this->addColorWidget(topLayout);

//	this->dataChangedSlot();

	return widget;
}

void ToolMetricWrapper::initializeProperties()
{
	mSpaceSelector = SpaceProperty::initialize("selectSpace",
											  "Space",
											  "Select coordinate system to store position in.");

	mSpaceSelector->setSpaceProvider(spaceProvider());
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));

	mToolNameSelector = StringProperty::initialize("selectToolName",
															  "Tool Name",
															  "The name of the tool",
															  "",
															  QDomNode());
	connect(mToolNameSelector.get(), SIGNAL(valueWasSet()), this, SLOT(toolNameSet()));

	mToolOffsetSelector = DoubleProperty::initialize("selectToolOffset",
																"Tool Offset",
																"Tool Offset",
																0,
																DoubleRange(0, 100, 1),
																1);
	connect(mToolOffsetSelector.get(), SIGNAL(valueWasSet()), this, SLOT(toolOffsetSet()));
}

QString ToolMetricWrapper::getValue() const
{
	return prettyFormat(mData->getRefCoord(), 1, 3);
}

DataMetricPtr ToolMetricWrapper::getData() const
{
	return mData;
}

QString ToolMetricWrapper::getType() const
{
	return "tool";
}

QString ToolMetricWrapper::getArguments() const
{
	return mData->getSpace().toString();
}


void ToolMetricWrapper::resampleMetric()
{
//	CoordinateSystem ref = CoordinateSystemHelpers::getR();
	Transform3D qMt = spaceProvider()->getActiveToolTipTransform(mData->getSpace(), true);
	mData->setFrame(qMt);
	mData->setToolName(trackingService()->getActiveTool()->getName());
	mData->setToolOffset(trackingService()->getActiveTool()->getTooltipOffset());
}


void ToolMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	CoordinateSystem space = mSpaceSelector->getValue();
	if (space.isValid())
		mData->setSpace(space);
}

void ToolMetricWrapper::toolNameSet()
{
	if (mInternalUpdate)
		return;
	mData->setToolName(mToolNameSelector->getValue());
}

void ToolMetricWrapper::toolOffsetSet()
{
	if (mInternalUpdate)
		return;
	mData->setToolOffset(mToolOffsetSelector->getValue());
}

//void ToolMetricWrapper::dataChangedSlot()
//{
//}

void ToolMetricWrapper::update()
{
	mInternalUpdate = true;

	mSpaceSelector->setValue(mData->getSpace());
	mFrameWidget->setMatrix(mData->getFrame());
	mToolNameSelector->setValue(mData->getToolName());
	mToolOffsetSelector->setValue(mData->getToolOffset());

	mInternalUpdate = false;
}


void ToolMetricWrapper::frameWidgetChangedSlot()
{
	if (mInternalUpdate)
		return;
	Transform3D matrix = mFrameWidget->getMatrix();
	mData->setFrame(matrix);
}


} //namespace cx
