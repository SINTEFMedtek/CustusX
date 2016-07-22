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
#include <cxDataMetricWrappers.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>

#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxTrackingService.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxVector3DWidget.h"
#include "cxRegistrationTransform.h"
#include "cxHelperWidgets.h"
#include "cxBaseWidget.h"
#include "cxBoolProperty.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"
#include "cxSpaceEditWidget.h"
#include "cxSpaceProperty.h"
#include "cxStringListProperty.h"
#include "cxStringListSelectWidget.h"
//TODO :remove
#include "cxLegacySingletons.h"

namespace cx
{

MetricBase::MetricBase(ViewServicePtr viewService, PatientModelServicePtr patientModelService) :
	mViewService(viewService),
	mPatientModelService(patientModelService)
{
}

void MetricBase::colorSelected()
{
	this->getData()->setColor(mColorSelector->getValue());
}

QString MetricBase::getValue() const
{
	QString retval = this->getData()->getValueAsString();
	if (retval.isEmpty())
		return "NA";
	return retval;
}

void MetricBase::addColorWidget(QVBoxLayout* layout)
{
	mColorSelector = ColorProperty::initialize("color", "Color",
													 "Set color of metric",
													 this->getData()->getColor(), QDomNode());
	QHBoxLayout* line = new QHBoxLayout;
	line->addWidget(createDataWidget(mViewService, mPatientModelService, NULL, mColorSelector));
	line->addStretch();
	line->setMargin(0);
	layout->addLayout(line);
	connect(mColorSelector.get(), SIGNAL(valueWasSet()), this, SLOT(colorSelected()));
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MetricReferenceArgumentListGui::MetricReferenceArgumentListGui()
{
	mModified = true;
	mInternalUpdate = false;
}

void MetricReferenceArgumentListGui::setArguments(MetricReferenceArgumentListPtr arguments)
{
	mArguments = arguments;
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SLOT(dataChangedSlot()));
}

void MetricReferenceArgumentListGui::addWidgets(QBoxLayout* layout)
{
	QString value;

	mPSelector.resize(mArguments->getCount());
	for (unsigned i=0; i<mPSelector.size(); ++i)
	{
		mPSelector[i] = StringProperty::initialize(QString("p%1").arg(i),
														 QString("p%1").arg(i),
														 mArguments->getDescription(i),
														 mArguments->get(i) ? mArguments->get(i)->getUid() : "",
														 QStringList(),
														 QDomNode());
//		mPSelector[i]->setDisplayNames(names);
		layout->addWidget(new LabeledComboBoxWidget(NULL, mPSelector[i]));
		connect(mPSelector[i].get(), SIGNAL(valueWasSet()), this, SLOT(pointSelected()));
	}

	this->dataChangedSlot();
}

void MetricReferenceArgumentListGui::getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap)
{
	std::map<QString, DataPtr> data = patientService()->getData();
	for (std::map<QString, DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
	{
		if (mArguments->validArgument(iter->second))
		{
			*uid << iter->first;
			(*namemap)[iter->first] = iter->second->getName();
		}
	}
}

QString MetricReferenceArgumentListGui::getAsString() const
{
	QStringList data;
	for (unsigned i=0; i<mArguments->getCount(); ++i)
		data << (mArguments->get(i) ? mArguments->get(i)->getName() : QString("*"));
	return data.join("-");
}

void MetricReferenceArgumentListGui::pointSelected()
{
	if (mInternalUpdate)
		return;
	for (unsigned i=0; i<mPSelector.size(); ++i)
	{
		DataPtr data = patientService()->getData(mPSelector[i]->getValue());
		if (mArguments->validArgument(data))
			mArguments->set(i, data);
		else
			reportWarning(QString("Failed to set data [%1] in metric, invalid argument.").arg(data?data->getName():"NULL"));
	}
}

void MetricReferenceArgumentListGui::dataChangedSlot()
{
//	mModified = true;
}

void MetricReferenceArgumentListGui::update()
{
	if (!mModified)
		return;
	mInternalUpdate = true;

	QStringList range;
	std::map<QString,QString> names;
	this->getAvailableArgumentMetrics(&range, &names);

	for (unsigned i=0; i<mPSelector.size(); ++i)
	{
		if (!mArguments->get(i))
			continue;
		mPSelector[i]->setValue(mArguments->get(i)->getUid());
		mPSelector[i]->setDisplayNames(names);
		mPSelector[i]->setValueRange(range);
	}

	mInternalUpdate = false;
	mModified = true;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

PointMetricWrapper::PointMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, PointMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mInternalUpdate = false;
//	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
//	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

PointMetricWrapper::~PointMetricWrapper()
{
//	disconnect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* PointMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mSpaceSelector = this->createSpaceSelector();
	hLayout->addWidget(new SpaceEditWidget(widget, mSpaceSelector));

	mCoordinate =  this->createCoordinateSelector();
	topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mCoordinate));

	QWidget* sampleButton = this->createSampleButton(widget);
	hLayout->addWidget(sampleButton);

	this->addColorWidget(topLayout);
	topLayout->addStretch();
//	this->dataChangedSlot();

	return widget;
}

