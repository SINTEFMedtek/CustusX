/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxToolMetricWrapper.h"
#include <QHBoxLayout>
#include "cxSpaceEditWidget.h"
#include "cxHelperWidgets.h"
#include "cxTrackingService.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"

namespace cx {

ToolMetricWrapper::ToolMetricWrapper(VisServicesPtr services, cx::ToolMetricPtr data) :
	MetricBase(services),
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

	QWidget* widget = this->newWidget("tool_metric");

	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	topLayout->setMargin(0);

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->setMargin(0);
	topLayout->addLayout(hLayout2);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	hLayout2->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mToolNameSelector));
	hLayout2->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mToolOffsetSelector));

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

	mSpaceSelector->setSpaceProvider(mServices->spaceProvider());
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
	Transform3D qMt = mServices->spaceProvider()->getActiveToolTipTransform(mData->getSpace(), true);
	mData->setFrame(qMt);
	mData->setToolName(mServices->tracking()->getActiveTool()->getName());
	mData->setToolOffset(mServices->tracking()->getActiveTool()->getTooltipOffset());
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
