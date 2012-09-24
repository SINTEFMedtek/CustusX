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

namespace cx
{

PointMetricWrapper::PointMetricWrapper(ssc::PointMetricPtr data) : mData(data)
{
  mInternalUpdate = false;
  connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

QWidget* PointMetricWrapper::createWidget()
{
  QWidget* widget = new QWidget;
  QVBoxLayout* topLayout = new QVBoxLayout(widget);
  QHBoxLayout* hLayout = new QHBoxLayout;
  hLayout->setMargin(0);
  topLayout->setMargin(0);
  topLayout->addLayout(hLayout);

  QString value;// = qstring_cast(mData->getFrame());
  std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces();
  QStringList range;
  for (unsigned i=0; i<spaces.size(); ++i)
    range << spaces[i].toString();

  mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
      "Space",
      "Select coordinate system to store position in.",
      value,
      range,
      QDomNode());
  hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mSpaceSelector));

  mCoordinate = ssc::Vector3DDataAdapterXml::initialize("selectCoordinate",
      "Coord",
      "Coordinate values.",
      ssc::Vector3D(0,0,0),
      ssc::DoubleRange(-1000,1000,0.1),
      1,
      QDomNode());
//  topLayout->addWidget(Vector3DWidget::createVerticalWithSliders(widget, mCoordinate));
  topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mCoordinate));

  QPushButton* sampleButton = new QPushButton("Sample");
  sampleButton->setToolTip("Set the position equal to the current tool tip position.");
  hLayout->addWidget(sampleButton);

  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));
  connect(mCoordinate.get(), SIGNAL(valueWasSet()), this, SLOT(coordinateChanged()));
  connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));
  this->dataChangedSlot();

  return widget;
}

QString PointMetricWrapper::getValue() const
{
  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mData->getSpace(), ssc::CoordinateSystem(ssc::csREF));
  ssc::Vector3D p0_r = rM0.coord(mData->getCoordinate());
//  return "pt_r="+qstring_cast(p0_r);
  int w=3;
  return QString("%1 %2 %3").arg(p0_r[0], w, 'f', 1).arg(p0_r[1], w, 'f', 1).arg(p0_r[2], w, 'f', 1);
}

ssc::DataPtr PointMetricWrapper::getData() const
{
  return mData;
}

QString PointMetricWrapper::getType() const
{
  return "point";
}

QString PointMetricWrapper::getArguments() const
{
  ssc::Vector3D p = mData->getCoordinate();
  int w=1;
  QString coord = QString("(%1 %2 %3)").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);
  if (mData->getSpace().mId==ssc::csREF)
  	coord = ""; // ignore display of coord if in ref space

  return mData->getSpace().toString() + " " + coord;
}


void PointMetricWrapper::moveToToolPosition()
{
  ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(mData->getSpace(), true);
  mData->setCoordinate(p);
}

void PointMetricWrapper::spaceSelected()
{
  if (mInternalUpdate)
    return;
  ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
  if (space.mId==ssc::csCOUNT)
    return;
//  std::cout << "selected frame " << frame.toString() << std::endl;
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
  mInternalUpdate = true;
//  std::cout << mData->getUid() <<" frame: " << frameString << " coord: " << mData->getCoordinate() << std::endl;
  mSpaceSelector->setValue(mData->getSpace().toString());
  mCoordinate->setValue(mData->getCoordinate());
  mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


PlaneMetricWrapper::PlaneMetricWrapper(ssc::PlaneMetricPtr data) : mData(data)
{
  mInternalUpdate = false;
  connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

QWidget* PlaneMetricWrapper::createWidget()
{
  QWidget* widget = new QWidget;
  QVBoxLayout* topLayout = new QVBoxLayout(widget);
  QHBoxLayout* hLayout = new QHBoxLayout;
  hLayout->setMargin(0);
  topLayout->setMargin(0);
  topLayout->addLayout(hLayout);

  QString value;// = qstring_cast(mData->getFrame());
  std::vector<ssc::CoordinateSystem> spaces = ssc::SpaceHelpers::getAvailableSpaces();
  QStringList range;
  for (unsigned i=0; i<spaces.size(); ++i)
    range << spaces[i].toString();

  mSpaceSelector = ssc::StringDataAdapterXml::initialize("selectSpace",
      "Space",
      "Select coordinate system to store position in.",
      value,
      range,
      QDomNode());
  hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mSpaceSelector));
  connect(mSpaceSelector.get(), SIGNAL(valueWasSet()), this, SLOT(spaceSelected()));

  mCoordinate = ssc::Vector3DDataAdapterXml::initialize("selectCoordinate",
      "Coord",
      "Coordinate values.",
      ssc::Vector3D(0,0,0),
      ssc::DoubleRange(-1000,1000,0.1),
      1,
      QDomNode());
  topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mCoordinate));
  connect(mCoordinate.get(), SIGNAL(valueWasSet()), this, SLOT(coordinateChanged()));

  mNormal = ssc::Vector3DDataAdapterXml::initialize("selectNormal",
      "Normal",
      "Normal values.",
      ssc::Vector3D(1,0,0),
      ssc::DoubleRange(-1,1,0.1),
      2,
      QDomNode());
  topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mNormal));
  connect(mNormal.get(), SIGNAL(valueWasSet()), this, SLOT(coordinateChanged()));

  QPushButton* sampleButton = new QPushButton("Sample");
  sampleButton->setToolTip("Set the position equal to the current tool tip position.");
  hLayout->addWidget(sampleButton);
  connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));

  this->dataChangedSlot();

  return widget;
}