SpacePropertyPtr PointMetricWrapper::createSpaceSelector() const
{
	SpacePropertyPtr retval;
	retval = SpaceProperty::initialize("selectSpace",
											  "Space",
											  "Select coordinate system to store position in.");
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	retval->setSpaceProvider(spaceProvider());
	return retval;
}

Vector3DPropertyPtr PointMetricWrapper::createCoordinateSelector() const
{
	Vector3DPropertyPtr retval;
	retval = Vector3DProperty::initialize("selectCoordinate",
												"Coord",
												"Coordinate values.",
												Vector3D(0,0,0),
												DoubleRange(-1000,1000,1),
												1,
												QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(coordinateChanged()));
	return retval;
}

QWidget* PointMetricWrapper::createSampleButton(QWidget* parent) const
{
	QAction* sampleAction = new QAction("Sample", parent);
	QString sampleTip("Set the position equal to the current tool tip position.");
	sampleAction->setStatusTip(sampleTip);
	sampleAction->setWhatsThis(sampleTip);
	sampleAction->setToolTip(sampleTip);
	connect(sampleAction, SIGNAL(triggered()), this, SLOT(moveToToolPosition()));

	CXToolButton* sampleButton = new CXToolButton(parent);
	sampleButton->setDefaultAction(sampleAction);
	return sampleButton;
}

DataMetricPtr PointMetricWrapper::getData() const
{
	return mData;
}

QString PointMetricWrapper::getType() const
{
	return "point";
}

QString PointMetricWrapper::getArguments() const
{
	Vector3D p = mData->getCoordinate();
	QString coord = prettyFormat(p, 1, 1);
	if (mData->getSpace().mId==csREF)
		coord = ""; // ignore display of coord if in ref space

	return mData->getSpace().toString() + " " + coord;
}

void PointMetricWrapper::moveToToolPosition()
{
	Vector3D p = spaceProvider()->getActiveToolTipPoint(mData->getSpace(), true);
	mData->setCoordinate(p);
}

void PointMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	CoordinateSystem space = mSpaceSelector->getValue();
	if (!space.isValid())
		return;
	mData->setSpace(space);
}

void PointMetricWrapper::coordinateChanged()
{
	if (mInternalUpdate)
		return;
	mData->setCoordinate(mCoordinate->getValue());
}

//void PointMetricWrapper::dataChangedSlot()
//{
//}

void PointMetricWrapper::update()
{
	mInternalUpdate = true;
	mSpaceSelector->setValue(mData->getSpace());
	mCoordinate->setValue(mData->getCoordinate());
	mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

PlaneMetricWrapper::PlaneMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, PlaneMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

PlaneMetricWrapper::~PlaneMetricWrapper()
{
	disconnect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* PlaneMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);
	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();

	return widget;
}

DataMetricPtr PlaneMetricWrapper::getData() const
{
	return mData;
}

QString PlaneMetricWrapper::getType() const
{
	return "plane";
}

QString PlaneMetricWrapper::getArguments() const
{
	return mArguments.getAsString();
}

void PlaneMetricWrapper::dataChangedSlot()
{
//	mInternalUpdate = true;
	mInternalUpdate = false;
}

void PlaneMetricWrapper::update()
{
	mArguments.update();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DistanceMetricWrapper::DistanceMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, DistanceMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(patientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* DistanceMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);
	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();
	return widget;
}

