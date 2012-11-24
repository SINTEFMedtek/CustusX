/*
 * cxMetricWidget.cpp
 *
 *  \date Jul 5, 2011
 *      \author christiana
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
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxVector3DWidget.h"
#include "sscRegistrationTransform.h"
#include "sscTimeKeeper.h"

namespace cx
{


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MetricWidget::MetricWidget(QWidget* parent) :
  BaseWidget(parent, "MetricWidget", "Metrics/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this)),
  mActiveLandmark("")
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(setModified()));
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(setModified()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(mTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));
  connect(mTable, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));

  this->setLayout(mVerticalLayout);

  mEditWidgets = new QStackedWidget;

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  QActionGroup* group = new QActionGroup(this);
  this->createAction(group, ":/icons/metric_point.png", "Pt", "Create a new Point Metric",      SLOT(addPointButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_distance.png", "Dist", "Create a new Distance Metric", SLOT(addDistanceButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_angle.png", "Angle", "Create a new Angle Metric",   SLOT(addAngleButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_plane.png", "Plane", "Create a new Plane Metric",   SLOT(addPlaneButtonClickedSlot()));
  this->createAction(group, "", "", "", NULL)->setSeparator(true);
  mRemoveAction = this->createAction(group, ":/icons/metric_remove.png", "Remove", "Remove currently selected metric",   SLOT(removeButtonClickedSlot()));
  mRemoveAction->setDisabled(true);
  mLoadReferencePointsAction = this->createAction(group, ":/icons/metric_reference.png", "Import", "Import reference points from reference tool", SLOT(loadReferencePointsSlot()));
  mLoadReferencePointsAction->setDisabled(true);

  QToolBar* toolBar = new QToolBar("actions", this);
  toolBar->addActions(group->actions());
  buttonLayout->addWidget(toolBar);
  buttonLayout->addStretch();

  //layout
  mVerticalLayout->addLayout(buttonLayout);
  mVerticalLayout->addWidget(mTable, 1);
  mVerticalLayout->addWidget(mEditWidgets, 0);
}

MetricWidget::~MetricWidget()
{}

//template<class T>
QAction* MetricWidget::createAction(QActionGroup* group, QString iconName, QString text, QString tip, const char* slot)
{
  QAction* action = new QAction(QIcon(iconName), text, group);
  action->setStatusTip(tip);
  action->setToolTip(tip);
  if (slot)
  {
  	connect(action, SIGNAL(triggered()), this, slot);
  }
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

void MetricWidget::cellClickedSlot(int row, int column)
{
	if (row < 0 || column < 0)
		return;

	  QTableWidgetItem* item = mTable->item(row,column);
	  ssc::DataPtr data = ssc::dataManager()->getData(item->data(Qt::UserRole).toString());
	  ssc::PointMetricPtr pointData = boost::shared_dynamic_cast<ssc::PointMetric>(data);
	  if (pointData)
	  {
		  	ssc::Vector3D p_r = pointData->getRefCoord();;
		  	ssc::Vector3D p_pr = ssc::toolManager()->get_rMpr()->coord(p_r);;
		  	this->setManualToolPosition(p_r);
	  }
}


void MetricWidget::setManualToolPosition(ssc::Vector3D p_r)
{
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
	ssc::Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();
	ssc::Vector3D offset = tool->get_prMt().vector(ssc::Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	ssc::dataManager()->setCenter(p_r);
	ssc::Vector3D p0_pr = tool->get_prMt().coord(ssc::Vector3D(0, 0, 0));
	tool->set_prMt(ssc::createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

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

// Turned off by request (#420) - too confusing
//  ViewGroupDataPtr data = viewManager()->getViewGroups()[0]->getData();
//  ViewGroupData::Options options = data->getOptions();
//  options.mShowPointPickerProbe = true;
//  data->setOptions(options);

  this->setModified();
}

void MetricWidget::hideEvent(QHideEvent* event)
{
  QWidget::hideEvent(event);
}

MetricBasePtr MetricWidget::createMetricWrapper(ssc::DataPtr data)
{
  if (boost::shared_dynamic_cast<ssc::PointMetric>(data))
  {
    return MetricBasePtr(new PointMetricWrapper(boost::shared_dynamic_cast<ssc::PointMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<ssc::DistanceMetric>(data))
  {
    return MetricBasePtr(new DistanceMetricWrapper(boost::shared_dynamic_cast<ssc::DistanceMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<ssc::PlaneMetric>(data))
  {
    return MetricBasePtr(new PlaneMetricWrapper(boost::shared_dynamic_cast<ssc::PlaneMetric>(data)));
  }
  else if (boost::shared_dynamic_cast<ssc::AngleMetric>(data))
  {
    return MetricBasePtr(new AngleMetricWrapper(boost::shared_dynamic_cast<ssc::AngleMetric>(data)));
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

void MetricWidget::prePaintEvent()
{
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
    std::cout << "rebuild " << newMetrics.size() << std::endl;
    mTable->clear();

    while (mEditWidgets->count())
    {
    	mEditWidgets->removeWidget(mEditWidgets->widget(0));
    }

    for (unsigned i=0; i<mMetrics.size(); ++i)
    {
        disconnect(mMetrics[i]->getData().get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
    }

    mMetrics = newMetrics;

    for (unsigned i=0; i<mMetrics.size(); ++i)
    {
    	MetricBasePtr wrapper = mMetrics[i];
        connect(wrapper->getData().get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
  		QGroupBox* groupBox = new QGroupBox(wrapper->getData()->getName(), this);
  		groupBox->setFlat(true);
  	  QVBoxLayout* gbLayout = new QVBoxLayout(groupBox);
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
//        item->setFlags(item->flags() & (~Qt::ItemFlags(Qt::ItemIsEditable))); // turn off editable (also turned off ability to select+copy text - removed)
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
  mRemoveAction->setEnabled(mActiveLandmark!="");
  mLoadReferencePointsAction->setEnabled(ssc::toolManager()->getReferenceTool());
}

ssc::PointMetricPtr MetricWidget::addPoint(ssc::Vector3D point, ssc::CoordinateSystem space, QString name)
{
	ssc::PointMetricPtr p1(new ssc::PointMetric("point%1","point%1"));
  p1->get_rMd_History()->setParentSpace("reference");
	p1->setSpace(space);
	p1->setCoordinate(point);
	ssc::dataManager()->loadData(p1);

	viewManager()->getViewGroups()[0]->getData()->addData(p1);
	this->setActiveUid(p1->getUid());

	return p1;
}

void MetricWidget::setActiveUid(QString uid)
{
	mActiveLandmark = uid;
    this->setModified();
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
//  ssc::Vector3D p_ref = ssc::SpaceHelpers::getDominantToolTipPoint(ref, true);

  ssc::PlaneMetricPtr p1(new ssc::PlaneMetric("plane%1","plane%1"));
  p1->get_rMd_History()->setParentSpace("reference");
  p1->setSpace(ref);

  ssc::ToolPtr tool = ssc::toolManager()->getDominantTool();
  if (!tool)
  {
	  p1->setCoordinate(ssc::Vector3D(0,0,0));
	  p1->setNormal(ssc::Vector3D(1,0,0));
  }
  else
  {
	  ssc::CoordinateSystem from(ssc::csTOOL_OFFSET, tool->getUid());
	  ssc::Vector3D point_t = ssc::Vector3D(0,0,0);
	  ssc::Transform3D rMto = ssc::CoordinateSystemHelpers::get_toMfrom(from, ref);

	  p1->setCoordinate(rMto.coord(ssc::Vector3D(0,0,0)));
	  p1->setNormal(rMto.vector(ssc::Vector3D(0,0,1)));
  }

  ssc::dataManager()->loadData(p1);
	this->setActiveUid(p1->getUid());

  viewManager()->getViewGroups()[0]->getData()->addData(p1);
}

/**Starting with a selection of allowed arguments for a new metric,
 * refine them by removing nonselected items, and adding more point
 * metrics if there are too few arguments.
 */
