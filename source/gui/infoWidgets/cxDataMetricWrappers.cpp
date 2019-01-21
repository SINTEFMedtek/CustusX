/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxVisServices.h"
#include "cxImage.h"

namespace cx
{

MetricBase::MetricBase(VisServicesPtr services) :
	mServices(services)
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
	line->addWidget(createDataWidget(mServices->view(), mServices->patient(), NULL, mColorSelector));
	line->addStretch();
	line->setMargin(0);
	layout->addLayout(line);
	connect(mColorSelector.get(), SIGNAL(valueWasSet()), this, SLOT(colorSelected()));
}

QWidget *cx::MetricBase::newWidget(QString objectName)
{
	QWidget* widget = new QWidget;
	widget->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
	widget->setObjectName(objectName);
	return widget;
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MetricReferenceArgumentListGui::MetricReferenceArgumentListGui(VisServicesPtr services) :
	mServices(services)
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
	std::map<QString, DataPtr> data = mServices->patient()->getDatas();
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
		DataPtr data = mServices->patient()->getData(mPSelector[i]->getValue());
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

PointMetricWrapper::PointMetricWrapper(VisServicesPtr services, PointMetricPtr data) :
	MetricBase(services),
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
	QWidget* widget = this->newWidget("point_metric");
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
	retval->setSpaceProvider(mServices->spaceProvider());
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
	Vector3D p = mServices->spaceProvider()->getActiveToolTipPoint(mData->getSpace(), true);
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

PlaneMetricWrapper::PlaneMetricWrapper(VisServicesPtr services, PlaneMetricPtr data) :
	MetricBase(services),
	mData(data),
	mArguments(services)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

PlaneMetricWrapper::~PlaneMetricWrapper()
{
	disconnect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* PlaneMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("plane_metric");
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

DistanceMetricWrapper::DistanceMetricWrapper(VisServicesPtr services, DistanceMetricPtr data) :
	MetricBase(services),
	mData(data),
	mArguments(services)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* DistanceMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("distance_metric");
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

AngleMetricWrapper::AngleMetricWrapper(VisServicesPtr services, AngleMetricPtr data) :
	MetricBase(services),
	mData(data),
	mArguments(services)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

AngleMetricWrapper::~AngleMetricWrapper()
{
	disconnect(mServices->patient().get(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

QWidget* AngleMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("angle_metric");
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mUseSimpleVisualization =  this->createUseSimpleVisualizationSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mUseSimpleVisualization));

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

DonutMetricWrapper::DonutMetricWrapper(VisServicesPtr services, DonutMetricPtr data) :
	MetricBase(services),
	mData(data),
	mArguments(services)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* DonutMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("donut_metric");
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mRadius =  this->createRadiusSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mRadius));
	mThickness =  this->createThicknessSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mThickness));
	mFlat =  this->createFlatSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mFlat));
	mHeight =  this->createHeightSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mHeight));

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

CustomMetricWrapper::CustomMetricWrapper(VisServicesPtr services, CustomMetricPtr data) :
	MetricBase(services),
	mData(data),
	mScaleToP1Widget(NULL),
	mArguments(services)
{
    mArguments.setArguments(data->getArguments());
    mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* CustomMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("custom_metric");
    QVBoxLayout* topLayout = new QVBoxLayout(widget);
    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    topLayout->setMargin(0);
    topLayout->addLayout(hLayout);

    mArguments.addWidgets(hLayout);

    mDefineVectorUpMethod =  this->createDefineVectorUpMethodSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mDefineVectorUpMethod));
	mModel = this->createModelSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mModel));

	mOffsetFromP0 = this->createOffsetFromP0();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mOffsetFromP0));
	mOffsetFromP1 = this->createOffsetFromP1();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mOffsetFromP1));
	mRepeatDistance = this->createRepeatDistance();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mRepeatDistance));

	mShowDistanceMarkers = this->createShowDistanceMarkers();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mShowDistanceMarkers));
	mDistanceMarkerVisibility = this->createDistanceMarkerVisibility();
	mDistanceMarkerVisibilityWidget = createDataWidget(mServices->view(), mServices->patient(), widget, mDistanceMarkerVisibility);
	topLayout->addWidget(mDistanceMarkerVisibilityWidget);

	mScaleToP1 = this->createScaletoP1();
	mScaleToP1Widget = createDataWidget(mServices->view(), mServices->patient(), widget, mScaleToP1);
	topLayout->addWidget(mScaleToP1Widget);

	mTranslationOnly= this->createTranslationOnly();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mTranslationOnly));

	mTextureFollowTool= this->createTextureFollowTool();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mTextureFollowTool));

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
	mModel->setValue(mData->getModelUid());
    mInternalUpdate = false;
	guiChanged();
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

	if(mModel->getData() && mModel->getData()->getType() == Image::getTypeName())
		mScaleToP1Widget->setEnabled(false);
	else
		mScaleToP1Widget->setEnabled(true);

	if(mShowDistanceMarkers->getValue())
		mDistanceMarkerVisibilityWidget->setEnabled(true);
	else
		mDistanceMarkerVisibilityWidget->setEnabled(false);

    mInternalUpdate = true;
    mData->setDefineVectorUpMethod(mDefineVectorUpMethod->getValue());
	mData->setModelUid(mModel->getValue());
	mData->setOffsetFromP0(mOffsetFromP0->getValue());
	mData->setOffsetFromP1(mOffsetFromP1->getValue());
	mData->setRepeatDistance(mRepeatDistance->getValue());
	mData->setScaleToP1(mScaleToP1->getValue());
	mData->setShowDistanceMarkers(mShowDistanceMarkers->getValue());
	mData->setDistanceMarkerVisibility(mDistanceMarkerVisibility->getValue());
	mData->setTranslationOnly(mTranslationOnly->getValue());
	mData->setTextureFollowTool(mTextureFollowTool->getValue());

	mInternalUpdate = false;
}

