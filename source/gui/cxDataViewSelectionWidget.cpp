/*
 * sscDataViewSelectionWidget.cpp
 *
 *  Created on: Apr 5, 2011
 *      Author: christiana
 */

#include <cxDataViewSelectionWidget.h>
#include "cxToolListWidget.h"
#include "sscDataManager.h"
#include "sscData.h"
#include <QListWidgetItem>
#include <QDir>
#include <QHBoxLayout>
#include <QDropEvent>
#include <QMimeData>
#include <QAction>
#include <QMenu>
#include "sscEnumConverter.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxToolConfigurationParser.h"
#include "sscImageAlgorithms.h"
#include "sscImage.h"
#include "cxViewManager.h"
#include "cxViewGroup.h"

namespace cx
{

DataListWidget::DataListWidget(QWidget* parent) :
    QListWidget(parent)
{
//  connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(toolClickedSlot(QListWidgetItem*)));

  this->setSelectionBehavior(QAbstractItemView::SelectItems);
  this->setSelectionMode(QAbstractItemView::SingleSelection);
}

DataListWidget::~DataListWidget()
{}

void DataListWidget::populate(QStringList dataUids)
{
  this->clear();

  foreach(QString data, dataUids)
  {
    this->populateData(data);
  }
  emit listSizeChanged();
}

void DataListWidget::populateData(QString uid, bool indent, QListWidgetItem* after)
{
  ssc::DataPtr data = ssc::dataManager()->getData(uid);
  if (!data)
    return;

  QListWidgetItem* item = new QListWidgetItem(data->getName());
  item->setData(Qt::UserRole, uid);

  if (indent)
    item->setText("    " + item->text());

  if (boost::shared_dynamic_cast<ssc::Image>(data))
    item->setIcon(QIcon(":/icons/volume.png"));
  else
    item->setIcon(QIcon(":/icons/surface.png"));

  if (after)
  {
    std::cout << "set " << item->text() << " before: " << after->text() << " " << this->currentRow() << std::endl;
    this->setCurrentItem(after);
    this->insertItem(this->currentRow(), item);
    this->setCurrentItem(item);
  }
  else
  {
    this->addItem(item);
  }

  emit listSizeChanged();
}


void DataListWidget::dataClickedSlot(QListWidgetItem* item)
{
  QString uid = item->data(Qt::UserRole).toString();
  emit dataSelected(uid);
}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


AllDataListWidget::AllDataListWidget(QWidget* parent) :
    DataListWidget(parent)
{
  this->setDragDropMode(QAbstractItemView::DragOnly);
  this->setDragEnabled(true);

  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateAllDataList()));
  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(populateAllDataList()));
}

AllDataListWidget::~AllDataListWidget()
{}

void AllDataListWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    startPos = event->pos();
  QListWidget::mousePressEvent(event);
}

void AllDataListWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - startPos).manhattanLength();
    if (distance >= 10)
      this->startDrag();
  }
}

void AllDataListWidget::startDrag()
{
  QListWidgetItem *item = currentItem();
  if (item)
  {
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(item->data(Qt::UserRole).toString());
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->start(Qt::CopyAction);

//    if (drag->start(Qt::MoveAction) == Qt::MoveAction)
//      delete item;
  }
}


void AllDataListWidget::populateAllDataList()
{
  this->clear();

  //add actions to the actiongroups and the contextmenu
  std::vector<ssc::DataPtr> sorted = sortOnGroupsAndAcquisitionTime(ssc::dataManager()->getData());
  QString lastDataActionUid = "________________________";
  for (std::vector<ssc::DataPtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
  {
    QString uid = (*iter)->getUid();

    if (uid.contains(lastDataActionUid))
    {
      this->populateData(uid, true);
    }
    else
    {
      this->populateData(uid, false);
      lastDataActionUid = uid;
    }

  }

  emit listSizeChanged();
}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------



SelectedDataListWidget::SelectedDataListWidget(QWidget* parent) :
    DataListWidget(parent)
{
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  this->viewport()->setAcceptDrops(true);
  this->setAcceptDrops(true);
  this->setDropIndicatorShown(true);
  this->setDefaultDropAction(Qt::CopyAction);
//  this->setDragDropMode(QAbstractItemView::DropOnly | QAbstractItemView::InternalMove);
  this->setDragDropMode(QAbstractItemView::DragDrop);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));
}

