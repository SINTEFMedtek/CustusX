/*
 * sscDataViewSelectionWidget.cpp
 *
 *  \date Apr 5, 2011
 *      \author christiana
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
#include <QLabel>
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

  connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChangedSlot()));

//  this->setMinimumSize(QSize(20, 20));
  this->setSizePolicy(QSizePolicy::Expanding, this->sizePolicy().verticalPolicy());

}

/**
  * Only way to downsize the QListWidget, in conjunction with
  * this->setSizePolicy(QSizePolicy::Expanding, this->sizePolicy().verticalPolicy())
  *
  * Setting minimumSize did not work, for unknown reason.
  */
QSize DataListWidget::sizeHint() const
{
	return QSize(80,20);
}

//QSize DataListWidget::minimumSizeHint () const
//{
//	return QSize(20,20);
//}

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

void DataListWidget::itemSelectionChangedSlot()
{
  QList<QListWidgetItem*> items = this->selectedItems();
  if (items.empty())
    return;
  ssc::ImagePtr image = ssc::dataManager()->getImage(items[0]->data(Qt::UserRole).toString());
  if (image)
    ssc::dataManager()->setActiveImage(image);

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

  item->setToolTip(item->text());

  emit listSizeChanged();
}


//void DataListWidget::dataClickedSlot(QListWidgetItem* item)
//{
//  QString uid = item->data(Qt::UserRole).toString();
//  emit dataSelected(uid);
//}


//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


AllDataListWidget::AllDataListWidget(QWidget* parent) :
    DataListWidget(parent)
{
//  this->setDragDropMode(QAbstractItemView::DragOnly);
//  this->setDragEnabled(true);
  this->setDropIndicatorShown(false);
  this->setDragEnabled(true);
//  this->setAcceptDrops(true); // if we want to back-drag items (for deletion)

  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateAllDataList()));
  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(populateAllDataList()));
}

AllDataListWidget::~AllDataListWidget()
{}

void AllDataListWidget::mousePressEvent(QMouseEvent *event)
{
//  if (event->button() == Qt::LeftButton)
//    startPos = event->pos();
  QListWidget::mousePressEvent(event);
}

void AllDataListWidget::mouseMoveEvent(QMouseEvent *event)
{
  QListWidget::mouseMoveEvent(event);
//  if (event->buttons() & Qt::LeftButton)
//  {
//    int distance = (event->pos() - startPos).manhattanLength();
//    if (distance >= 10)
//      this->startDrag();
//  }
}

//void AllDataListWidget::startDrag()
//{
//  QListWidgetItem *item = currentItem();
//  if (item)
//  {
//    QMimeData *mimeData = new QMimeData;
//    mimeData->setText(item->data(Qt::UserRole).toString());
//    QDrag *drag = new QDrag(this);
//    drag->setMimeData(mimeData);
//
//    drag->start(Qt::CopyAction);
//
////    if (drag->start(Qt::MoveAction) == Qt::MoveAction)
////      delete item;
//  }
//}


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

  this->setDropIndicatorShown(true);
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->viewport()->setAcceptDrops(true);
  this->setDragDropOverwriteMode(true);

//  this->viewport()->setAcceptDrops(true);
//  this->setAcceptDrops(true);
//  this->setDropIndicatorShown(true);
//  this->setDefaultDropAction(Qt::CopyAction);
//  this->setDragDropMode(QAbstractItemView::DragDrop);

  connect(this, SIGNAL(userChangedList()), this, SLOT(userChangedListSlot()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenuSlot(const QPoint &)));

//  if (!viewManager()->getViewGroups().empty())
//    mSelectedDataListWidget->setViewGroupData(viewManager()->getViewGroups()[0]->getData());
  //nt ViewManager::getActiveViewGroup() const
  //void DataViewSelectionWidget::activeViewChanged
}


SelectedDataListWidget::~SelectedDataListWidget()
{}



void SelectedDataListWidget::userChangedListSlot()
{
  QStringList data = this->getData();
  std::reverse(data.begin(), data.end());

  mViewGroupData->clearData();
  for (int i=0; i<data.size(); ++i)
  {
    ssc::DataPtr current = ssc::dataManager()->getData(data[i]);
    if (!current)
      continue;
    mViewGroupData->addData(current);
  }
}

void SelectedDataListWidget::keyPressEvent(QKeyEvent* event)
{
  if (event->matches(QKeySequence::Delete) || event->matches(QKeySequence::Back))
  {
    QList<QListWidgetItem*> items = this->selectedItems();
    for (int i=0; i<items.size(); ++i)
      delete items[i];
    emit userChangedList();
    emit listSizeChanged();
  }
}

/**convert from the internal qt mime format used in the convenience views (QListWidget etc)
 * to a QMap with one entry for each QListItem::data() entry.
 *
 */
QMap<int, QVariant> SelectedDataListWidget::convertFromCustomQtMimeFormat(const QMimeData* mimeData) const
{
  QMap<int, QVariant> v;
  if (!mimeData->hasFormat("application/x-qabstractitemmodeldatalist"))
    return v;

  QByteArray itemData = mimeData->data("application/x-qabstractitemmodeldatalist");
  QDataStream stream(&itemData, QIODevice::ReadOnly);
  int r, c;
//  QMap<int, QVariant> v;
  stream >> r >> c >> v;
//  std::cout << "var: " << r << " " << c << " " << v.size() << std::endl;
//  if (!v.empty())
//  {
//    QString uid = v[Qt::UserRole].toString();
//    std::cout << "v: " << " " << v.begin()->typeName() << " " << v.begin()->toString() << " uid= " << uid << std::endl;
//
////      newUid = v.begin()->toString();
////    newUid = uid;
//  }
  return v;
}

