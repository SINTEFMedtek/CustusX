#include "cxViewWrapper.h"

#include <QMenu>
#include "vtkCamera.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "cxCameraControl.h"
#include "sscImageAlgorithms.h"

namespace cx
{

SyncedValue::SyncedValue(QVariant val) : mValue(val) {}
SyncedValuePtr SyncedValue::create(QVariant val)
{
  return SyncedValuePtr(new SyncedValue(val));
}
void SyncedValue::set(QVariant val)
{
  if (mValue==val)
    return;
  mValue = val;
  emit changed();
}
QVariant SyncedValue::get() const
{
  return mValue;
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ViewGroupData::ViewGroupData() :
    mCamera3D(CameraData::create())
{
  connect(ssc::dataManager(), SIGNAL(dataRemoved(QString)), this, SLOT(removeDataSlot(QString)));
}

void ViewGroupData::removeDataSlot(QString uid)
{
  for (unsigned i=0; i<mData.size(); ++i)
    if (mData[i]->getUid()==uid)
      this->removeData(mData[i]);
}

void ViewGroupData::requestInitialize()
{
  emit initialized();
}

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

ViewGroupData::Options ViewGroupData::getOptions() const
{
  return mOptions;
}

void ViewGroupData::setOptions(ViewGroupData::Options options)
{
  mOptions = options;
  emit optionsChanged();
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
  ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
  if (image)
    this->imageAdded(image);
  ssc::MeshPtr mesh = ssc::dataManager()->getMesh(uid);
  if (mesh)
    this->meshAdded(mesh);
}

void ViewWrapper::dataRemovedSlot(QString uid)
{
  this->imageRemoved(uid);
  this->meshRemoved(uid);
}

void ViewWrapper::contextMenuSlot(const QPoint& point)
{
  QWidget* sender = dynamic_cast<QWidget*>(this->sender());
  QPoint pointGlobal = sender->mapToGlobal(point);
  QMenu contextMenu(sender);

  //add actions to the actiongroups and the contextmenu
  std::vector<ssc::DataPtr> sorted = sortOnGroupsAndAcquisitionTime(ssc::dataManager()->getData());
  mLastDataActionUid = "________________________";
  for (std::vector<ssc::DataPtr>::iterator iter=sorted.begin(); iter!=sorted.end(); ++iter)
  {
    this->addDataAction((*iter)->getUid(), &contextMenu);
  }

//  std::map<QString, QString>::iterator iter;
//  std::map<QString, QString> imageUidsAndNames = ssc::dataManager()->getImageUidsAndNames();
//  for(iter=imageUidsAndNames.begin(); iter != imageUidsAndNames.end(); ++iter)
//  {
//    this->addDataAction(iter->first, &contextMenu);
//  }
//
//  contextMenu.addSeparator();
//
//  std::map<QString, QString> meshUidsAndNames = ssc::dataManager()->getMeshUidsWithNames();
//  for(iter=meshUidsAndNames.begin(); iter != meshUidsAndNames.end(); ++iter)
//  {
//    this->addDataAction(iter->first, &contextMenu);
//  }

  //append specific info from derived classes
  this->appendToContextMenu(contextMenu);

  contextMenu.exec(pointGlobal);
}

void ViewWrapper::addDataAction(QString uid, QMenu* contextMenu)
{
  ssc::DataPtr data = ssc::dataManager()->getData(uid);

  QAction* action = new QAction(qstring_cast(data->getName()), contextMenu);

  if (boost::shared_dynamic_cast<ssc::Image>(data))
    action->setIcon(QIcon(":/icons/volume.png"));
  else
    action->setIcon(QIcon(":/icons/surface.png"));

  if (uid.contains(mLastDataActionUid))
  {
    action->setText("    " + action->text());
  }
  else
  {
    mLastDataActionUid = uid;
  }

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
  ssc::DataPtr data = ssc::dataManager()->getData(uid);
  ssc::ImagePtr image = ssc::dataManager()->getImage(data->getUid());

  bool firstData = mViewGroup->getData().empty();

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

  if (firstData)
  {
    Navigation().centerToGlobalDataCenter(); // reset center for convenience
    mViewGroup->requestInitialize();
  }
}
  
void ViewWrapper::connectContextMenu(ssc::View* view)
{
   connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contextMenuSlot(const QPoint &)));
}

QStringList ViewWrapper::getAllDataNames() const
{
  std::vector<ssc::DataPtr> data = mViewGroup->getData();

  QStringList text;
  for (unsigned i = 0; i < data.size(); ++i)
  {
    text << qstring_cast(data[i]->getName());
  }
  std::reverse(text.begin(), text.end());
  return text;
}


}//namespace cx