SelectedDataListWidget::~SelectedDataListWidget()
{}

void SelectedDataListWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    startPos = event->pos();
  QListWidget::mousePressEvent(event);
}

void SelectedDataListWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    int distance = (event->pos() - startPos).manhattanLength();
    if (distance >= 10)
      this->startDrag();
  }
}

void SelectedDataListWidget::startDrag()
{
  QListWidgetItem *item = currentItem();
  if (item)
  {
    QMimeData *mimeData = new QMimeData;
    mimeData->setText(item->data(Qt::UserRole).toString());
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

//    drag->start(Qt::MoveAction);

    if (drag->start(Qt::MoveAction) == Qt::MoveAction)
    {
     std::cout << "removing dragged item " << item->text() << std::endl;
      delete item;
    }
  }
}

void SelectedDataListWidget::dragEnterEvent(QDragEnterEvent *event)
{
  if (event->source()==this)
  {
    event->accept();
    return;
  }

  QStringList all = this->getData();
  if (all.contains(event->mimeData()->text()))
    event->ignore();
  else
    event->accept();
}

void SelectedDataListWidget::dragMoveEvent(QDragMoveEvent *event)
{
  DataListWidget::dragMoveEvent(event);
  event->setDropAction(Qt::MoveAction);
  if (event->source()==this)
  {
    event->accept();
//    std::cout << "from this" << std::endl;
    return;
  }

  //  event->accept();
  QStringList all = this->getData();
  if (all.contains(event->mimeData()->text()))
    event->ignore();
  else
    event->accept();
}


bool SelectedDataListWidget::dropMimeData(int index, const QMimeData* data, Qt::DropAction action)
{
  std::cout << "hit" << std::endl;
  return false;
}

void SelectedDataListWidget::dropEvent(QDropEvent *event)
{
  QListWidgetItem* pos = NULL;

  if (event->source()==this)
  {
    pos = this->itemAt(event->pos());
  }

//  std:: cout << "received dropEvent: " << event->mimeData()->text() << std::endl;
  this->populateData(event->mimeData()->text(), false, pos);
  event->setDropAction(Qt::MoveAction);
  event->accept();
  emit userChangedList();
  emit listSizeChanged();
}

QStringList SelectedDataListWidget::getData()
{
  QStringList retval;

  for(int i=0; i < this->count(); ++i)
  {
    QListWidgetItem* item = this->item(i);
    retval << item->data(Qt::UserRole).toString();
  }

  return retval;
}

void SelectedDataListWidget::deleteSlot()
{
  if(!mItemToDelete)
  {
    ssc::messageManager()->sendDebug("Found no item to delete...");
    return;
  }
  this->deleteItemSlot(mItemToDelete);
}

void SelectedDataListWidget::deleteItemSlot(QListWidgetItem* item)
{
  delete item;
  emit userChangedList();
  emit listSizeChanged();
}

void SelectedDataListWidget::contextMenuSlot(const QPoint& point)
{
  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  QMenu contextMenu(sender);

  QAction* action = new QAction("Remove", &contextMenu);

  QListWidgetItem* item = this->itemAt(point);
  if(!item)
  {
    ssc::messageManager()->sendDebug("Found no item to delete...");
  }
  mItemToDelete = item;

  connect(action, SIGNAL(triggered()), this, SLOT(deleteSlot()));
  contextMenu.addAction(action);

  contextMenu.exec(pointGlobal);
}

