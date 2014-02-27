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
#include "sscDataManager.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxVector3DWidget.h"
//#include "sscRegistrationTransform.h"
#include "sscTimeKeeper.h"
#include "cxFrameMetricWrapper.h"
#include "cxToolMetricWrapper.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "sscTime.h"
#include "cxMetricManager.h"

//#include "sscManualTool.h"
//#include "sscPointMetric.h"
//#include "sscDistanceMetric.h"
#include "sscLogger.h"

namespace cx
{


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

MetricWidget::MetricWidget(QWidget* parent) :
  BaseWidget(parent, "MetricWidget", "Metrics/3D ruler"),
  mVerticalLayout(new QVBoxLayout(this)),
  mTable(new QTableWidget(this))
{
	// the delayed timer lowers the update rate of this widget,
	// as is is seen to strangle the render speed when many metrics are present.
	int lowUpdateRate = 100;
	mLocalModified = false;
	mDelayedUpdateTimer = new QTimer(this);
	connect(mDelayedUpdateTimer, SIGNAL(timeout()), this, SLOT(delayedUpdate())); // this signal will be executed in the thread of THIS, i.e. the main thread.
	mDelayedUpdateTimer->start(lowUpdateRate);


	mModifiedCount = 0;
	mPaintCount = 0;
	mMetricManager.reset(new MetricManager());
	connect(mMetricManager.get(), SIGNAL(activeMetricChanged()), this, SLOT(setModified()));
	connect(mMetricManager.get(), SIGNAL(metricsChanged()), this, SLOT(setModified()));

  //table widget
  connect(mTable, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  connect(mTable, SIGNAL(cellChanged(int, int)), this, SLOT(cellChangedSlot(int, int)));
  connect(mTable, SIGNAL(cellClicked(int, int)), this, SLOT(cellClickedSlot(int, int)));

  this->setLayout(mVerticalLayout);

  mEditWidgets = new QStackedWidget;

  QActionGroup* group = new QActionGroup(this);
  this->createActions(group);

  QToolBar* toolBar = new QToolBar("actions", this);
  toolBar->addActions(group->actions());

  QHBoxLayout* buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(toolBar);
  buttonLayout->addStretch();

  //layout
  mVerticalLayout->addLayout(buttonLayout);
  mVerticalLayout->addWidget(mTable, 1);
  mVerticalLayout->addWidget(mEditWidgets, 0);
}

MetricWidget::~MetricWidget()
{}

void MetricWidget::createActions(QActionGroup* group)
{
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
}

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
	  QString uid = item->data(Qt::UserRole).toString();
	  mMetricManager->moveToMetric(uid);
}

void MetricWidget::itemSelectionChanged()
{
  QTableWidgetItem* item = mTable->currentItem();

  mMetricManager->setActiveUid(item->data(Qt::UserRole).toString());
  mEditWidgets->setCurrentIndex(mTable->currentRow());

  mMetricManager->setSelection(this->getSelectedUids());

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

namespace
{
template<class T, class SUPER>
boost::shared_ptr<T> castTo(boost::shared_ptr<SUPER> data)
{
	return boost::dynamic_pointer_cast<T>(data);
}

template<class T, class SUPER>
bool isType(boost::shared_ptr<SUPER> data)
{
	return castTo<T>(data);
}

template<class WRAPPER, class METRIC, class SUPER>
boost::shared_ptr<WRAPPER> createMetricWrapperOfType(boost::shared_ptr<SUPER> data)
{
	return boost::shared_ptr<WRAPPER>(new WRAPPER(castTo<METRIC>(data)));
}
}

MetricBasePtr MetricWidget::createMetricWrapper(DataPtr data)
{
	if (isType<PointMetric>(data))
	  return createMetricWrapperOfType<PointMetricWrapper, PointMetric>(data);
	if (isType<DistanceMetric>(data))
	  return createMetricWrapperOfType<DistanceMetricWrapper, DistanceMetric>(data);
	if (isType<AngleMetric>(data))
	  return createMetricWrapperOfType<AngleMetricWrapper, AngleMetric>(data);
	if (isType<FrameMetric>(data))
	  return createMetricWrapperOfType<FrameMetricWrapper, FrameMetric>(data);
	if (isType<ToolMetric>(data))
	  return createMetricWrapperOfType<ToolMetricWrapper, ToolMetric>(data);
	if (isType<PlaneMetric>(data))
	  return createMetricWrapperOfType<PlaneMetricWrapper, PlaneMetric>(data);
	if (isType<DonutMetric>(data))
	  return createMetricWrapperOfType<DonutMetricWrapper, DonutMetric>(data);
	if (isType<SphereMetric>(data))
	  return createMetricWrapperOfType<SphereMetricWrapper, SphereMetric>(data);

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
//	QTime timer;
//	timer.start();
	mPaintCount++;
  std::vector<MetricBasePtr> newMetrics = this->createMetricWrappers();

  bool rebuild = !this->checkEqual(newMetrics, mMetrics);
  if (rebuild)
  {
	this->resetWrappersAndEditWidgets(newMetrics);
	this->initializeTable();
  }

  this->updateMetricWrappers();
  this->updateTableContents();

  if (rebuild)
  {
	  this->expensizeColumnResize();
  }

  this->enablebuttons();
//  std::cout << QString("prepaint, mod=%1, paint=%2, elapsed=%3ms").arg(mModifiedCount).arg(mPaintCount).arg(timer.elapsed()) << std::endl;
//  std::cout << QString("prepaint, mod=%1, paint=%2").arg(mModifiedCount).arg(mPaintCount) << std::endl;
}

void MetricWidget::expensizeColumnResize()
{
	int valueColumn = 1;
	mTable->resizeColumnToContents(valueColumn);
}

void MetricWidget::initializeTable()
{
	mTable->blockSignals(true);

	mTable->clear();

	mTable->setRowCount(mMetrics.size());
	mTable->setColumnCount(4);
	QStringList headerItems(QStringList() << "Name" << "Value" << "Arguments" << "Type");
	mTable->setHorizontalHeaderLabels(headerItems);
//	mTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents); // dangerous: uses lots of painting time
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
		}
	}
	mTable->blockSignals(false);
}

