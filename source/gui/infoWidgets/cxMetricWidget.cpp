/*
 * cxMetricWidget.cpp
 *
 *  Created on: Jul 5, 2011
 *      Author: christiana
 */

#include <cxMetricWidget.h>

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
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxVector3DWidget.h"

namespace cx
{

PointMetricWrapper::PointMetricWrapper(PointMetricPtr data) : mData(data)
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

	mFrameSelector = ssc::StringDataAdapterXml::initialize("selectFrame",
			"Frame",
			"Select coordinate system to store position in.",
      value,
      range,
      QDomNode());
	hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mFrameSelector));

	mCoordinate = ssc::Vector3DDataAdapterXml::initialize("selectCoordinate",
			"Coord",
			"Coordinate values.",
      ssc::Vector3D(0,0,0),
      ssc::DoubleRange(-1000,1000,0.1),
      1,
      QDomNode());
//	topLayout->addWidget(Vector3DWidget::createVerticalWithSliders(widget, mCoordinate));
  topLayout->addWidget(Vector3DWidget::createSmallHorizontal(widget, mCoordinate));

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	hLayout->addWidget(sampleButton);

	connect(mFrameSelector.get(), SIGNAL(valueWasSet()), this, SLOT(frameSelected()));
	connect(mCoordinate.get(), SIGNAL(valueWasSet()), this, SLOT(coordinateChanged()));
	connect(sampleButton, SIGNAL(clicked()), this, SLOT(moveToToolPosition()));
	this->dataChangedSlot();

	return widget;
}

QString PointMetricWrapper::getValue() const
{
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mData->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mData->getCoordinate());
//	return "pt_r="+qstring_cast(p0_r);
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

  return mData->getFrame().toString() + " " + coord;
}


void PointMetricWrapper::moveToToolPosition()
{
  ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(mData->getFrame(), true);
  mData->setCoordinate(p);
}

void PointMetricWrapper::frameSelected()
{
  if (mInternalUpdate)
    return;
	ssc::CoordinateSystem frame = ssc::CoordinateSystem::fromString(mFrameSelector->getValue());
	if (frame.mId==ssc::csCOUNT)
		return;
//	std::cout << "selected frame " << frame.toString() << std::endl;
	mData->setFrame(frame);
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
	mFrameSelector->setValue(mData->getFrame().toString());
	mCoordinate->setValue(mData->getCoordinate());
  mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


PlaneMetricWrapper::PlaneMetricWrapper(PlaneMetricPtr data) : mData(data)
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

  mFrameSelector = ssc::StringDataAdapterXml::initialize("selectFrame",
      "Frame",
      "Select coordinate system to store position in.",
      value,
      range,
      QDomNode());
  hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mFrameSelector));
  connect(mFrameSelector.get(), SIGNAL(valueWasSet()), this, SLOT(frameSelected()));

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
//  ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mData->getFrame(), ssc::CoordinateSystem(ssc::csREF));
//  ssc::Vector3D p0_r = rM0.coord(mData->getCoordinate());
////  return "pt_r="+qstring_cast(p0_r);
//  int w=3;
//  return QString("p_r=%1 %2 %3").arg(p0_r[0], w, 'f', 1).arg(p0_r[1], w, 'f', 1).arg(p0_r[2], w, 'f', 1);
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
  return "";
//  ssc::Vector3D p = mData->getCoordinate();
//  int w=1;
//  QString coord = QString("(%1 %2 %3)").arg(p[0], w, 'f', 1).arg(p[1], w, 'f', 1).arg(p[2], w, 'f', 1);
//
//  return mData->getFrame().toString() + " " + coord;
}


void PlaneMetricWrapper::moveToToolPosition()
{
  ssc::Vector3D p = ssc::SpaceHelpers::getDominantToolTipPoint(mData->getFrame(), true);
  mData->setCoordinate(p);
}