DataMetricPtr DistanceMetricWrapper::getData() const
{
	return mData;
}

QString DistanceMetricWrapper::getType() const
{
	return "distance";
}

QString DistanceMetricWrapper::getArguments() const
{
	return mArguments.getAsString();
}

void DistanceMetricWrapper::dataChangedSlot()
{
//	mInternalUpdate = true;
	mInternalUpdate = false;
}

void DistanceMetricWrapper::update()
{
	mArguments.update();
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

AngleMetricWrapper::AngleMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, AngleMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

AngleMetricWrapper::~AngleMetricWrapper()
{
	disconnect(mPatientModelService.get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* AngleMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mUseSimpleVisualization =  this->createUseSimpleVisualizationSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mUseSimpleVisualization));

	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();
	return widget;
}

DataMetricPtr AngleMetricWrapper::getData() const
{
	return mData;
}
QString AngleMetricWrapper::getType() const
{
	return "angle";
}

QString AngleMetricWrapper::getArguments() const
{
	return mArguments.getAsString();
}

void AngleMetricWrapper::dataChangedSlot()
{
	mInternalUpdate = true;
	mUseSimpleVisualization->setValue(mData->getUseSimpleVisualization());
	mInternalUpdate = false;
}

void AngleMetricWrapper::guiChanged()
{
	if (mInternalUpdate)
		return;
	mData->setUseSimpleVisualization(mUseSimpleVisualization->getValue());
}

BoolPropertyPtr AngleMetricWrapper::createUseSimpleVisualizationSelector() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Simple Visualization", "",
											  "Simple Visualization",
											  mData->getUseSimpleVisualization());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

void AngleMetricWrapper::update()
{
	mArguments.update();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DonutMetricWrapper::DonutMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, DonutMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* DonutMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mRadius =  this->createRadiusSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mRadius));
	mThickness =  this->createThicknessSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mThickness));
	mFlat =  this->createFlatSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mFlat));
	mHeight =  this->createHeightSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mHeight));

	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();
	return widget;
}

DataMetricPtr DonutMetricWrapper::getData() const
{
	return mData;
}
QString DonutMetricWrapper::getType() const
{
	return "donut";
}

QString DonutMetricWrapper::getArguments() const
{
	return mArguments.getAsString();
}

void DonutMetricWrapper::update()
{
	mArguments.update();

	if (mInternalUpdate)
		return;
	mInternalUpdate = true;
	mRadius->setValue(mData->getRadius());
	mThickness->setValue(mData->getThickness());
	mHeight->setValue(mData->getHeight());
	mFlat->setValue(mData->getFlat());
	mInternalUpdate = false;
}

void DonutMetricWrapper::dataChangedSlot()
{
//	if (mInternalUpdate)
//		return;
//	mInternalUpdate = true;
//	mRadius->setValue(mData->getRadius());
//	mThickness->setValue(mData->getThickness());
//	mFlat->setValue(mData->getFlat());
//	mInternalUpdate = false;
}

void DonutMetricWrapper::guiChanged()
{
	if (mInternalUpdate)
		return;
	mInternalUpdate = true;
	mData->setRadius(mRadius->getValue());
	mData->setThickness(mThickness->getValue());
	mData->setHeight(mHeight->getValue());
	mData->setFlat(mFlat->getValue());
	mInternalUpdate = false;
}


DoublePropertyPtr DonutMetricWrapper::createRadiusSelector() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("selectRadius",
											  "Radius",
											  "Donut Radius",
											  mData->getRadius(),
											  DoubleRange(0, 50, 1),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr DonutMetricWrapper::createThicknessSelector() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("selectThickness",
											  "Thickness",
											  "Donut Thickness",
											  mData->getThickness(),
											  DoubleRange(0.05, 1, 0.05),
											  2,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr DonutMetricWrapper::createHeightSelector() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("selectHeight",
											  "Height",
											  "Disc height, NA to torus",
											  mData->getHeight(),
											  DoubleRange(0.0, 100, 1),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolPropertyPtr DonutMetricWrapper::createFlatSelector() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("selectFlat",
											  "Flat",
											  "Flat disk or torus",
											  mData->getFlat(),
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

CustomMetricWrapper::CustomMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, CustomMetricPtr data) :
    MetricBase(viewService, patientModelService),
    mData(data)
{
    mArguments.setArguments(data->getArguments());
    mInternalUpdate = false;
    connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* CustomMetricWrapper::createWidget()
{
    QWidget* widget = new QWidget;
    QVBoxLayout* topLayout = new QVBoxLayout(widget);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    topLayout->setMargin(0);
    topLayout->addLayout(hLayout);

    mArguments.addWidgets(hLayout);

    mDefineVectorUpMethod =  this->createDefineVectorUpMethodSelector();
    topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mDefineVectorUpMethod));
    mSTLFile = this->createSTLFileSelector();
    topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mSTLFile));

    this->addColorWidget(topLayout);
    topLayout->addStretch();

    this->dataChangedSlot();
    return widget;
}