QString PlaneMetricWrapper::getValue() const
{
  return "NA";
}

ssc::DataPtr PlaneMetricWrapper::getData() const
{
  return mData;
}

QString PlaneMetricWrapper::getType() const
{
  return "plane";
}

QString PlaneMetricWrapper::getArguments() const
{
  return mData->getSpace().toString();
}


void PlaneMetricWrapper::moveToToolPosition()
{
	//  ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(mData->getSpace(), true);
	//  mData->setCoordinate(p);
//	ssc::CoordinateSystem ref = ssc::SpaceHelpers::getR();

	ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
	if (!tool)
	{
		mData->setCoordinate(ssc::Vector3D(0, 0, 0));
		mData->setNormal(ssc::Vector3D(1, 0, 0));
	}
	else
	{
		ssc::CoordinateSystem from(ssc::csTOOL_OFFSET, tool->getUid());
		ssc::Vector3D point_t = ssc::Vector3D(0, 0, 0);
		ssc::Transform3D rMto = ssc::CoordinateSystemHelpers::get_toMfrom(from, mData->getSpace());

		mData->setCoordinate(rMto.coord(ssc::Vector3D(0, 0, 0)));
		mData->setNormal(rMto.vector(ssc::Vector3D(0, 0, 1)));
	}
}

void PlaneMetricWrapper::spaceSelected()
{
  if (mInternalUpdate)
    return;
  ssc::CoordinateSystem space = ssc::CoordinateSystem::fromString(mSpaceSelector->getValue());
  if (space.mId==ssc::csCOUNT)
    return;
  mData->setSpace(space);
}

void PlaneMetricWrapper::coordinateChanged()
{
  if (mInternalUpdate)
    return;
  mData->setCoordinate(mCoordinate->getValue());
  mData->setNormal(mNormal->getValue());
}

void PlaneMetricWrapper::dataChangedSlot()
{
  mInternalUpdate = true;
  mSpaceSelector->setValue(mData->getSpace().toString());
  mCoordinate->setValue(mData->getCoordinate());
  mNormal->setValue(mData->getNormal());
  mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DistanceMetricWrapper::DistanceMetricWrapper(ssc::DistanceMetricPtr data) : mData(data)
{
  mInternalUpdate = false;
  connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

void DistanceMetricWrapper::getAvailableArgumentMetrics(QStringList* uid, std::map<QString,QString>* namemap)
{
  std::map<QString, ssc::DataPtr> data = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
  {
    if (mData->validArgument(iter->second))
    {
      *uid << iter->first;
      (*namemap)[iter->first] = iter->second->getName();
    }
  }
}


QWidget* DistanceMetricWrapper::createWidget()
{
  QWidget* widget = new QWidget;
  QVBoxLayout* topLayout = new QVBoxLayout(widget);
  QHBoxLayout* hLayout = new QHBoxLayout;
  hLayout->setMargin(0);
  topLayout->setMargin(0);
  topLayout->addLayout(hLayout);

  QString value;// = qstring_cast(mData->getFrame());
  QStringList range;
  std::map<QString,QString> names;
  this->getAvailableArgumentMetrics(&range, &names);

  mPSelector.resize(mData->getArgumentCount());
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    mPSelector[i] = ssc::StringDataAdapterXml::initialize(QString("p%1").arg(i),
        QString("p%1").arg(i),
        QString("line endpoint %1").arg(i),
        mData->getArgument(i) ? mData->getArgument(i)->getUid() : "",
        range,
        QDomNode());
    mPSelector[i]->setDisplayNames(names);
    hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mPSelector[i]));
    connect(mPSelector[i].get(), SIGNAL(valueWasSet()), this, SLOT(pointSelected()));
  }

  this->dataChangedSlot();
  return widget;
}

