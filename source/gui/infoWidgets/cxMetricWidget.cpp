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
#include "sscManualTool.h"
#include "cxFrameMetricWrapper.h"
#include "cxToolMetricWrapper.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscTime.h"

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
  connect(toolManager(), SIGNAL(configured()), this, SLOT(setModified()));
  connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(setModified()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(mTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));
  connect(mTable, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));

  this->setLayout(mVerticalLayout);

  mEditWidgets = new QStackedWidget;

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  QActionGroup* group = new QActionGroup(this);
  mPointMetricAction = this->createAction(group, ":/icons/metric_point.png", "Pt", "Create a new Point Metric", SLOT(addPointButtonClickedSlot()));
  mFrameMetricAction = this->createAction(group, ":/icons/metric_frame.png", "Frame", "Create a new Frame Metric (position and orientation)", SLOT(addFrameButtonClickedSlot()));
  mToolMetricAction = this->createAction(group, ":/icons/metric_tool.png", "Tool", "Create a new Tool Metric", SLOT(addToolButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_distance.png", "Dist", "Create a new Distance Metric", SLOT(addDistanceButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_angle.png", "Angle", "Create a new Angle Metric",   SLOT(addAngleButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_plane.png", "Plane", "Create a new Plane Metric",   SLOT(addPlaneButtonClickedSlot()));

  this->createAction(group, ":/icons/metric_point.png", "Sphere", "Create a new SphereMetric",   SLOT(addSphereButtonClickedSlot()));
  this->createAction(group, ":/icons/metric_point.png", "Donut", "Create a new Donut Metric",   SLOT(addDonutButtonClickedSlot()));

  this->createAction(group, "", "", "", NULL)->setSeparator(true);
  mRemoveAction = this->createAction(group, ":/icons/metric_remove.png", "Remove", "Remove currently selected metric",   SLOT(removeButtonClickedSlot()));
  mRemoveAction->setDisabled(true);
  mLoadReferencePointsAction = this->createAction(group, ":/icons/metric_reference.png", "Import", "Import reference points from reference tool", SLOT(loadReferencePointsSlot()));
  mLoadReferencePointsAction->setDisabled(true);
  this->createAction(group, "", "", "", NULL)->setSeparator(true);
  mExportFramesAction = this->createAction(group, ":/icons/save.png", "ExportFrames", "Export metrics to file",   SLOT(exportMetricsButtonClickedSlot()));

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
    DataPtr data = dataManager()->getData(item->data(Qt::UserRole).toString());
    if (data)
      data->setName(item->text());
  }

}

void MetricWidget::cellClickedSlot(int row, int column)
{
	if (row < 0 || column < 0)
		return;

	  QTableWidgetItem* item = mTable->item(row,column);
	  DataPtr data = dataManager()->getData(item->data(Qt::UserRole).toString());
	  DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
	  if (!metric)
		  return;
	  Vector3D p_r = metric->getRefCoord();;
	  this->setManualToolPosition(p_r);
}


void MetricWidget::setManualToolPosition(Vector3D p_r)
{
	Transform3D rMpr = *toolManager()->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ManualToolPtr tool = cxToolManager::getInstance()->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	dataManager()->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
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

MetricBasePtr MetricWidget::createMetricWrapper(DataPtr data)
{
  if (boost::dynamic_pointer_cast<PointMetric>(data))
  {
    return MetricBasePtr(new PointMetricWrapper(boost::dynamic_pointer_cast<PointMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<DistanceMetric>(data))
  {
    return MetricBasePtr(new DistanceMetricWrapper(boost::dynamic_pointer_cast<DistanceMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<PlaneMetric>(data))
  {
    return MetricBasePtr(new PlaneMetricWrapper(boost::dynamic_pointer_cast<PlaneMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<AngleMetric>(data))
  {
    return MetricBasePtr(new AngleMetricWrapper(boost::dynamic_pointer_cast<AngleMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<cx::FrameMetric>(data))
  {
    return MetricBasePtr(new FrameMetricWrapper(boost::dynamic_pointer_cast<cx::FrameMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<cx::ToolMetric>(data))
  {
	return MetricBasePtr(new ToolMetricWrapper(boost::dynamic_pointer_cast<cx::ToolMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<DonutMetric>(data))
  {
	return MetricBasePtr(new DonutMetricWrapper(boost::dynamic_pointer_cast<DonutMetric>(data)));
  }
  else if (boost::dynamic_pointer_cast<SphereMetric>(data))
  {
	return MetricBasePtr(new SphereMetricWrapper(boost::dynamic_pointer_cast<SphereMetric>(data)));
  }

	return MetricBasePtr();
}

/** create new metric wrappers for all metrics in PaSM
 *
 */
std::vector<MetricBasePtr> MetricWidget::createMetricWrappers()
{
	std::vector<MetricBasePtr> retval;
  std::map<QString, DataPtr> all = dataManager()->getData();
  for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
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
//    std::cout << "rebuild " << newMetrics.size() << std::endl;
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
  mLoadReferencePointsAction->setEnabled(toolManager()->getReferenceTool());
}

PointMetricPtr MetricWidget::addPoint(Vector3D point, CoordinateSystem space, QString name)
{
	PointMetricPtr p1(new PointMetric("point%1","point%1"));
  p1->get_rMd_History()->setParentSpace("reference");
	p1->setSpace(space);
	p1->setCoordinate(point);
	dataManager()->loadData(p1);

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
	this->addPointInDefaultPosition();
}

PointMetricPtr MetricWidget::addPointInDefaultPosition()
{
	CoordinateSystem ref = SpaceHelpers::getR();
	Vector3D p_ref = SpaceHelpers::getDominantToolTipPoint(ref, true);
	return this->addPoint(p_ref, ref);
}

void MetricWidget::addFrameButtonClickedSlot()
{
  FrameMetricPtr frame(new FrameMetric("frame%1", "frame%1"));
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = SpaceHelpers::getR();
  Transform3D rMt = SpaceHelpers::getDominantToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);

  this->installNewMetric(frame);
}

void MetricWidget::addToolButtonClickedSlot()
{
  ToolMetricPtr frame(new ToolMetric("tool%1", "tool%1"));
  frame->get_rMd_History()->setParentSpace("reference");

  CoordinateSystem ref = SpaceHelpers::getR();
  Transform3D rMt = SpaceHelpers::getDominantToolTipTransform(ref, true);

  frame->setSpace(ref);
  frame->setFrame(rMt);
  frame->setToolName(toolManager()->getDominantTool()->getName());
  frame->setToolOffset(toolManager()->getDominantTool()->getTooltipOffset());

  this->installNewMetric(frame);
}

void MetricWidget::addPlaneButtonClickedSlot()
{
  CoordinateSystem ref = SpaceHelpers::getR();
//  Vector3D p_ref = SpaceHelpers::getDominantToolTipPoint(ref, true);

  PlaneMetricPtr p1(new PlaneMetric("plane%1","plane%1"));
  p1->get_rMd_History()->setParentSpace("reference");
  p1->setSpace(ref);

  ToolPtr tool = toolManager()->getDominantTool();
  if (!tool)
  {
	  p1->setCoordinate(Vector3D(0,0,0));
	  p1->setNormal(Vector3D(1,0,0));
  }
  else
  {
	  CoordinateSystem from(csTOOL_OFFSET, tool->getUid());
	  Vector3D point_t = Vector3D(0,0,0);
	  Transform3D rMto = CoordinateSystemHelpers::get_toMfrom(from, ref);

	  p1->setCoordinate(rMto.coord(Vector3D(0,0,0)));
	  p1->setNormal(rMto.vector(Vector3D(0,0,1)));
  }

  this->installNewMetric(p1);
}

/**Starting with a selection of allowed arguments for a new metric,
 * refine them by removing nonselected items, and adding more point
 * metrics if there are too few arguments.
 */
std::vector<DataPtr> MetricWidget::refinePointArguments(std::vector<DataPtr> args, unsigned argNo)
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
	  PointMetricPtr p0 = this->addPointInDefaultPosition();
	  args.push_back(p0);
  }

  return args;
}

void MetricWidget::addDistanceButtonClickedSlot()
{
	DistanceMetricPtr d0(new DistanceMetric("distance%1","distance%1"));
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
  for (unsigned i=0; i<args.size(); ++i)
	d0->getArguments()->set(i, args[i]);

  this->installNewMetric(d0);
}

void MetricWidget::addAngleButtonClickedSlot()
{
	AngleMetricPtr d0(new AngleMetric("angle%1","angle%1"));
  d0->get_rMd_History()->setParentSpace("reference");

  std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments(), 3);
  d0->getArguments()->set(0, args[0]);
  d0->getArguments()->set(1, args[1]);
  d0->getArguments()->set(2, args[1]);
  d0->getArguments()->set(3, args[2]);

  this->installNewMetric(d0);
}

std::vector<DataPtr> MetricWidget::getSpecifiedNumberOfValidArguments(MetricReferenceArgumentListPtr arguments, int numberOfRequiredArguments)
{
	// first try to reuse existing points as distance arguments, otherwise create new ones.

	std::vector<DataPtr> args;
	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
	  if (arguments->validArgument(mMetrics[i]->getData()))
		  args.push_back(mMetrics[i]->getData());
	}

	if (numberOfRequiredArguments<0)
		numberOfRequiredArguments = arguments->getCount();
	args = this->refinePointArguments(args, numberOfRequiredArguments);

	return args;
}

void MetricWidget::addSphereButtonClickedSlot()
{
	SphereMetricPtr d0(new SphereMetric("sphere%1","sphere%1"));
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);

	this->installNewMetric(d0);
}

void MetricWidget::addDonutButtonClickedSlot()
{
	DonutMetricPtr d0(new DonutMetric("donut%1","donut%1"));
	d0->get_rMd_History()->setParentSpace("reference");
	std::vector<DataPtr> args = this->getSpecifiedNumberOfValidArguments(d0->getArguments());
	d0->getArguments()->set(0, args[0]);
	d0->getArguments()->set(1, args[1]);

	this->installNewMetric(d0);
}

void MetricWidget::installNewMetric(DataMetricPtr metric)
{
	dataManager()->loadData(metric);
	this->setActiveUid(metric->getUid());
	viewManager()->getViewGroups()[0]->getData()->addData(metric);
}

void MetricWidget::removeButtonClickedSlot()
{
	int nextIndex = mTable->currentRow() + 1;
	QString nextUid;
	if (nextIndex < mTable->rowCount())
	{
		QTableWidgetItem* nextItem = mTable->item(nextIndex, 0);
		nextUid = nextItem->data(Qt::UserRole).toString();
	}

	patientService()->getPatientData()->removeData(mActiveLandmark);

	if (!nextUid.isEmpty())
		this->setActiveUid(nextUid);
}

void MetricWidget::loadReferencePointsSlot()
{
  ToolPtr refTool = toolManager()->getReferenceTool();
  if(!refTool) // we only load reference points from reference tools
  {
    messageManager()->sendDebug("No reference tool, cannot load reference points into the pointsampler");
    return;
  }

  std::map<int, Vector3D> referencePoints_s = refTool->getReferencePoints();
  if(referencePoints_s.empty())
  {
    messageManager()->sendWarning("No referenceppoints in reference tool "+refTool->getName());
    return;
  }

  CoordinateSystem ref = CoordinateSystemHelpers::getR();
  CoordinateSystem sensor = CoordinateSystemHelpers::getS(refTool);

  std::map<int, Vector3D>::iterator it = referencePoints_s.begin();
  for(; it != referencePoints_s.end(); ++it)
  {
    Vector3D P_ref = CoordinateSystemHelpers::get_toMfrom(sensor, ref).coord(it->second);
    this->addPoint(P_ref, CoordinateSystem(csREF), "ref%1");
  }
}

void MetricWidget::exportMetricsButtonClickedSlot()
{
	QString suggestion = QString("%1/Logs/metrics_%2.txt")
			.arg(patientService()->getPatientData()->getActivePatientFolder())
			.arg(QDateTime::currentDateTime().toString(timestampSecondsFormat()));

	QString filename = QFileDialog::getSaveFileName(this,
													"Create/select file to export metrics to",
													suggestion);
	if(!filename.isEmpty())
		this->exportMetricsToFile(filename);
}

void MetricWidget::exportMetricsToFile(QString filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;	

	std::map<QString, DataPtr> dataMap = dataManager()->getData();
	std::map<QString, DataPtr>::iterator iter;
	for (iter = dataMap.begin(); iter != dataMap.end(); ++iter)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(iter->second);
		if(metric)
		{
			file.write(metric->getAsSingleLineString().toAscii());
			file.write("\n");
		}
	}
	file.close();
}


}//end namespace cx
