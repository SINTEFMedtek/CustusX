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

#include "cxToolMetricWrapper.h"
#include <QHBoxLayout>
#include "sscLabeledComboBoxWidget.h"
#include "cxDataAdapterHelper.h"

namespace cx {

ToolMetricWrapper::ToolMetricWrapper(cx::ToolMetricPtr data) : mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

QWidget* ToolMetricWrapper::createWidget()
{
	this->initializeDataAdapters();

	QWidget* widget = new QWidget;

	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	topLayout->setMargin(0);

	QHBoxLayout* hLayout2 = new QHBoxLayout;
	hLayout2->setMargin(0);
	topLayout->addLayout(hLayout2);

	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	hLayout2->addWidget(createDataWidget(widget, mToolNameSelector));
	hLayout2->addWidget(createDataWidget(widget, mToolOffsetSelector));

	hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mSpaceSelector));

	QPushButton* sampleButton = new QPushButton("Sample");
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	mFrameWidget = new Transform3DWidget(widget);
	connect(mFrameWidget, SIGNAL(changed()), this, SLOT(frameWidgetChangedSlot()));
	topLayout->addWidget(mFrameWidget);

	this->dataChangedSlot();

	return widget;
}

void ToolMetricWrapper::initializeDataAdapters()
{
	QString value;// = qstring_cast(mData->getFrame());
	std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces(true);
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
		range << spaces[i].toString();

	mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
			"Space",
			"Select coordinate system to store position in.",
			value,
			range,
			QDomNode());
	connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));

	mToolNameSelector = ssc::StringDataAdapterXml::initialize("selectToolName",
															  "Tool Name",
															  "The name of the tool",
															  "",
															  QDomNode());
	connect(mToolNameSelector.get(), SIGNAL(valueWasSet()), this, SLOT(toolNameSet()));

	mToolOffsetSelector = ssc::DoubleDataAdapterXml::initialize("selectToolOffset",
																"Tool Offset",
																"Tool Offset",
																0,
																ssc::DoubleRange(0, 100, 1),
																1);
	connect(mToolOffsetSelector.get(), SIGNAL(valueWasSet()), this, SLOT(toolOffsetSet()));
}

QString ToolMetricWrapper::getValue() const
{
	return ssc::prettyFormat(mData->getRefCoord(), 1, 3);
}

ssc::DataPtr ToolMetricWrapper::getData() const
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


void ToolMetricWrapper::moveToToolPosition()
{
	ssc::CoordinateSystem ref = ssc::SpaceHelpers::getR();
	ssc::Transform3D qMt = ssc::SpaceHelpers::getDominantToolTipTransform(mData->getSpace());
//	std::cout << "set frame " << qMt << std::endl;
	mData->setFrame(qMt);
}

void ToolMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
	if (space.mId==ssc::csCOUNT)
		return;
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

void ToolMetricWrapper::dataChangedSlot()
{
	mInternalUpdate = true;

	mSpaceSelector->setValue(mData->getSpace().toString());
	mFrameWidget->setMatrix(mData->getFrame());
	mToolNameSelector->setValue(mData->getToolName());
	mToolOffsetSelector->setValue(mData->getToolOffset());

	mInternalUpdate = false;
}

void ToolMetricWrapper::frameWidgetChangedSlot()
{
	if (mInternalUpdate)
		return;
	ssc::Transform3D matrix = mFrameWidget->getMatrix();
	mData->setFrame(matrix);
}


} //namespace cx