QString DistanceMetricWrapper::getValue() const
{
  return QString("%1 mm").arg(mData->getDistance(), 5, 'f', 1);
}
ssc::DataPtr DistanceMetricWrapper::getData() const
{
  return mData;
}
QString DistanceMetricWrapper::getType() const
{
  return "distance";
}

QString DistanceMetricWrapper::getArguments() const
{
  QStringList data;
  for (unsigned i=0; i<mData->getArgumentCount(); ++i)
    data << (mData->getArgument(i) ? mData->getArgument(i)->getName() : QString("*"));
  return data.join("-");

}


void DistanceMetricWrapper::pointSelected()
{
  if (mInternalUpdate)
    return;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    ssc::DataPtr arg = ssc::dataManager()->getData(mPSelector[i]->getValue());
    if (mData->validArgument(arg))
      mData->setArgument(i, arg);
  }
}

void DistanceMetricWrapper::dataChangedSlot()
{
  mInternalUpdate = true;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    if (mData->getArgument(i))
      mPSelector[i]->setValue(mData->getArgument(i)->getUid());
  }
  mInternalUpdate = false;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


AngleMetricWrapper::AngleMetricWrapper(ssc::AngleMetricPtr data) : mData(data)
{
  mInternalUpdate = false;
  connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

void AngleMetricWrapper::getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap)
{
  std::map<QString, ssc::DataPtr> data = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
  {
    if (boost::shared_dynamic_cast<ssc::PointMetric>(iter->second))
    {
      *uid << iter->first;
      (*namemap)[iter->first] = iter->second->getName();
    }
  }
}

QWidget* AngleMetricWrapper::createWidget()
{
  QWidget* widget = new QWidget;
  QVBoxLayout* topLayout = new QVBoxLayout(widget);
  QHBoxLayout* hLayout = new QHBoxLayout;
  hLayout->setMargin(0);
  topLayout->setMargin(0);
  topLayout->addLayout(hLayout);

  QString value;// = qstring_cast(mData->getFrame());
  QStringList range;
  std::map<QString,QString> names;
  this->getPointMetrics(&range, &names);

  mPSelector.resize(4);
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    mPSelector[i] = ssc::StringDataAdapterXml::initialize(QString("p%1").arg(i),
        QString("p%1").arg(i),
        QString("p%1").arg(i),
        mData->getArgument(i) ? mData->getArgument(i)->getUid() : "",
        range,
        QDomNode());
    mPSelector[i]->setDisplayNames(names);
    hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mPSelector[i]));
    connect(mPSelector[i].get(), SIGNAL(valueWasSet()), this, SLOT(pointSelected()));
  }

  this->dataChangedSlot();
  return widget;
}

QString AngleMetricWrapper::getValue() const
{
  return QString("%1*").arg(mData->getAngle()/M_PI*180, 5, 'f', 1);
}
ssc::DataPtr AngleMetricWrapper::getData() const
{
  return mData;
}
QString AngleMetricWrapper::getType() const
{
  return "angle";
}

QString AngleMetricWrapper::getArguments() const
{
  QStringList data;
  for (unsigned i=0; i<4; ++i)
    data << (mData->getArgument(i) ? mData->getArgument(i)->getName() : QString("*"));
  return data.join("-");
}


void AngleMetricWrapper::pointSelected()
{
  if (mInternalUpdate)
    return;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
	  ssc::PointMetricPtr p = boost::shared_dynamic_cast<ssc::PointMetric>(ssc::dataManager()->getData(mPSelector[i]->getValue()));
    mData->setArgument(i, p);
  }
}

void AngleMetricWrapper::dataChangedSlot()
{
  mInternalUpdate = true;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    if (mData->getArgument(i))
      mPSelector[i]->setValue(mData->getArgument(i)->getUid());
  }
  mInternalUpdate = false;
}



}
