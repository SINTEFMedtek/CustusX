/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFrameMetricWrapper.h"

#include <QHBoxLayout>
#include "cxLabeledComboBoxWidget.h"
#include "cxSpaceProvider.h"
#include "cxSpaceEditWidget.h"
#include "cxVisServices.h"

namespace cx {

FrameMetricWrapper::FrameMetricWrapper(VisServicesPtr services, cx::FrameMetricPtr data) :
	MetricBase(services),
	mData(data)
{
	mInternalUpdate = false;
}

FrameMetricWrapper::~FrameMetricWrapper()
{
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
	mSpaceSelector->setSpaceProvider(mServices->spaceProvider());
	hLayout->addWidget(new SpaceEditWidget(widget, mSpaceSelector));

	mFrameWidget = new Transform3DWidget(widget);
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));

	this->addColorWidget(topLayout);

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
	Transform3D qMt = mServices->spaceProvider()->getActiveToolTipTransform(mData->getSpace(), true);
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