DataMetricPtr CustomMetricWrapper::getData() const
{
    return mData;
}
QString CustomMetricWrapper::getType() const
{
    return "Custom";
}

QString CustomMetricWrapper::getArguments() const
{
    return mArguments.getAsString();
}

void CustomMetricWrapper::update()
{
    mArguments.update();

    if (mInternalUpdate)
        return;
    mInternalUpdate = true;
    mDefineVectorUpMethod->setValue(mData->getDefineVectorUpMethod());
    mSTLFile->setValue(mData->getSTLFile());
    mInternalUpdate = false;
}

void CustomMetricWrapper::dataChangedSlot()
{
//	if (mInternalUpdate)
//		return;
//	mInternalUpdate = true;
//	mRadius->setValue(mData->getRadius());
//	mThickness->setValue(mData->getThickness());
//	mFlat->setValue(mData->getFlat());
//	mInternalUpdate = false;
}

void CustomMetricWrapper::guiChanged()
{
    if (mInternalUpdate)
        return;
    mInternalUpdate = true;
    mData->setDefineVectorUpMethod(mDefineVectorUpMethod->getValue());
    mData->setSTLFile(mSTLFile->getValue());
    mInternalUpdate = false;
}

StringPropertyPtr CustomMetricWrapper::createDefineVectorUpMethodSelector() const
{
    StringPropertyPtr retval;
    retval = StringProperty::initialize("selectDefineVectorUp",
                                              "Use to define the vector up",
                                              "The vector up of the metric will be connected to the static up vector of the operating table or to a frame in p1, which might well be connected to a tool giving a dynamic up vector.",
                                              mData->getDefineVectorUpMethod(),
                                              mData->getDefineVectorUpMethods().getAvailableDefineVectorUpMethods(),
                                              QDomNode());
    retval->setDisplayNames(mData->getDefineVectorUpMethods().getAvailableDefineVectorUpMethodsDisplayNames());


    connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
    return retval;
}

FilePathPropertyPtr CustomMetricWrapper::createSTLFileSelector() const
{
    QStringList paths = QStringList() << qApp->applicationDirPath();
#ifdef __APPLE__
    // special case for running from the build tree, server built as bundle.
    //Jon, necessary?? no compile
    //paths << QString("%1/%2.app/Contents/MacOS").arg(DataLocations::getBundlePath()).arg(filename);
#endif

    FilePathPropertyPtr retval;
    retval = FilePathProperty::initialize("selectSTLFile",
                                          "STL File",
                                          "STL geometry file",
                                          "",
                                          paths,
                                          QDomNode());
    connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
    return retval;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SphereMetricWrapper::SphereMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, SphereMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* SphereMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mRadius =  this->createRadiusSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mRadius));

	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();
	return widget;
}

DataMetricPtr SphereMetricWrapper::getData() const
{
	return mData;
}
QString SphereMetricWrapper::getType() const
{
	return "sphere";
}

QString SphereMetricWrapper::getArguments() const
{
	return mArguments.getAsString();
}

void SphereMetricWrapper::dataChangedSlot()
{

}

void SphereMetricWrapper::update()
{
	mArguments.update();
	mInternalUpdate = true;
	mRadius->setValue(mData->getRadius());
	mInternalUpdate = false;
}

void SphereMetricWrapper::guiChanged()
{
	if (mInternalUpdate)
		return;
	mData->setRadius(mRadius->getValue());
}