void SelectedDataListWidget::dropEvent(QDropEvent *event)
{
  if (event->source()==this)
  {
    std::cout << "drop this: " << event->mimeData()->formats().join(",") << std::endl;
    event->setDropAction(Qt::MoveAction);
  }

  QString sourceUid;
  QMap<int, QVariant> sourceData = this->convertFromCustomQtMimeFormat(event->mimeData());
  if (sourceData.contains(Qt::UserRole))
    sourceUid = sourceData[Qt::UserRole].toString();

//  if (event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
//  {
//    QByteArray itemData = event->mimeData()->data("application/x-qabstractitemmodeldatalist");
//    QDataStream stream(&itemData, QIODevice::ReadOnly);
//    int r, c;
//    QMap<int, QVariant> v;
//    stream >> r >> c >> v;
//    std::cout << "var: " << r << " " << c << " " << v.size() << std::endl;
//    if (!v.empty())
//    {
//      QString uid = v[Qt::UserRole].toString();
//      std::cout << "v: " << " " << v.begin()->typeName() << " " << v.begin()->toString() << " uid= " << uid << std::endl;
//
////      newUid = v.begin()->toString();
//      newUid = uid;
//    }
//  }

  QListWidgetItem* itemToDelete = 0;
  for(int i=0; i < this->count(); ++i)
  {
    QListWidgetItem* item = this->item(i);
    if (item->data(Qt::UserRole)==sourceUid)
      itemToDelete = item;
  }

//  std:: cout << "received dropEvent: " << event->mimeData()->text() << std::endl;
//    this->populateData(event->mimeData()->text(), false, pos);
//    event->setDropAction(Qt::MoveAction);
//    event->accept();
//  std::cout << "dropaction " << event->dropAction() << std::endl;
  QListWidget::dropEvent(event);
  if (event->source()!=this) // remove old item if new if moved in from external source
    delete itemToDelete;

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
  std::reverse(sorted.begin(), sorted.end());
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

  this->populateList();
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

    QString newUid;
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
        newUid = v.begin()->toString();
      }
    }

    QListWidgetItem* itemToDelete = 0;
    for(int i=0; i < this->count(); ++i)
    {
      QListWidgetItem* item = this->item(i);
      if (item->text()==newUid)
        itemToDelete = item;
    }

  //  std:: cout << "received dropEvent: " << event->mimeData()->text() << std::endl;
//    this->populateData(event->mimeData()->text(), false, pos);
//    event->setDropAction(Qt::MoveAction);
//    event->accept();
    std::cout << "dropaction " << event->dropAction() << std::endl;
    QListWidget::dropEvent(event);
    if (event->source()!=this) // remove old item if new if moved in from external source
      delete itemToDelete;
  }

};

class TestClass : public QListWidget
{
public:
  TestClass(QWidget* parent) : QListWidget(parent) {}
protected:
  void dropEvent(QDropEvent *event)
  {
    // ignore drop
    event->setDropAction(Qt::MoveAction);
    event->acceptProposedAction();
//    QListWidget::dropEvent(event);
  }

};

DataViewSelectionWidget::DataViewSelectionWidget(QWidget* parent)
{
  // TODO Auto-generated constructor stub
  QHBoxLayout* layout = new QHBoxLayout(this);

  mSelectedDataListWidget = new SelectedDataListWidget(this);
  mAllDataListWidget = new AllDataListWidget(this);
#if 0
  TestClass* test = new TestClass(this);
  test->addItem("test1");
  test->addItem("test2");
  test->addItem("test3");
  test->addItem("test4");
  //test->setDragDropMode(QAbstractItemView::InternalMove);
  test->setDropIndicatorShown(false);
  test->setDragEnabled(true);
  test->setAcceptDrops(true);

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

  layout->addWidget(test);
  layout->addWidget(abra);
#endif

  QVBoxLayout* selLayout = new QVBoxLayout;
  mVisibleLabel = new QLabel("Visible", this);
  selLayout->addWidget(mVisibleLabel);
  selLayout->addWidget(mSelectedDataListWidget);
  layout->addLayout(selLayout);

  QVBoxLayout* allLayout = new QVBoxLayout;
  allLayout->addWidget(new QLabel("All"));
  allLayout->addWidget(mAllDataListWidget);
  layout->addLayout(allLayout);

//  layout->addLayout(selLayout);
//  layout->addLayout(allLayout);

  connect(viewManager(), SIGNAL(activeViewChanged()), this, SLOT(viewGroupChangedSlot()));
  this->viewGroupChangedSlot();
//  if (!viewManager()->getViewGroups().empty())
//    mSelectedDataListWidget->setViewGroupData(viewManager()->getViewGroups()[0]->getData());
//  //nt ViewManager::getActiveViewGroup() const
//  //void DataViewSelectionWidget::activeViewChanged
}

void DataViewSelectionWidget::viewGroupChangedSlot()
{
  int vg = viewManager()->getActiveViewGroup();
  if (vg<0)
    vg = 0;

  if (( vg>=0 )&&( vg<int(viewManager()->getViewGroups().size()) ))
  {
    mSelectedDataListWidget->setViewGroupData(viewManager()->getViewGroups()[vg]->getData());
  }

  mVisibleLabel->setText("Visible in group "+qstring_cast(vg));
}


DataViewSelectionWidget::~DataViewSelectionWidget()
{
  // TODO Auto-generated destructor stub
}

}