void MetricWidget::updateMetricWrappers()
{
	for (unsigned i = 0; i < mMetrics.size(); ++i)
	{
		mMetrics[i]->update();
	}
}

void MetricWidget::updateTableContents()
{
	mTable->blockSignals(true);
	// update contents:
	QTime timer;
	timer.start();
	for (unsigned i = 0; i < mMetrics.size(); ++i)
	{
		MetricBasePtr current = mMetrics[i];
		QString name = current->getData()->getName();
		QString value = current->getValue();
		QString arguments = current->getArguments();
		QString type = current->getType();
	}

	for (unsigned i = 0; i < mMetrics.size(); ++i)
	{
		MetricBasePtr current = mMetrics[i];
		if (!mTable->item(i,0))
		{
			std::cout << "no qitem for:: " << i << " " << current->getData()->getName() << std::endl;
			continue;
		}
		QString name = current->getData()->getName();
		QString value = current->getValue();
		QString arguments = current->getArguments();
		QString type = current->getType();

		mTable->item(i,0)->setText(name);
		mTable->item(i,1)->setText(value);
		mTable->item(i,2)->setText(arguments);
		mTable->item(i,3)->setText(type);

		//highlight selected row
		if (current->getData()->getUid() == mMetricManager->getActiveUid())
		{
			mTable->setCurrentCell(i,1);
			mEditWidgets->setCurrentIndex(i);
		}
	}
	mTable->blockSignals(false);
}

void MetricWidget::setModified()
{
	mLocalModified = true;
//	BaseWidget::setModified();
	mModifiedCount++;
}

void MetricWidget::delayedUpdate()
{
	if (!mLocalModified)
		return;
	BaseWidget::setModified();
	mLocalModified = false;

}

void MetricWidget::resetWrappersAndEditWidgets(std::vector<MetricBasePtr> wrappers)
{
	while (mEditWidgets->count())
	{
		mEditWidgets->removeWidget(mEditWidgets->widget(0));
	}

	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
		disconnect(mMetrics[i]->getData().get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
	}

	mMetrics = wrappers;

	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
		connect(mMetrics[i]->getData().get(), SIGNAL(transformChanged()), this, SLOT(setModified()));
	}

	for (unsigned i=0; i<mMetrics.size(); ++i)
	{
		MetricBasePtr wrapper = mMetrics[i];
		QGroupBox* groupBox = new QGroupBox(wrapper->getData()->getName(), this);
		groupBox->setFlat(true);
		QVBoxLayout* gbLayout = new QVBoxLayout(groupBox);
		gbLayout->setMargin(4);
		gbLayout->addWidget(wrapper->createWidget());
		mEditWidgets->addWidget(groupBox);
	}

	mEditWidgets->setCurrentIndex(-1);
}

bool MetricWidget::checkEqual(const std::vector<MetricBasePtr>& a, const std::vector<MetricBasePtr>& b) const
{
	if (a.size()!=b.size())
		return false;

	for (unsigned i=0; i<a.size(); ++i)
	{
		if (a[i]->getData()!=b[i]->getData())
			return false;
	}

	return true;
}

void MetricWidget::enablebuttons()
{
	mRemoveAction->setEnabled(!mMetricManager->getActiveUid().isEmpty());
  mLoadReferencePointsAction->setEnabled(toolManager()->getReferenceTool());
}

void MetricWidget::loadReferencePointsSlot()
{
	mMetricManager->loadReferencePointsSlot();
}
void MetricWidget::addPointButtonClickedSlot()
{
	mMetricManager->addPointButtonClickedSlot();
}
void MetricWidget::addFrameButtonClickedSlot()
{
	mMetricManager->addFrameButtonClickedSlot();
}
void MetricWidget::addToolButtonClickedSlot()
{
	mMetricManager->addToolButtonClickedSlot();
}
void MetricWidget::addPlaneButtonClickedSlot()
{
	mMetricManager->addPlaneButtonClickedSlot();
}
void MetricWidget::addAngleButtonClickedSlot()
{
	mMetricManager->addAngleButtonClickedSlot();
}
void MetricWidget::addDistanceButtonClickedSlot()
{
	mMetricManager->addDistanceButtonClickedSlot();
}
void MetricWidget::addSphereButtonClickedSlot()
{
	mMetricManager->addSphereButtonClickedSlot();
}
void MetricWidget::addDonutButtonClickedSlot()
{
	mMetricManager->addDonutButtonClickedSlot();
}

std::set<QString> MetricWidget::getSelectedUids()
{
	QList<QTableWidgetItem*> selection = mTable->selectedItems();

	std::set<QString> selectedUids;
	for (int i=0; i<selection.size(); ++i)
	{
	  selectedUids.insert(selection[i]->data(Qt::UserRole).toString());
	}
	return selectedUids;
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

	patientService()->getPatientData()->removeData(mMetricManager->getActiveUid());

	if (!nextUid.isEmpty())
		mMetricManager->setActiveUid(nextUid);
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
		mMetricManager->exportMetricsToFile(filename);
}



}//end namespace cx