void PlaneMetricWrapper::frameSelected()
{
  if (mInternalUpdate)
    return;
  ssc::CoordinateSystem frame = ssc::CoordinateSystem::fromString(mFrameSelector->getValue());
  if (frame.mId==ssc::csCOUNT)
    return;
  mData->setFrame(frame);
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
  mFrameSelector->setValue(mData->getFrame().toString());
  mCoordinate->setValue(mData->getCoordinate());
  mNormal->setValue(mData->getNormal());
  mInternalUpdate = false;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DistanceMetricWrapper::DistanceMetricWrapper(DistanceMetricPtr data) : mData(data)
{
  mInternalUpdate = false;
  connect(mData.get(), SIGNAL(transformChanged()), this, SLOT(dataChangedSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(dataChangedSlot()));
}

void DistanceMetricWrapper::getPointMetrics(QStringList* uid, std::map<QString,QString>* namemap)
{
  std::map<QString, ssc::DataPtr> data = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=data.begin(); iter!=data.end(); ++iter)
  {
    if (boost::shared_dynamic_cast<PointMetric>(iter->second))
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
  this->getPointMetrics(&range, &names);

  mPSelector.resize(2);
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    mPSelector[i] = ssc::StringDataAdapterXml::initialize(QString("p%1").arg(i),
        QString("p%1").arg(i),
        QString("line endpoint %1").arg(i),
        mData->getPoint(i) ? mData->getPoint(i)->getUid() : "",
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
  return ((mData->getPoint(0)) ? mData->getPoint(0)->getName() : QString("*"))
      + QString("-")
      + ((mData->getPoint(1)) ? mData->getPoint(1)->getName() : QString("*"));
}


void DistanceMetricWrapper::pointSelected()
{
  if (mInternalUpdate)
    return;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    PointMetricPtr p = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(mPSelector[i]->getValue()));
    mData->setPoint(i, p);
  }
}

void DistanceMetricWrapper::dataChangedSlot()
{
  mInternalUpdate = true;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    if (mData->getPoint(i))
      mPSelector[i]->setValue(mData->getPoint(i)->getUid());
  }
  mInternalUpdate = false;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


AngleMetricWrapper::AngleMetricWrapper(AngleMetricPtr data) : mData(data)
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
    if (boost::shared_dynamic_cast<PointMetric>(iter->second))
    {
      *uid << iter->first;
      (*namemap)[iter->first] = iter->second->getName();
    }
  }
}

QWidget* AngleMetricWrapper::createWidget()
{
//  std::cout << "AngleMetricWrapper::createWidget() begin"  << std::endl;

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
        mData->getPoint(i) ? mData->getPoint(i)->getUid() : "",
        range,
        QDomNode());
    mPSelector[i]->setDisplayNames(names);
    hLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mPSelector[i]));
    connect(mPSelector[i].get(), SIGNAL(valueWasSet()), this, SLOT(pointSelected()));
  }

//  std::cout << "AngleMetricWrapper::createWidget() prechange"  << std::endl;
  this->dataChangedSlot();
//  std::cout << "AngleMetricWrapper::createWidget() end"  << std::endl;
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
  return "";
//  return ((mData->getPoint(0)) ? mData->getPoint(0)->getName() : QString("*"))
//      + QString("-")
//      + ((mData->getPoint(1)) ? mData->getPoint(1)->getName() : QString("*"));
}


void AngleMetricWrapper::pointSelected()
{
  if (mInternalUpdate)
    return;
//  std::cout << "AngleMetricWrapper::pointSelected()"<< std::endl;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    PointMetricPtr p = boost::shared_dynamic_cast<PointMetric>(ssc::dataManager()->getData(mPSelector[i]->getValue()));
//    std::cout << "    " << i << ", set: " << (p?p->getUid():"NULL") << ", old: "<< (mData->getPoint(i)?mData->getPoint(i)->getUid():"NULL") << std::endl;
    mData->setPoint(i, p);
  }
}

