// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxFrameMetricWrapper.h"

#include <QHBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"

namespace cx {

FrameMetricWrapper::FrameMetricWrapper(cx::FrameMetricPtr data) : mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* FrameMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	QString value;// = qstring_cast(mData->getFrame());
	std::vector<CoordinateSystem> spaces = spaceProvider()->getSpacesToPresentInGUI();
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
		range << spaces[i].toString();

	mSpaceSelector = StringDataAdapterXml::initialize("selectSpace",
			"Space",
			"Select coordinate system to store position in.",
			value,
			range,
			QDomNode());
	hLayout->addWidget(new LabeledComboBoxWidget(widget, mSpaceSelector));

	mFrameWidget = new Transform3DWidget(widget);
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));

	this->addColorWidget(topLayout);

	this->dataChangedSlot();

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
	Transform3D qMt = spaceProvider()->getDominantToolTipTransform(mData->getSpace(), true);
	mData->setFrame(qMt);
}

void FrameMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	CoordinateSystem space = CoordinateSystem::fromString(mSpaceSelector->getValue());
	if (space.mId==csCOUNT)
		return;
	mData->setSpace(space);
}

void FrameMetricWrapper::dataChangedSlot()
{
	mInternalUpdate = true;
	mSpaceSelector->setValue(mData->getSpace().toString());
	mFrameWidget->setMatrix(mData->getFrame());
	mInternalUpdate = false;
}

void FrameMetricWrapper::frameWidgetChangedSlot()
{
	if (mInternalUpdate)
		return;
	Transform3D matrix = mFrameWidget->getMatrix();
	mData->setFrame(matrix);
}


} //namespace cx
