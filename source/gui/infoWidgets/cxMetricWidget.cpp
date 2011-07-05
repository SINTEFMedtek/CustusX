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

namespace cx
{

PointMetricWrapper::PointMetricWrapper(PointMetricPtr data) : mData(data)
{
}

QWidget* PointMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QHBoxLayout* topLayout = new QHBoxLayout(widget);

	QString value;
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
	topLayout->addWidget(new ssc::LabeledComboBoxWidget(widget, mFrameSelector));

	QPushButton* sampleButton = new QPushButton("Sample");
	sampleButton->setToolTip("Set the position equal to the current tool tip position.");
	topLayout->addWidget(sampleButton);

	return widget;
}

QString PointMetricWrapper::getValue() const
{
	ssc::Transform3D rM0 = ssc::SpaceHelpers::get_toMfrom(mData->getFrame(), ssc::CoordinateSystem(ssc::csREF));
	ssc::Vector3D p0_r = rM0.coord(mData->getCoordinate());
	return "pt_r="+qstring_cast(p0_r);
}
ssc::DataPtr PointMetricWrapper::getData() const
{
	return mData;
}
QString PointMetricWrapper::getType() const
{
	return "point";
}

void PointMetricWrapper::moveToToolPosition()
{

}

void PointMetricWrapper::frameSelected(QString id)
{

}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DistanceMetricWrapper::DistanceMetricWrapper(DistanceMetricPtr data) : mData(data)
{
}
QWidget* DistanceMetricWrapper::createWidget()
{
	QWidget* widget = new QWidget;
	QVBoxLayout* topLayout = new QVBoxLayout(widget);
	topLayout->addWidget(new QPushButton("dist"));
	return widget;
}

QString DistanceMetricWrapper::getValue() const
{
	return "d= "+qstring_cast(mData->getDistance())+" mm";
}
ssc::DataPtr DistanceMetricWrapper::getData() const
{
	return mData;
}
QString DistanceMetricWrapper::getType() const
{
	return "distance";
}



//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


MetricWidget::MetricWidget(QWidget* parent) :
  BaseWidget(parent, "MetricWidget", "Metrics/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mActiveLandmark(""),
//  mAddButton(new QPushButton("Add", this)),
  mAddPointButton(new QPushButton("New Pt", this)),
  mAddDistButton(new QPushButton("New Dist", this)),
  mEditButton(new QPushButton("Resample", this)),
  mRemoveButton(new QPushButton("Remove", this)),
  mTestButton(new QPushButton("Test", this)),
  mLoadReferencePointsButton(new QPushButton("Load reference points", this))
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(updateSlot()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(updateSlot()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

  this->setLayout(mVerticalLayout);

  connect(mAddPointButton, SIGNAL(clicked()), this, SLOT(addPointButtonClickedSlot()));
  connect(mAddDistButton, SIGNAL(clicked()), this, SLOT(addDistanceButtonClickedSlot()));

  mEditWidgets = new QStackedWidget;

  //pushbuttons
//  connect(mAddButton, SIGNAL(clicked()), this, SLOT(addButtonClickedSlot()));
  mEditButton->setDisabled(true);
  connect(mEditButton, SIGNAL(clicked()), this, SLOT(editButtonClickedSlot()));
  mRemoveButton->setDisabled(true);
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeButtonClickedSlot()));
  connect(mTestButton, SIGNAL(clicked()), this, SLOT(testSlot()));
  connect(mLoadReferencePointsButton, SIGNAL(clicked()), this, SLOT(loadReferencePointsSlot()));

  //layout
  mVerticalLayout->addWidget(mTable, 1);
  mVerticalLayout->addWidget(mEditWidgets, 0);

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  mVerticalLayout->addLayout(buttonLayout);

  buttonLayout->addWidget(mAddPointButton);
  buttonLayout->addWidget(mAddDistButton);
  buttonLayout->addWidget(mEditButton);
  buttonLayout->addWidget(mRemoveButton);
  buttonLayout->addWidget(mTestButton);
  mVerticalLayout->addWidget(mLoadReferencePointsButton);
}

MetricWidget::~MetricWidget()
{}

QString MetricWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Utility for sampling points in 3D</h3>"
      "<p>Lets you sample points in 3D and get the distance between sampled points.</p>"
      "<p><i></i></p>"
      "</html>";
}

void MetricWidget::testSlot()
{
	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
//	PointMetricPtr p0(new PointMetric("point%1","point%1"));
//	p0->setFrame(ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
//	p0->setCoordinate(ssc::Vector3D(0,0,0));
//	ssc::dataManager()->loadData(p0);

	PointMetricPtr p1 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csTOOL, "ManualTool"));
//	PointMetricPtr p1(new PointMetric("point%1","point%1"));
//	p1->setFrame(ssc::CoordinateSystem(ssc::csTOOL, "ManualTool"));
//	p1->setCoordinate(ssc::Vector3D(0,0,0));
//	ssc::dataManager()->loadData(p1);

	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
	d0->setPoint(0, p0);
	d0->setPoint(1, p1);
	ssc::dataManager()->loadData(d0);

	d0->getDistance();
}