void AngleMetricWrapper::dataChangedSlot()
{
  mInternalUpdate = true;
  for (unsigned i=0; i<mPSelector.size(); ++i)
  {
    if (mData->getPoint(i))
      mPSelector[i]->setValue(mData->getPoint(i)->getUid());
  }
  mInternalUpdate = false;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MetricWidget::MetricWidget(QWidget* parent) :
  BaseWidget(parent, "MetricWidget", "Metrics/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mActiveLandmark(""),
//  mAddPointButton(new QPushButton("New Pt", this)),
//  mAddDistButton(new QPushButton("New Dist", this)),
  mRemoveButton(new QPushButton("Remove", this)),
  mLoadReferencePointsButton(new QPushButton("Load reference points", this))
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(updateSlot()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(mTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));

  this->setLayout(mVerticalLayout);

  mEditWidgets = new QStackedWidget;

  mRemoveButton->setDisabled(true);
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeButtonClickedSlot()));
  connect(mLoadReferencePointsButton, SIGNAL(clicked()), this, SLOT(loadReferencePointsSlot()));

  //layout
  mVerticalLayout->addWidget(mTable, 1);
  mVerticalLayout->addWidget(mEditWidgets, 0);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mVerticalLayout->addLayout(buttonLayout);

  this->createAction(buttonLayout, "", "New Pt", "Create a new Point Metric",      SLOT(addPointButtonClickedSlot()));
  this->createAction(buttonLayout, "", "New Dist", "Create a new Distance Metric", SLOT(addDistanceButtonClickedSlot()));
  this->createAction(buttonLayout, "", "New Angle", "Create a new Angle Metric",   SLOT(addAngleButtonClickedSlot()));
  this->createAction(buttonLayout, "", "New Plane", "Create a new Plane Metric",   SLOT(addPlaneButtonClickedSlot()));

  buttonLayout->addWidget(mRemoveButton);
  mVerticalLayout->addWidget(mLoadReferencePointsButton);
}

MetricWidget::~MetricWidget()
{}

template<class T>
QAction* MetricWidget::createAction(QLayout* layout, QString iconName, QString text, QString tip, T slot)
{
  QAction* action = new QAction(QIcon(iconName), text, this);
  action->setStatusTip(tip);
  action->setToolTip(tip);
  connect(action, SIGNAL(triggered()), this, slot);
  QToolButton* button = new QToolButton();
  button->setDefaultAction(action);
  layout->addWidget(button);
  return action;
}

QString MetricWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Utility for sampling points in 3D</h3>"
      "<p>Lets you sample points in 3D and get the distance between sampled points.</p>"
      "<p><i></i></p>"
      "</html>";
}

void MetricWidget::cellChangedSlot(int row, int col)
{
  if (col==0) // data name changed
  {
    QTableWidgetItem* item = mTable->item(row,col);
    ssc::DataPtr data = ssc::dataManager()->getData(item->data(Qt::UserRole).toString());
    if (data)
      data->setName(item->text());
  }
}


//
//void MetricWidget::testSlot()
//{
//	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
////	PointMetricPtr p0(new PointMetric("point%1","point%1"));
////	p0->setFrame(ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
////	p0->setCoordinate(ssc::Vector3D(0,0,0));
////	ssc::dataManager()->loadData(p0);
//
//	PointMetricPtr p1 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csTOOL, "ManualTool"));
////	PointMetricPtr p1(new PointMetric("point%1","point%1"));
////	p1->setFrame(ssc::CoordinateSystem(ssc::csTOOL, "ManualTool"));
////	p1->setCoordinate(ssc::Vector3D(0,0,0));
////	ssc::dataManager()->loadData(p1);
//
//	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
//	d0->setPoint(0, p0);
//	d0->setPoint(1, p1);
//	ssc::dataManager()->loadData(d0);
//
//	d0->getDistance();
//}


void MetricWidget::itemSelectionChanged()
{
  QTableWidgetItem* item = mTable->currentItem();

  mActiveLandmark = item->data(Qt::UserRole).toString();
  mEditWidgets->setCurrentIndex(mTable->currentRow());
  enablebuttons();
}


void MetricWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);

  ViewGroupDataPtr data = viewManager()->getViewGroups()[0]->getData();
  ViewGroupData::Options options = data->getOptions();
  options.mShowPointPickerProbe = true;
  data->setOptions(options);

  this->updateSlot();
}

void MetricWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