std::vector<ssc::DataPtr> MetricWidget::refinePointArguments(std::vector<ssc::DataPtr> args, unsigned argNo)
{
  // erase non-selected arguments if we have more than enough
  QList<QTableWidgetItem*> selection = mTable->selectedItems();
  std::set<QString> selectedUids;
  for (int i=0; i<selection.size(); ++i)
  {
  	selectedUids.insert(selection[i]->data(Qt::UserRole).toString());
  }
  for (unsigned i=0; i<args.size();)
  {
  	if (args.size() <= argNo)
  		break;
  	if (!selectedUids.count(args[i]->getUid()))
  		args.erase(args.begin()+i);
  	else
  		++i;
  }

  while (args.size() > argNo)
  	args.erase(args.begin());

  while (args.size() < argNo)
  {
	  ssc::PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csREF, ""));
  	args.push_back(p0);
  }

  return args;
}


void MetricWidget::addDistanceButtonClickedSlot()
{
	ssc::DistanceMetricPtr d0(new ssc::DistanceMetric("distance%1","distance%1"));
  d0->get_rMd_History()->setParentSpace("reference");
	// first try to reuse existing points as distance arguments, otherwise create new ones.
  std::vector<ssc::DataPtr> args;

  for (unsigned i=0; i<mMetrics.size(); ++i)
  {
  	if (d0->validArgument(mMetrics[i]->getData()))
  		args.push_back(mMetrics[i]->getData());
  }

  args = this->refinePointArguments(args, d0->getArgumentCount());

//  // erase non-selected arguments if we have more than enough
//  QList<QTableWidgetItem*> selection = mTable->selectedItems();
//  std::set<QString> selectedUids;
//  for (int i=0; i<selection.size(); ++i)
//  {
//  	selectedUids.insert(selection[i]->data(Qt::UserRole).toString());
//  }
//  for (unsigned i=0; i<args.size();)
//  {
//  	if (args.size()<=d0->getArgumentCount())
//  		break;
//  	if (!selectedUids.count(args[i]->getUid()))
//  		args.erase(args.begin()+i);
//  	else
//  		++i;
//  }
//
//  while (args.size() > d0->getArgumentCount())
//  	args.erase(args.begin());
//
//  while (args.size() < d0->getArgumentCount())
//  {
//  	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csREF, ""));
//  	args.push_back(p0);
//  }

  for (unsigned i=0; i<args.size(); ++i)
    d0->setArgument(i, args[i]);

	ssc::dataManager()->loadData(d0);

	this->setActiveUid(d0->getUid());
	viewManager()->getViewGroups()[0]->getData()->addData(d0);
}