void MetricWidget::itemSelectionChanged()
{
  //std::cout << "pling" << std::endl;

  QTableWidgetItem* item = mTable->currentItem();

  mActiveLandmark = item->data(Qt::UserRole).toString();
  mEditWidgets->setCurrentIndex(mTable->currentRow());

//  for (unsigned i=0; i<mSamples.size(); ++i)
//  {
//    if (mSamples[i].getUid()!=mActiveLandmark)
//      continue;
//    setManualTool(mSamples[i].getCoord());
//    break;
//  }

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

	return MetricBasePtr();
}

void MetricWidget::updateSlot()
{
  mTable->blockSignals(true);
  mTable->clear();

  while (mEditWidgets->count())
  {
  	mEditWidgets->removeWidget(mEditWidgets->widget(0));
  }

  for (unsigned i=0; i<mMetrics.size(); ++i)
  {
  	disconnect(mMetrics[i]->getData().get(), SIGNAL(transformChanged()), this, SLOT(updateSlot()));
  }

  mMetrics.clear();
  std::map<QString, ssc::DataPtr> all = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
  {
  	MetricBasePtr wrapper = this->createMetricWrapper(iter->second);
  	if (wrapper)
  	{
  		mMetrics.push_back(wrapper);
  		connect(wrapper->getData().get(), SIGNAL(transformChanged()), this, SLOT(updateSlot()));
  		mEditWidgets->addWidget(wrapper->createWidget());
  	}
  }

  mEditWidgets->setCurrentIndex(-1);

  //ready the table widget
  mTable->setRowCount(mMetrics.size());
  mTable->setColumnCount(4);
  QStringList headerItems(QStringList() << "Name" << "Value" << "Arguments" << "Type");
  mTable->setHorizontalHeaderLabels(headerItems);
  mTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mTable->setSelectionBehavior(QAbstractItemView::SelectRows);

  for (unsigned i = 0; i < mMetrics.size(); ++i)
  {
  	MetricBasePtr current = mMetrics[i];
    std::vector<QTableWidgetItem*> items(4); // name,val,args,type

    items[0] = new QTableWidgetItem(current->getData()->getName());
    items[1] = new QTableWidgetItem(current->getValue());
    items[2] = new QTableWidgetItem("args");
    items[3] = new QTableWidgetItem(current->getType());

    for (unsigned j = 0; j < items.size(); ++j)
    {
      items[j]->setData(Qt::UserRole, current->getData()->getUid());
      mTable->setItem(i, j, items[j]);
    }

    //highlight selected row
    if (current->getData()->getUid() == mActiveLandmark)
    {
      mTable->setCurrentItem(items[1]);
    }
  }

  mTable->blockSignals(false);

  this->enablebuttons();
}

void MetricWidget::enablebuttons()
{
//  mAddButton->setEnabled(true);
  mEditButton->setEnabled(mActiveLandmark!="");
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

void MetricWidget::setManualTool(const ssc::Vector3D& p_r)
{
  ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();

  //ssc::Transform3D sMr = mSliceProxy->get_sMr();
  ssc::Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
  ssc::Transform3D prMt = tool->get_prMt();

  // find tool position in r
  ssc::Vector3D tool_t(0,0,tool->getTooltipOffset());
  ssc::Vector3D tool_r = (rMpr*prMt).coord(tool_t);

  // find click position in s.
  //ssc::Vector3D click_s = get_vpMs().inv().coord(click_vp);

  // compute the new tool position in slice space as a synthesis of the plane part of click and the z part of original.
  //ssc::Vector3D cross_s(click_s[0], click_s[1], tool_s[2]);
  // compute the position change and transform to patient.
  ssc::Vector3D delta_r = p_r - tool_r;
  ssc::Vector3D delta_pr = rMpr.inv().vector(delta_r);

  // MD is the actual tool movement in patient space, matrix form
  ssc::Transform3D MD = ssc::createTransformTranslate(delta_pr);
  // set new tool position to old modified by MD:
  tool->set_prMt(MD*prMt);
}

void MetricWidget::addPointButtonClickedSlot()
{
	this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
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
  	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csPATIENTREF, ""));
  	points.push_back(p0);
  }

	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
	d0->setPoint(0, points[0]);
	d0->setPoint(1, points[1]);
	ssc::dataManager()->loadData(d0);

	viewManager()->getViewGroups()[0]->getData()->addData(d0);
}



ssc::Vector3D MetricWidget::getSample() const
{
// find current tool position:
//  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
//  if (!tool)
//    return ssc::Vector3D(0,0,0);
//  ssc::Transform3D prMt = tool->get_prMt();
//  ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
//  ssc::Vector3D pos = (rMpr*prMt).coord(ssc::Vector3D(0,0,tool->getTooltipOffset()));

  ssc::CoordinateSystem ref = ssc::CoordinateSystemHelpers::getR();
  ssc::Vector3D P_ref = ssc::CoordinateSystemHelpers::getDominantToolTipPoint(ref, true);

  return P_ref;
}

void MetricWidget::editButtonClickedSlot()
{
//  for (unsigned i=0; i<mSamples.size(); ++i)
//  {
//    if (mSamples[i].getUid()!=mActiveLandmark)
//      continue;
//    mSamples[i] = ssc::Landmark(mActiveLandmark, getSample());
//  }
//  updateSlot();
}

void MetricWidget::removeButtonClickedSlot()
{
	ssc::dataManager()->removeData(mActiveLandmark);
}

void MetricWidget::gotoButtonClickedSlot()
{

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
