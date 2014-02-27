/*
 * cxDataMetricWrappers.cpp
 *
 *  \date Jul 29, 2011
 *      \author christiana
 */

#include <cxDataMetricWrappers.h>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QHeaderView>

#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscCoordinateSystemHelpers.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxVector3DWidget.h"
#include "sscRegistrationTransform.h"
#include "cxDataAdapterHelper.h"
#include "cxBaseWidget.h"
#include "sscBoolDataAdapterXml.h"

#include "cxLegacySingletons.h"
#include "cxSpaceProvider.h"

namespace cx
{

MetricBase::MetricBase() //: mModified(true)
{
//	mInternalUpdate = false;
//	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
//	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
//	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
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
	mColorSelector = ColorDataAdapterXml::initialize("color", "Color",
													 "Set color of metric",
													 this->getData()->getColor(), QDomNode());
	QHBoxLayout* line = new QHBoxLayout;
	line->addWidget(createDataWidget(NULL, mColorSelector));
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
//	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
}

void MetricReferenceArgumentListGui::setArguments(MetricReferenceArgumentListPtr arguments)
{
	mArguments = arguments;
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SLOT(dataChangedSlot()));
//	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(argumentsChanged()));
}

void MetricReferenceArgumentListGui::addWidgets(QBoxLayout* layout)
{
	QString value;// = qstring_cast(mData->getFrame());
//	QStringList range;
//	std::map<QString,QString> names;
//	this->getAvailableArgumentMetrics(&range, &names);

	mPSelector.resize(mArguments->getCount());
	for (unsigned i=0; i<mPSelector.size(); ++i)
	{
		mPSelector[i] = StringDataAdapterXml::initialize(QString("p%1").arg(i),
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
	std::map<QString, DataPtr> data = dataManager()->getData();
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
		DataPtr data = dataManager()->getData(mPSelector[i]->getValue());
		if (mArguments->validArgument(data))
			mArguments->set(i, data);
		else
			messageManager()->sendWarning(QString("Failed to set data [%1] in metric, invalid argument.").arg(data?data->getName():"NULL"));
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

PointMetricWrapper::PointMetricWrapper(PointMetricPtr data) : mData(data)
{
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
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
	hLayout->addWidget(new LabeledComboBoxWidget(widget, mSpaceSelector));

	mCoordinate =  this->createCoordinateSelector();
	topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mCoordinate));

	QWidget* sampleButton = this->createSampleButton(widget);
	hLayout->addWidget(sampleButton);

	this->addColorWidget(topLayout);
	topLayout->addStretch();
	this->dataChangedSlot();

	return widget;
}

StringDataAdapterXmlPtr PointMetricWrapper::createSpaceSelector() const
{
	QString value;// = qstring_cast(mData->getFrame());
	std::vector<CoordinateSystem> spaces = spaceProvider()->getSpacesToPresentInGUI();
	QStringList range;
	for (unsigned i=0; i<spaces.size(); ++i)
		range << spaces[i].toString();

	StringDataAdapterXmlPtr retval;
	retval = StringDataAdapterXml::initialize("selectSpace",
											  "Space",
											  "Select coordinate system to store position in.",
											  value,
											  range,
											  QDomNode());
	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
	return retval;
}

Vector3DDataAdapterXmlPtr PointMetricWrapper::createCoordinateSelector() const
{
	Vector3DDataAdapterXmlPtr retval;
	retval = Vector3DDataAdapterXml::initialize("selectCoordinate",
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
	Vector3D p = spaceProvider()->getDominantToolTipPoint(mData->getSpace(), true);
	mData->setCoordinate(p);
}

void PointMetricWrapper::spaceSelected()
{
	if (mInternalUpdate)
		return;
	CoordinateSystem space = CoordinateSystem::fromString(mSpaceSelector->getValue());
	if (space.mId==csCOUNT)
		return;
	mData->setSpace(space);
}

void PointMetricWrapper::coordinateChanged()
{
	if (mInternalUpdate)
		return;
	mData->setCoordinate(mCoordinate->getValue());
}

void PointMetricWrapper::dataChangedSlot()
{

}

void PointMetricWrapper::update()
{
	mInternalUpdate = true;
	mSpaceSelector->setValue(mData->getSpace().toString());
	mCoordinate->setValue(mData->getCoordinate());
	mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

PlaneMetricWrapper::PlaneMetricWrapper(PlaneMetricPtr data) : mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(mData.get(), SIGNAL(propertiesChanged()), this, SLOT(dataChangedSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
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
	mInternalUpdate = true;
	mInternalUpdate = false;
}

void PlaneMetricWrapper::update()
{
	mArguments.update();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DistanceMetricWrapper::DistanceMetricWrapper(DistanceMetricPtr data) : mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
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
	mInternalUpdate = true;
	mInternalUpdate = false;
}

void DistanceMetricWrapper::update()
{
	mArguments.update();
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

AngleMetricWrapper::AngleMetricWrapper(AngleMetricPtr data) : mData(data)
{
	mArguments.setArguments(data->getArguments());
	mInternalUpdate = false;
	connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataChangedSlot()));
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
	topLayout->addWidget(createDataWidget(widget, mUseSimpleVisualization));

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

BoolDataAdapterXmlPtr AngleMetricWrapper::createUseSimpleVisualizationSelector() const
{
	BoolDataAdapterXmlPtr retval;
	retval = BoolDataAdapterXml::initialize("Simple Visualization", "",
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

DonutMetricWrapper::DonutMetricWrapper(DonutMetricPtr data) : mData(data)
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
	topLayout->addWidget(createDataWidget(widget, mRadius));
	mThickness =  this->createThicknessSelector();
	topLayout->addWidget(createDataWidget(widget, mThickness));
	mFlat =  this->createFlatSelector();
	topLayout->addWidget(createDataWidget(widget, mFlat));
	mHeight =  this->createHeightSelector();
	topLayout->addWidget(createDataWidget(widget, mHeight));

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


DoubleDataAdapterXmlPtr DonutMetricWrapper::createRadiusSelector() const
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("selectRadius",
											  "Radius",
											  "Donut Radius",
											  mData->getRadius(),
											  DoubleRange(0, 50, 1),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoubleDataAdapterXmlPtr DonutMetricWrapper::createThicknessSelector() const
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("selectThickness",
											  "Thickness",
											  "Donut Thickness",
											  mData->getThickness(),
											  DoubleRange(0.05, 1, 0.05),
											  2,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

DoubleDataAdapterXmlPtr DonutMetricWrapper::createHeightSelector() const
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("selectHeight",
											  "Height",
											  "Disc height, NA to torus",
											  mData->getHeight(),
											  DoubleRange(0.0, 100, 1),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}

BoolDataAdapterXmlPtr DonutMetricWrapper::createFlatSelector() const
{
	BoolDataAdapterXmlPtr retval;
	retval = BoolDataAdapterXml::initialize("selectFlat",
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

SphereMetricWrapper::SphereMetricWrapper(SphereMetricPtr data) : mData(data)
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
	topLayout->addWidget(createDataWidget(widget, mRadius));

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

DoubleDataAdapterXmlPtr SphereMetricWrapper::createRadiusSelector() const
{
	DoubleDataAdapterXmlPtr retval;
	retval = DoubleDataAdapterXml::initialize("selectRadius",
											  "Radius",
											  "Sphere Radius",
											  mData->getRadius(),
											  DoubleRange(0, 50, 0.5),
											  1,
											  QDomNode());

	connect(retval.get(), SIGNAL(valueWasSet()), this, SLOT(guiChanged()));
	return retval;
}



}