void MetricWidget::addAngleButtonClickedSlot()
{
	ssc::AngleMetricPtr d0(new ssc::AngleMetric("angle%1","angle%1"));
  d0->get_rMd_History()->setParentSpace("reference");
	// first try to reuse existing points as distance arguments, otherwise create new ones.
  std::vector<ssc::DataPtr> args;

  for (unsigned i=0; i<mMetrics.size(); ++i)
  {
  	if (d0->validArgument(mMetrics[i]->getData()))
  		args.push_back(mMetrics[i]->getData());
  }

  args = this->refinePointArguments(args, 3);
//
//  while (args.size() > 3)
//  	args.erase(args.begin());
//
//  while (args.size() < 3)
//  {
//  	PointMetricPtr p0 = this->addPoint(ssc::Vector3D(0,0,0), ssc::CoordinateSystem(ssc::csREF, ""));
//  	args.push_back(p0);
//  }

//  for (unsigned i=0; i<args.size(); ++i)
//    d0->setArgument(i, args[i]);

  d0->setArgument(0, args[0]);
  d0->setArgument(1, args[1]);
  d0->setArgument(2, args[1]);
  d0->setArgument(3, args[2]);

  ssc::dataManager()->loadData(d0);

	this->setActiveUid(d0->getUid());
  viewManager()->getViewGroups()[0]->getData()->addData(d0);
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
//    this->addPoint(P_ref);
    this->addPoint(P_ref, ssc::CoordinateSystem(ssc::csREF), "ref%1");
  }
}

}//end namespace cx
