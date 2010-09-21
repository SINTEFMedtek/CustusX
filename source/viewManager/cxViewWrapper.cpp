#include "cxViewWrapper.h"

#include <QMenu>
#include "vtkCamera.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "sscMesh.h"
#include "sscTypeConversions.h"

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

CameraData::CameraData()
{
}

void CameraData::setCamera(vtkCameraPtr camera)
{
  mCamera = camera;
}

vtkCameraPtr CameraData::getCamera() const
{
  if (!mCamera)
    mCamera = vtkCameraPtr::New();
  return mCamera;
}

void CameraData::addTextElement(QDomNode parentNode, QString name, QString value) const
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement node = doc.createElement(name);
  node.appendChild(doc.createTextNode(value));
  parentNode.appendChild(node);
}

void CameraData::addXml(QDomNode dataNode) const
{
  if (!mCamera)
    return;

  this->addTextElement(dataNode, "position",      qstring_cast(ssc::Vector3D(mCamera->GetPosition())));
  this->addTextElement(dataNode, "focalPoint",    qstring_cast(ssc::Vector3D(mCamera->GetFocalPoint())));
  this->addTextElement(dataNode, "viewUp",        qstring_cast(ssc::Vector3D(mCamera->GetViewUp())));
  this->addTextElement(dataNode, "nearClip",      qstring_cast(mCamera->GetClippingRange()[0]));
  this->addTextElement(dataNode, "farClip",       qstring_cast(mCamera->GetClippingRange()[1]));
  this->addTextElement(dataNode, "parallelScale", qstring_cast(mCamera->GetParallelScale()));
}

void CameraData::parseXml(QDomNode dataNode)
{
  ssc::Vector3D vup = ssc::Vector3D::fromString(dataNode.namedItem("viewUp").toElement().text());
  if (ssc::similar(vup.length(), 0.0))
    return; // ignore reading if undefined data

  this->getCamera();

  ssc::Vector3D position =   ssc::Vector3D::fromString(dataNode.namedItem("position").toElement().text());
  ssc::Vector3D focalPoint = ssc::Vector3D::fromString(dataNode.namedItem("focalPoint").toElement().text());
  ssc::Vector3D viewUp =     ssc::Vector3D::fromString(dataNode.namedItem("viewUp").toElement().text());
  double nearClip =      dataNode.namedItem("nearClip").toElement().text().toDouble();
  double farClip =       dataNode.namedItem("farClip").toElement().text().toDouble();
  double parallelScale = dataNode.namedItem("parallelScale").toElement().text().toDouble();

  mCamera->SetClippingRange(nearClip, farClip);
  mCamera->SetPosition(position.begin());
  mCamera->SetFocalPoint(focalPoint.begin());
  mCamera->ComputeViewPlaneNormal();
  mCamera->SetViewUp(viewUp.begin());
  mCamera->SetParallelScale(parallelScale);
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ViewGroupData::ViewGroupData() :
    mCamera3D(CameraData::create())
{

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
  this->imageRemoved(uid);
  this->meshRemoved(uid);
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
    Navigation().centerToGlobalImageCenter(); // reset center for convenience
    mViewGroup->requestInitialize();
  }
}
  
void ViewWrapper::connectContextMenu(ssc::View* view)
{
   connect(view, SIGNAL(customContextMenuRequested(const QPoint &)),
       this, SLOT(contextMenuSlot(const QPoint &)));
}
}//namespace cx