void SelectedDataListWidget::populateList()
{
  this->clear();

  std::vector<ssc::DataPtr> sorted = mViewGroupData->getData();
  for (std::vector<ssc::DataPtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
  {
    QString uid = (*iter)->getUid();
    this->populateData(uid);
  }

  emit listSizeChanged();
}

void SelectedDataListWidget::setViewGroupData(ViewGroupDataPtr viewGroupData)
{
  if (mViewGroupData)
  {
    disconnect(mViewGroupData.get(), SIGNAL(initialized()), this, SLOT(populateList()));
    disconnect(mViewGroupData.get(), SIGNAL(dataAdded(QString)), this, SLOT(populateList()));
    disconnect(mViewGroupData.get(), SIGNAL(dataRemoved(QString)), this, SLOT(populateList()));
  }

  mViewGroupData = viewGroupData;

  if (mViewGroupData)
  {
    connect(mViewGroupData.get(), SIGNAL(initialized()), this, SLOT(populateList()));
    connect(mViewGroupData.get(), SIGNAL(dataAdded(QString)), this, SLOT(populateList()));
    connect(mViewGroupData.get(), SIGNAL(dataRemoved(QString)), this, SLOT(populateList()));
  }
}



//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


class AbraClass : public QListWidget
{
public:
  AbraClass(QWidget* parent) : QListWidget(parent) {}
protected:
  void dropEvent(QDropEvent *event)
  {
//    QListWidgetItem* pos = NULL;

    if (event->source()==this)
    {
      std::cout << "drop this: " << event->mimeData()->formats().join(",") << std::endl;
      event->setDropAction(Qt::MoveAction);
    }

    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
    {
      QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
      QDataStream stream(&itemData, QIODevice::ReadOnly);
      int r, c;
      QMap<int, QVariant> v;
      stream >> r >> c >> v;
      std::cout << "var: " << r << " " << c << " " << v.size() << std::endl;
      if (!v.empty())
      {
        std::cout << "v: " << " " << v.begin()->typeName() << " " << v.begin()->toString() << std::endl;
      }
    }


  //  std:: cout << "received dropEvent: " << event->mimeData()->text() << std::endl;
//    this->populateData(event->mimeData()->text(), false, pos);
//    event->setDropAction(Qt::MoveAction);
//    event->accept();
    QListWidget::dropEvent(event);
  }


//  void dropEvent(QGraphicsSceneDragDropEvent* event)
//  {
//    if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
//    {
//      QTreeWidget *tree = dynamic_cast<QTreeWidget *> (event->source());
//
//      QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
//      QDataStream stream(&itemData, QIODevice::ReadOnly);
//
//      int r, c;
//      QMap<int, QVariant> v;
//      stream >> r >> c >> v;
//
//      QTreeWidgetItem *item = tree->topLevelItem(r);
//
//      if (item)
//      {
//        itemDropped(item);
//      }
//    }
//
//  }

//  bool SelectedDataListWidget::dropMimeData(int index, const QMimeData* data, Qt::DropAction action)
//  {
//    std::cout << "hit" << std::endl;
//    return false;
//  }

};

DataViewSelectionWidget::DataViewSelectionWidget(QWidget* parent)
{
  // TODO Auto-generated constructor stub
  QHBoxLayout* layout = new QHBoxLayout(this);

  mSelectedDataListWidget = new SelectedDataListWidget(this);
  mAllDataListWidget = new AllDataListWidget(this);
  QListWidget* test = new QListWidget(this);
  test->addItem("test1");
  test->addItem("test2");
  test->addItem("test3");
  test->addItem("test4");
  //test->setDragDropMode(QAbstractItemView::InternalMove);
  test->setDropIndicatorShown(false);
  test->setDragEnabled(true);
//  test->setAcceptDrops(true);

  AbraClass* abra = new AbraClass(this);
  abra->addItem("abra1");
  abra->addItem("abra2");
  abra->addItem("abra3");
  abra->addItem("abra4");
//  abra->setDragDropMode(QAbstractItemView::InternalMove);
  abra->setDropIndicatorShown(true);
  abra->setDragEnabled(true);
  abra->setAcceptDrops(true);
  abra->viewport()->setAcceptDrops(true);
  abra->setDragDropOverwriteMode(true);

  // AbstractItemView::DropIndicatorPosition
  layout->addWidget(mSelectedDataListWidget);
  layout->addWidget(mAllDataListWidget);
  layout->addWidget(test);
  layout->addWidget(abra);

  if (!viewManager()->getViewGroups().empty())
    mSelectedDataListWidget->setViewGroupData(viewManager()->getViewGroups()[0]->getData());
}

//void DataViewSelectionWidget::activeViewChanged


DataViewSelectionWidget::~DataViewSelectionWidget()
{
  // TODO Auto-generated destructor stub
}

}