MetricBasePtr MetricWidget::createMetricWrapper(ssc::DataPtr data)
{
  if (boost::shared_dynamic_cast<PointMetric>(data))
  {
    return MetricBasePtr(new PointMetricWrapper(boost::shared_dynamic_cast<PointMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<DistanceMetric>(data))
  {
    return MetricBasePtr(new DistanceMetricWrapper(boost::shared_dynamic_cast<DistanceMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<PlaneMetric>(data))
  {
    return MetricBasePtr(new PlaneMetricWrapper(boost::shared_dynamic_cast<PlaneMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<AngleMetric>(data))
  {
    return MetricBasePtr(new AngleMetricWrapper(boost::shared_dynamic_cast<AngleMetric>(data)));
  }

	return MetricBasePtr();
}

/** create new metric wrappers for all metrics in PaSM
 *
 */
std::vector<MetricBasePtr> MetricWidget::createMetricWrappers()
{
	std::vector<MetricBasePtr> retval;
  std::map<QString, ssc::DataPtr> all = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
  {
  	MetricBasePtr wrapper = this->createMetricWrapper(iter->second);
  	if (wrapper)
  	{
  		retval.push_back(wrapper);
  	}
  }
  return retval;
}

/**update contents of table.
 * rebuild table only if necessary
 *
 */
void MetricWidget::updateSlot()
{
//  std::cout << "update " << std::endl;

  mTable->blockSignals(true);

  std::vector<MetricBasePtr> newMetrics = this->createMetricWrappers();

  bool rebuild = newMetrics.size()!=mMetrics.size();

  // check if we need to rebuild the table from scratch:
  if (!rebuild)
  {
  	for (unsigned i=0; i<mMetrics.size(); ++i)
  	{
  		rebuild = rebuild || mMetrics[i]->getData()!=newMetrics[i]->getData();
  	}
  }

  // rebuild all:
  if (rebuild)
  {
//    std::cout << "rebuild " << newMetrics.size() << std::endl;
    mTable->clear();

    while (mEditWidgets->count())
    {
    	mEditWidgets->removeWidget(mEditWidgets->widget(0));
    }

    for (unsigned i=0; i<mMetrics.size(); ++i)
    {
    	disconnect(mMetrics[i]->getData().get(), SIGNAL(transformChanged()), this, SLOT(updateSlot()));
    }

    mMetrics = newMetrics;

    for (unsigned i=0; i<mMetrics.size(); ++i)
    {
    	MetricBasePtr wrapper = mMetrics[i];
  		connect(wrapper->getData().get(), SIGNAL(transformChanged()), this, SLOT(updateSlot()));
//  		mEditWidgets->addWidget(wrapper->createWidget());

  		QGroupBox* groupBox = new QGroupBox(wrapper->getData()->getName(), this);
  		groupBox->setFlat(true);
//  	  QFrame* groupBox = new QFrame(this);
  	  QVBoxLayout* gbLayout = new QVBoxLayout(groupBox);
//  	  groupBox->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
//  	  groupBox->setLineWidth(3);
  	  gbLayout->setMargin(4);
  	  gbLayout->addWidget(wrapper->createWidget());
  	  mEditWidgets->addWidget(groupBox);
    }

    mEditWidgets->setCurrentIndex(-1);

    //ready the table widget
    mTable->setRowCount(mMetrics.size());
    mTable->setColumnCount(4);
    QStringList headerItems(QStringList() << "Name" << "Value" << "Arguments" << "Type");
    mTable->setHorizontalHeaderLabels(headerItems);
    mTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    mTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mTable->verticalHeader()->hide();

    for (unsigned i = 0; i < mMetrics.size(); ++i)
    {
    	MetricBasePtr current = mMetrics[i];

      for (unsigned j = 0; j < 4; ++j)
      {
      	QTableWidgetItem* item = new QTableWidgetItem("empty");
        item->setData(Qt::UserRole, current->getData()->getUid());
        mTable->setItem(i, j, item);
//        std::cout << "set item " << i << " " << j << std::endl;
      }
    }
//    std::cout << "rebuild end"  << std::endl;
  }

  // update contents:
  for (unsigned i = 0; i < mMetrics.size(); ++i)
  {
  	MetricBasePtr current = mMetrics[i];
    if (!mTable->item(i,0))
    {
      std::cout << "no qitem for:: " << i << " " << current->getData()->getName() << std::endl;
      continue;
    }
  	mTable->item(i,0)->setText(current->getData()->getName());
    mTable->item(i,1)->setText(current->getValue());
    mTable->item(i,2)->setText(current->getArguments());
    mTable->item(i,3)->setText(current->getType());

    //highlight selected row
    if (current->getData()->getUid() == mActiveLandmark)
    {
      mTable->setCurrentCell(i,1);
      mEditWidgets->setCurrentIndex(i);
    }
  }

  mTable->blockSignals(false);

  this->enablebuttons();
}

void MetricWidget::enablebuttons()
{
  mRemoveButton->setEnabled(mActiveLandmark!="");
  mLoadReferencePointsButton->setEnabled(ssc::toolManager()->getReferenceTool());
}

PointMetricPtr MetricWidget::addPoint(ssc::Vector3D point, ssc::CoordinateSystem frame)
{
	PointMetricPtr p1(new PointMetric("point%1","point%1"));
	p1->setFrame(frame);
	p1->setCoordinate(point);
	ssc::dataManager()->loadData(p1);

	viewManager()->getViewGroups()[0]->getData()->addData(p1);

	return p1;
}

void MetricWidget::addPointButtonClickedSlot()
{
  ssc::CoordinateSystem ref = ssc::SpaceHelpers::getR();
  ssc::Vector3D p_ref = ssc::SpaceHelpers::getDominantToolTipPoint(ref, true);

	this->addPoint(p_ref, ref);
}

void MetricWidget::addPlaneButtonClickedSlot()
{
  ssc::CoordinateSystem ref = ssc::SpaceHelpers::getR();
  ssc::Vector3D p_ref = ssc::SpaceHelpers::getDominantToolTipPoint(ref, true);

  PlaneMetricPtr p1(new PlaneMetric("plane%1","plane%1"));
  p1->setFrame(ref);
  p1->setCoordinate(p_ref);
  p1->setNormal(ssc::Vector3D(1,0,0));
  ssc::dataManager()->loadData(p1);

  viewManager()->getViewGroups()[0]->getData()->addData(p1);
}

void MetricWidget::addDistanceButtonClickedSlot()
{
	// first try to reuse existing points as distance arguments, otherwise create new ones.
  std::vector<PointMetricPtr> points;

  for (unsigned i=0; i<mMetrics.size(); ++i)
  	if (mMetrics[i]->getType()=="point")
  		points.push_back(boost::shared_dynamic_cast<PointMetric>(mMetrics[i]->getData()));

  while (points.size()>2)
  	points.erase(points.begin());

  while (points.size()<2)
  {
  	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csREF, ""));
  	points.push_back(p0);
  }

	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
	d0->setPoint(0, points[0]);
	d0->setPoint(1, points[1]);
	ssc::dataManager()->loadData(d0);

	viewManager()->getViewGroups()[0]->getData()->addData(d0);
}

void MetricWidget::addAngleButtonClickedSlot()
{
  // first try to reuse existing points as distance arguments, otherwise create new ones.
  std::vector<PointMetricPtr> points;

  for (unsigned i=0; i<mMetrics.size(); ++i)
    if (mMetrics[i]->getType()=="point")
      points.push_back(boost::shared_dynamic_cast<PointMetric>(mMetrics[i]->getData()));

  while (points.size()>4)
    points.erase(points.begin());

  while (points.size()<4)
  {
    PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csREF, ""));
    points.push_back(p0);
  }

  AngleMetricPtr d0(new AngleMetric("angle%1","angle%1"));
  for (unsigned i=0; i<points.size(); ++i)
    d0->setPoint(i, points[i]);
  ssc::dataManager()->loadData(d0);

  viewManager()->getViewGroups()[0]->getData()->addData(d0);
}

ssc::Vector3D MetricWidget::getSample() const
{
  ssc::CoordinateSystem ref = ssc::CoordinateSystemHelpers::getR();
  ssc::Vector3D P_ref = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(ref, true);
  return P_ref;
}

void MetricWidget::removeButtonClickedSlot()
{
	ssc::dataManager()->removeData(mActiveLandmark);
}

void MetricWidget::loadReferencePointsSlot()
{
  ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
    ssc::messageManager()->sendDebug("No reference tool, cannot load reference points into the pointsampler");
    return;
  }

  std::map<int, ssc::Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
    ssc::messageManager()->sendWarning("No referenceppoints in reference tool "+refTool->getName());
    return;
  }

  ssc::CoordinateSystem ref = ssc::CoordinateSystemHelpers::getR();
  ssc::CoordinateSystem sensor = ssc::CoordinateSystemHelpers::getS(refTool);

  std::map<int, ssc::Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
    ssc::Vector3D P_ref = ssc::CoordinateSystemHelpers::get_toMfrom(sensor, ref).coord(it->second);
    this->addPoint(P_ref);
  }
}

}//end namespace cx
