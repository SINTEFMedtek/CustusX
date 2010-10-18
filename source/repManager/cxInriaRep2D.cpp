#include "cxInriaRep2D.h"

#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkTransform.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>
#include "sscView.h"
#include "cxTool.h"
#include "sscToolManager.h"

namespace cx
{
InriaRep2D::InriaRep2D(const QString& uid, const QString& name) :
  ssc::RepImpl(uid, name),
  mType("cxInriaRep2D"),
  mTool(NULL),
  mInria(vtkViewImage2D::New()),
  mConnections(vtkEventQtSlotConnect::New())
{
  //mInria->SetLeftButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  //mInria->SetMiddleButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  mInria->SetLeftButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  mInria->SetMiddleButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  mInria->SetWheelInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  mInria->SetRightButtonInteractionStyle(vtkViewImage2D::NO_INTERACTION);

  mInria->SetLinkZoom(true);

}
InriaRep2D::~InriaRep2D()
{}
QString InriaRep2D::getType() const
{
  return mType;
} 
void InriaRep2D::setImage(ssc::ImagePtr image)
{
  if(mImage)
    disconnect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(updateUserTransform()));
  mImage = image;
  if(mImage)
    connect(mImage.get(), SIGNAL(transformChanged()), this, SLOT(updateUserTransform()));
  this->updateUserTransform();
}
void InriaRep2D::connectToView(ssc::View *theView)
{
  if(isConnectedToView(theView))
    return;

  mViews.insert(theView);
  this->addRepActorsToViewRenderer(theView);
}
void InriaRep2D::disconnectFromView(ssc::View *theView)
{
  mViews.erase(theView);
  this->removeRepActorsFromViewRenderer(theView);
}
void InriaRep2D::setDataset(vtkMetaDataSet *dataset)
{
  mInria->RemoveAllDataSet();

  vtkImageData *imageDataset = this->getImageDataFromVtkMetaDataSet(dataset);
  if(imageDataset != NULL)
  {
    mInria->AddDataSet(imageDataset);
  }
  else
  {
    std::cout << "Could not get vtkImageData from vtkmetaDataSet..\n";
  }
}
vtkViewImage2DPtr InriaRep2D::getVtkViewImage2D()
{
  return mInria;
}
void InriaRep2D::setTool(Tool* tool)
{
  if(tool == NULL)
    return;

  mTool = tool;

  connect(mTool, SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
          this, SLOT(toolTransformAndTimeStampSlot(Transform3D, double)));
  connect(mTool, SIGNAL(toolVisible(bool)),
          this, SLOT(toolVisibleSlot(bool)));
}
void InriaRep2D::removeTool(Tool* tool)
{
  if(tool == mTool)
  {
    disconnect(mTool, SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
               this, SLOT(receiveToolTransformAndTimeStamp(Transform3D, double)));
    disconnect(mTool, SIGNAL(toolVisible(bool)),
               this, SLOT(receiveToolVisible(bool)));
    mTool = NULL;
  }
}
bool InriaRep2D::hasTool(Tool* tool)
{
  if(mTool == tool)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/**
 * Pick a point in data ref space
 * @param[in] object Not used for anything
 */
void InriaRep2D::pickSurfacePointSlot(vtkObject* object)
{
  double point[3];
  mInria->GetCurrentPoint(point);

  emit pointPicked(point[0], point[1], point[2]);
}
/**
 * Sets the crosshair in the inria2D views
 * @param[in] x world coordinat, ref space
 * @param[in] y world coordinat, ref space
 * @param[in] z world coordinat, ref space
 */
void InriaRep2D::syncSetPosition(double x, double y, double z)
{
  const double point[3] = {x,y,z};
  mInria->SyncSetPosition(point);
}
vtkImageData *InriaRep2D::getImageDataFromVtkMetaDataSet(vtkMetaDataSet *dataset)
{
  if(dataset == NULL)
    std::cout << "NULL dataset in getImageDataFromVtkMetaDataSet..\n";

  if(dataset->GetType() == vtkMetaDataSet::VTK_META_IMAGE_DATA)
  {
    vtkMetaImageData *metaImgData = dynamic_cast<vtkMetaImageData*>(dataset);

    if(metaImgData == NULL)
    {
      std::cout << "oops, metaimgData could not be cast to vtkMetaImageData in "
        "getImageDataFromVtkMeta..\n";
    }
    else
    {
      return metaImgData->GetImageData();
    }
  }

  return NULL;
}
void InriaRep2D::addRepActorsToViewRenderer(ssc::View* view)
{
  mInria->SetRenderWindow(view->GetRenderWindow());
  mInria->SetRenderer(view->getRenderer());
  mInria->SetBackgroundColor(0.0, 0.0, 0.0);
  //mInria->Render();

  mConnections->Connect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void InriaRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mInria->RemoveAllDataSet();
  //mInria->Uninitialize();
  // Why???
  //mInria->SetRenderWindow(NULL);
  //mInria->SetRenderer(NULL);

  mConnections->Disconnect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
/**
 * Receives tool transforms from the (dominant?) tool
 * @param prMt the transformation that brings you from patient ref to data ref
 * @param timestamp the time the transform was created
 */
void InriaRep2D::toolTransformAndTimeStampSlot(Transform3D prMt, double timestamp)
{
  ssc::Transform3DPtr rMt(new ssc::Transform3D((*ToolManager::getInstance()->get_rMpr())*prMt));
  double position[3] = { rMt->matrix()->GetElement(0,3),
      rMt->matrix()->GetElement(1,3),
      rMt->matrix()->GetElement(2,3)};
  /*double position[3] = { matrix.matrix()->GetElement(0,3),
                         matrix.matrix()->GetElement(1,3),
                         matrix.matrix()->GetElement(2,3)};*/
  //TODO, this is wrong
  mInria->SyncSetPosition(position);
}
/**
 * \warning NOT IMPLEMENTED!!!
 * @param visible wheter or not a tool should be visible
 */
void InriaRep2D::toolVisibleSlot(bool visible)
{
  //TODO either implement or remove?
}
void InriaRep2D::updateUserTransform()
{
  /*
  if(!mImage)
      return;
  ssc::Transform3D rMd = mImage->get_rMd();
  std::cout << "Transform set in the \n"+getName() << rMd << std::endl;
  vtkTransformPtr transform = vtkTransform::New();
  transform->SetMatrix(rMd.matrix());
  mInria->SetTransform(transform);
  mInria->Update();
  */
}
}//namespace cx