BoolPropertyPtr CustomMetricWrapper::createScaletoP1() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Scale to P1", "",
										  "Scale model so that it fits between P0 and P1",
										  mData->getScaleToP1());
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolPropertyPtr CustomMetricWrapper::createShowDistanceMarkers() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Show distance markers", "",
										  "Show distance to P0 for each repeated model",
										  mData->getShowDistanceMarkers());
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolPropertyPtr CustomMetricWrapper::createTranslationOnly() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Translation Only", "",
										  "Ignore scale and rotate",
										  mData->getTranslationOnly());
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolPropertyPtr CustomMetricWrapper::createTextureFollowTool() const
{
	BoolPropertyPtr retval;
	retval = BoolProperty::initialize("Texture Follow Tool", "",
										  "Any texture on the model will move with the tool",
										  mData->getTextureFollowTool());
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr CustomMetricWrapper::createOffsetFromP0() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("Offset from P0", "",
											"Position model an offset from P0 towards P1",
											mData->getOffsetFromP0(), DoubleRange(-100, 100, 1), 0);
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr CustomMetricWrapper::createOffsetFromP1() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("Offset from P1", "",
											"When scaling to P1, scale to a point offset from P1 towards P0",
											mData->getOffsetFromP1(), DoubleRange(-100, 100, 1), 0);
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr CustomMetricWrapper::createRepeatDistance() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("Repeat Distance", "",
											"Repeat model with this distance",
											mData->getRepeatDistance(), DoubleRange(0, 100, 1), 0);
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoublePropertyPtr CustomMetricWrapper::createDistanceMarkerVisibility() const
{
	DoublePropertyPtr retval;
	retval = DoubleProperty::initialize("Distance markers visibility threshold", "",
											"Hide the markers if the distance to the camera exceeds this threshold",
											mData->getDistanceMarkerVisibility(), DoubleRange(0, 1000, 1), 0);
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

StringPropertyPtr CustomMetricWrapper::createDefineVectorUpMethodSelector() const
{
	StringPropertyPtr retval;
	retval = StringProperty::initialize("selectDefineVectorUp",
										"Use to define the vector up",
										"The vector up of the metric will be connected to one of:\n"
										"- a) The static up vector of the operating table\n"
										"- b) To a frame in p1, which might well be connected "
										"to a tool giving a dynamic up vector.\n"
										"- c) The tool up (tool negative x)",
										mData->getDefineVectorUpMethod(),
										mData->getDefineVectorUpMethods().getAvailableDefineVectorUpMethods(),
										QDomNode());
	retval->setDisplayNames(mData->getDefineVectorUpMethods().getAvailableDefineVectorUpMethodsDisplayNames());


	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

StringPropertySelectDataPtr CustomMetricWrapper::createModelSelector() const
{
	StringPropertySelectDataPtr retval;
	retval = StringPropertySelectData::New(mServices->patient(), "image|mesh");
	retval->setOnly2DImagesFilter(true);
	retval->setValueName("Model");
	retval->setHelp("Model can be mesh or 2D image");
	connect(retval.get(), &StringPropertySelectData::changed, this, &CustomMetricWrapper::guiChanged);
	return retval;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SphereMetricWrapper::SphereMetricWrapper(VisServicesPtr services, SphereMetricPtr data) :
	MetricBase(services),
	mData(data),
	mArguments(services)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* SphereMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("sphere_metric");
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mArguments.addWidgets(hLayout);

	mRadius =  this->createRadiusSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mRadius));

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

RegionOfInterestMetricWrapper::RegionOfInterestMetricWrapper(VisServicesPtr services, RegionOfInterestMetricPtr data) :
	MetricBase(services),
	mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
}

QWidget* RegionOfInterestMetricWrapper::createWidget()
{
	QWidget* widget = this->newWidget("region_of_interest_metric");
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	QHBoxLayout* hLayout = new QHBoxLayout;
	hLayout->setMargin(0);
	topLayout->setMargin(0);
	topLayout->addLayout(hLayout);

	mDataListProperty = this->createDataListProperty();
	StringListSelectWidget* datalistwidget = new StringListSelectWidget(widget, mDataListProperty);
	topLayout->addWidget(datalistwidget);

	mUseActiveTooltipProperty = this->createUseActiveTooltipSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mUseActiveTooltipProperty));

	mMaxBoundsDataProperty = this->createMaxBoundsDataSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mMaxBoundsDataProperty));

	mMarginProperty = this->createMarginSelector();
	topLayout->addWidget(createDataWidget(mServices->view(), mServices->patient(), widget, mMarginProperty));

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
	std::map<QString, DataPtr> alldata = mServices->patient()->getDatas();
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
