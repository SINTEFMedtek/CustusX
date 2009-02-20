#include "cxInriaRep2D.h"

#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include <vtkMetaDataSet.h>
#include <vtkMetaImageData.h>
#include "sscView.h"
#include "cxTool.h"

/**
 * cxInriaRep2D.cpp
 *
 * \brief
 *
 * \date Dec 8, 2008
 * \author: Janne Beate Bakeng, SINTEF
 */
namespace cx
{
InriaRep2D::InriaRep2D(const std::string& uid, const std::string& name) :
  ssc::RepImpl(uid, name),
  mType("cxInriaRep2D"),
  mTool(NULL),
  mInria(vtkViewImage2D::New()),
  mConnections(vtkEventQtSlotConnect::New())
{
  mInria->SetLeftButtonInteractionStyle(vtkViewImage2D::WINDOW_LEVEL_INTERACTION);
  mInria->SetMiddleButtonInteractionStyle(vtkViewImage2D::SELECT_INTERACTION);
  mInria->SetWheelInteractionStyle(vtkViewImage2D::ZOOM_INTERACTION);
  mInria->SetRightButtonInteractionStyle(vtkViewImage2D::NO_INTERACTION);

  mInria->SetLinkZoom(true);

}
InriaRep2D::~InriaRep2D()
{}
std::string InriaRep2D::getType() const
{
  return mType;
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
void InriaRep2D::pickSurfacePointSlot(vtkObject* object)
{
  double point[3];
  mInria->GetCurrentPoint(point);

  emit pointPicked(point[0], point[1], point[2]);
}
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
  mInria->Render();

  mConnections->Connect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::MiddleButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void InriaRep2D::removeRepActorsFromViewRenderer(ssc::View* view)
{
  mInria->RemoveAllDataSet();
  mInria->SetRenderWindow(NULL);
  mInria->SetRenderer(NULL);

  mConnections->Disconnect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::MiddleButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void InriaRep2D::toolTransformAndTimeStampSlot(Transform3D matrix, double timestamp)
{
  double position[3] = { matrix.matrix()->GetElement(0,3),
                         matrix.matrix()->GetElement(1,3),
                         matrix.matrix()->GetElement(2,3)};
  mInria->SyncSetPosition(position);
}
void InriaRep2D::toolVisibleSlot(bool visible)
{}
}//namespace cx