DoublePropertyPtr SphereMetricWrapper::createRadiusSelector() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("selectRadius",
											  "Radius",
											  "Sphere Radius",
											  mData->getRadius(),
											  DoubleRange(0, 50, 0.5),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegionOfInterestMetricWrapper::RegionOfInterestMetricWrapper(ViewServicePtr viewService, PatientModelServicePtr patientModelService, RegionOfInterestMetricPtr data) :
	MetricBase(viewService, patientModelService),
	mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* RegionOfInterestMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mDataListProperty = this->createDataListProperty();
	StringListSelectWidget* datalistwidget = new StringListSelectWidget(widget, mDataListProperty);
	topLayout->addWidget(datalistwidget);

	mUseActiveTooltipProperty = this->createUseActiveTooltipSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mUseActiveTooltipProperty));

	mMaxBoundsDataProperty = this->createMaxBoundsDataSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mMaxBoundsDataProperty));

	mMarginProperty = this->createMarginSelector();
	topLayout->addWidget(createDataWidget(mViewService, mPatientModelService, widget, mMarginProperty));

	this->addColorWidget(topLayout);
	topLayout->addStretch();

	this->dataChangedSlot();
	return widget;
}

DataMetricPtr RegionOfInterestMetricWrapper::getData() const
{
	return mData;
}
QString RegionOfInterestMetricWrapper::getType() const
{
	return "roi";
}

QString RegionOfInterestMetricWrapper::getArguments() const
{
	return "";
}

void RegionOfInterestMetricWrapper::dataChangedSlot()
{

}

StringListPropertyPtr RegionOfInterestMetricWrapper::createDataListProperty()
{
	StringListPropertyPtr retval = StringListProperty::initialize("data_list",
														"Data",
														"Select data to define ROI",
														QStringList(),
														QStringList());
	connect(retval.get(), &Property::changed, this, &RegionOfInterestMetricWrapper::guiChanged);
	return retval;
}

StringPropertyPtr RegionOfInterestMetricWrapper::createMaxBoundsDataSelector()
{
	StringPropertyPtr retval;
	retval = StringProperty::initialize("max_bounds_data",
										"Max Bounds",
										"Select data to define maximal extent of ROI",
										"",
										QStringList(),
										QDomNode());
	connect(retval.get(), &Property::changed, this, &RegionOfInterestMetricWrapper::guiChanged);
	return retval;
}

DoublePropertyPtr RegionOfInterestMetricWrapper::createMarginSelector() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("margin",
										"Margin",
										"Margin added outside the data",
										0,
										DoubleRange(0, 100, 1),
										1,
										QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolPropertyPtr RegionOfInterestMetricWrapper::createUseActiveTooltipSelector() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Use Tool Tip", "",
											  "Include tool tip in the roi",
											  false);

	connect(retval.get(), &Property::changed, this, &RegionOfInterestMetricWrapper::guiChanged);
	return retval;
}


void RegionOfInterestMetricWrapper::update()
{
	mInternalUpdate = true;

	QStringList data;
	std::map<QString, QString> names;
	std::map<QString, DataPtr> alldata = mPatientModelService->getData();
	for (std::map<QString, DataPtr>::iterator i=alldata.begin(); i!=alldata.end(); ++i)
	{
		if (i->first == mData->getUid())
			continue;
		data << i->first;
		names[i->first] = i->second->getName();
	}

	mDataListProperty->setValue(mData->getDataList());
	mDataListProperty->setValueRange(data);
	mDataListProperty->setDisplayNames(names);

	mMaxBoundsDataProperty->setValue(mData->getMaxBoundsData());
	mMaxBoundsDataProperty->setValueRange(data);
	mMaxBoundsDataProperty->setDisplayNames(names);

	mMarginProperty->setValue(mData->getMargin());
	mUseActiveTooltipProperty->setValue(mData->getUseActiveTooltip());

	mInternalUpdate = false;
}

void RegionOfInterestMetricWrapper::guiChanged()
{
	if (mInternalUpdate)
		return;
	mData->setDataList(mDataListProperty->getValue());
	mData->setUseActiveTooltip(mUseActiveTooltipProperty->getValue());
	mData->setMargin(mMarginProperty->getValue());
	mData->setMaxBoundsData(mMaxBoundsDataProperty->getValue());
}




}
