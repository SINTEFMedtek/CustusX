#include "cxViewWrapper.h"

#include <QMenu>
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "sscMesh.h"

namespace cx
{

SyncedValue::SyncedValue(QVariant val) : mValue(val) {}
SyncedValuePtr SyncedValue::create(QVariant val)
{
  return SyncedValuePtr(new SyncedValue(val));
}
void SyncedValue::set(QVariant val)
{
  //std::cout << "emit changed new=" << val.toDouble() << ", old=" << mValue.toDouble() << std::endl;
  if (mValue==val)
    return;
  mValue = val;
  //std::cout << "emit changed" << std::endl;
  emit changed();
}
QVariant SyncedValue::get() const
{
  return mValue;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


std::vector<ssc::DataPtr> ViewGroupData::getData() const
{
  return mData;
}

void ViewGroupData::addData(ssc::DataPtr data)
{
  if (!data)
    return;
  if (std::count(mData.begin(), mData.end(), data))
    return;
  mData.push_back(data);
  emit dataAdded(qstring_cast(data->getUid()));
}

void ViewGroupData::removeData(ssc::DataPtr data)
{
  if (!data)
    return;
  if (!std::count(mData.begin(), mData.end(), data))
    return;
  mData.erase(std::find(mData.begin(), mData.end(), data));
  emit dataRemoved(qstring_cast(data->getUid()));
}

void ViewGroupData::clearData()
{
  while (!mData.empty())
    this->removeData(mData.front());
}

std::vector<ssc::ImagePtr> ViewGroupData::getImages() const
{
  std::vector<ssc::ImagePtr> retval;
  for (unsigned i=0; i<mData.size(); ++i)
  {
    ssc::ImagePtr data = boost::shared_dynamic_cast<ssc::Image>(mData[i]);
    if (data)
      retval.push_back(data);
  }
  return retval;
}

std::vector<ssc::MeshPtr> ViewGroupData::getMeshes() const
{
  std::vector<ssc::MeshPtr> retval;
  for (unsigned i=0; i<mData.size(); ++i)
  {
    ssc::MeshPtr data = boost::shared_dynamic_cast<ssc::Mesh>(mData[i]);
    if (data)
      retval.push_back(data);
  }
  return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

void ViewWrapper::setViewGroup(ViewGroupDataPtr group)
{
  mViewGroup = group;
  connect(mViewGroup.get(), SIGNAL(dataAdded(QString)), SLOT(dataAddedSlot(QString)));
  connect(mViewGroup.get(), SIGNAL(dataRemoved(QString)), SLOT(dataRemovedSlot(QString)));

  std::vector<ssc::DataPtr> data = mViewGroup->getData();
  for (unsigned i=0; i<data.size(); ++i)
  {
    this->dataAddedSlot(qstring_cast(data[i]->getUid()));
  }

}

void ViewWrapper::dataAddedSlot(QString uid)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(string_cast(uid));
  if (image)
    this->imageAdded(image);
  ssc::MeshPtr mesh = ssc::dataManager()->getMesh(string_cast(uid));
  if (mesh)
    this->meshAdded(mesh);
}

void ViewWrapper::dataRemovedSlot(QString uid)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(string_cast(uid));
  if (image)
    this->imageRemoved(image);
  ssc::MeshPtr mesh = ssc::dataManager()->getMesh(string_cast(uid));
  if (mesh)
    this->meshRemoved(mesh);
}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  QMenu contextMenu(sender);

  //add actions to the actiongroups and the contextmenu
  std::map<std::string, std::string>::iterator iter;

  std::map<std::string, std::string> imageUidsAndNames = ssc::dataManager()->getImageUidsAndNames();
  for(iter=imageUidsAndNames.begin(); iter != imageUidsAndNames.end(); ++iter)
  {
    this->addDataAction(iter->first, &contextMenu);
  }
  
  contextMenu.addSeparator();

  std::map<std::string, std::string> meshUidsAndNames = ssc::dataManager()->getMeshUidsWithNames();
  for(iter=meshUidsAndNames.begin(); iter != meshUidsAndNames.end(); ++iter)
  {
    this->addDataAction(iter->first, &contextMenu);
  }

  //append specific info from derived classes
  this->appendToContextMenu(contextMenu);

  contextMenu.exec(pointGlobal);
}

void ViewWrapper::addDataAction(std::string uid, QMenu* contextMenu)
{
  ssc::DataPtr data = ssc::dataManager()->getData(uid);

  QAction* action = new QAction(qstring_cast(data->getName()), contextMenu);
  action->setData(QVariant(qstring_cast(uid)));
  action->setCheckable(true);
  std::vector<ssc::DataPtr> allVisible = mViewGroup->getData();
  action->setChecked(std::count(allVisible.begin(), allVisible.end(), data));
  connect(action, SIGNAL(triggered()), this, SLOT(dataActionSlot()));
  contextMenu->addAction(action);
}

void ViewWrapper::dataActionSlot()
{
  QAction* theAction = static_cast<QAction*>(sender());
  if(!theAction)
    return;

  QString uid = theAction->data().toString();
  ssc::DataPtr data = ssc::dataManager()->getData(uid.toStdString());
  ssc::ImagePtr image = ssc::dataManager()->getImage(data->getUid());

  if (theAction->isChecked())
  {
    mViewGroup->addData(data);
    if (image)
      ssc::dataManager()->setActiveImage(image);
  }
  else
  {
    mViewGroup->removeData(data);
    if (image)
      ssc::dataManager()->setActiveImage(ssc::ImagePtr());
  }

  Navigation().centerToGlobalImageCenter(); // reset center for convenience
}
  
void ViewWrapper::connectContextMenu(ssc::View* view)
{
   connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contextMenuSlot(const QPoint &)));
}
}//namespace cx
