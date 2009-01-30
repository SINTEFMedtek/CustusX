#include "cxVolumetricRep.h"

#include <limits>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkLineSource.h>
#include <vtkProbeFilter.h>
#include <vtkImageData.h>
#include <vtkDataSetAttributes.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkEventQtSlotConnect.h>
#include "sscView.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxInriaRep2D.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxMessageManager.h"

namespace cx
{
VolumetricRepPtr VolumetricRep::New(const std::string& uid, const std::string& name)
{
  VolumetricRepPtr retval(new VolumetricRep(uid, name));
  retval->mSelf = retval;
  return retval;
}
VolumetricRep::VolumetricRep(const std::string& uid, const std::string& name) :
  ssc::VolumetricRep(uid, name),
  mMessageManager(MessageManager::getInstance()),
  mThreshold(25),
  mCurrentX(std::numeric_limits<double>::max()),
  mCurrentY(std::numeric_limits<double>::max()),
  mCurrentZ(std::numeric_limits<double>::max()),
  mPickedPointActor(NULL),
  mConnections(vtkEventQtSlotConnect::New())
{}
void VolumetricRep::setImage(ssc::ImagePtr image)
{
  if (image==mImage)
  {
    return;
  }
  if (mImage)
  {
    disconnect(this, SIGNAL(addPermanentPoint(double, double, double)),
          mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
  }
  ssc::VolumetricRep::setImage(image);
  connect(this, SIGNAL(addPermanentPoint(double, double, double)),
        mImage.get(), SLOT(addLandmarkSlot(double, double, double)));
}
void VolumetricRep::setThreshold(const int threshold)
{
  mThreshold = threshold;
}
void VolumetricRep::pickSurfacePoint(vtkObject* object, double &x, double &y, double &z)
{
  vtkRenderWindowInteractor* iren =
      vtkRenderWindowInteractor::SafeDownCast(object);

  if(iren == NULL)
  {
    mMessageManager.sendError("Could not pick the point because the renderwindowinteractor is NULL.");
    return;
  }

  int pickedPoint[3];
  iren->GetEventPosition(pickedPoint);

  int selectionX = pickedPoint[0];
  int selectionY = pickedPoint[1];
  int selectionZ = pickedPoint[2];

  vtkRenderer* renderer = this->getRendererFromRenderWindow(*iren);
  if(renderer == NULL)
  {
    mMessageManager.sendError("Could not pick the point because the renderer is NULL.");
    return;
  }

  //Get camera focal point and position
  double cameraPos[4], cameraFocalPoint[4];
  vtkCamera* camera = renderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->GetPosition((double*)cameraPos);
  cameraPos[3] = 1.0;
  camera->GetFocalPoint((double*)cameraFocalPoint);
  cameraFocalPoint[3] = 1.0;

  //Get the cameras focal point in display coordinates.
  //We need a depth value for the z-buffer.
  double* displayCoords;
  renderer->SetWorldPoint(cameraFocalPoint[0], cameraFocalPoint[1], cameraFocalPoint[2], cameraFocalPoint[3]);
  renderer->WorldToDisplay();
  displayCoords = renderer->GetDisplayPoint();
  selectionZ = displayCoords[2];

  //Convert the selection point into world coordinates
  double* worldCoords;
  renderer->SetDisplayPoint(selectionX, selectionY, selectionZ);
  renderer->DisplayToWorld();
  worldCoords = renderer->GetWorldPoint();

  double pickPosition[3];
  for(int i=0; i<3; i++)
  {
    pickPosition[i] = worldCoords[i]/worldCoords[3];
  }

  //  Compute the ray endpoints.  The ray is along the line running from
  //  the camera position to the selection point, starting where this line
  //  intersects the front clipping plane, and terminating where this
  //  line intersects the back clipping plane.
  double ray[3], cameraDOP[3], rayLength;
  for(int i=0; i<3; i++)
  {
    ray[i] = pickPosition[i] - cameraPos[i];
    cameraDOP[i] = cameraFocalPoint[i] - cameraPos[i];
  }

  vtkMath::Normalize(cameraDOP);
  if((rayLength = vtkMath::Dot(cameraDOP, ray)) == 0.0)
  {
    mMessageManager.sendError("Cannot process the picked point.");
  }
  double* clipRange = camera->GetClippingRange();
  double tFront, tBack;
  double pointFrontWorld[4], pointBackWorld[4];
  if(camera->GetParallelProjection())
  {
    tFront = clipRange[0] - rayLength;
    tBack = clipRange[1] - rayLength;
    for(int i=0;i<3;i++)
    {
      pointFrontWorld[i] = pickPosition[i] + tFront*cameraDOP[i];
      pointBackWorld[i] = pickPosition[i] + tBack*cameraDOP[i];
    }
  }
  else
  {
    tFront = clipRange[0]/rayLength;
    tBack = clipRange[1]/rayLength;
    for(int i=0;i<3;i++)
    {
      pointFrontWorld[i] = cameraPos[i] + tFront*ray[i];
      pointBackWorld[i] = cameraPos[i] + tBack*ray[i];
    }
  }
  pointFrontWorld[3] = pointBackWorld[3] = 1.0;

  //Creating the line from the camera throught the picked point
  //into the volume
  vtkSmartPointer<vtkLineSource> lineSource = vtkLineSource::New();
  lineSource->SetPoint1(pointFrontWorld[0], pointFrontWorld[1], pointFrontWorld[2]);
  lineSource->SetPoint2(pointBackWorld[0], pointBackWorld[1], pointBackWorld[2]);
  lineSource->SetResolution(tBack);
  lineSource->Update();

  //Creating a probefilter
  vtkSmartPointer<vtkProbeFilter> probeFilter = vtkProbeFilter::New();
  probeFilter->SetSource(mImage->getBaseVtkImageData());
  probeFilter->SetInputConnection(lineSource->GetOutputPort());
  probeFilter->Update();

  vtkSmartPointer<vtkPolyDataMapper> probeFilterMapper = vtkPolyDataMapper::New();
  probeFilterMapper->SetInput(probeFilter->GetPolyDataOutput());

  vtkSmartPointer<vtkDataSetAttributes> dataSetAttribute = vtkDataSetAttributes::New();
  dataSetAttribute = (vtkDataSetAttributes*)probeFilterMapper->GetInput()->GetPointData();

  double value;
  int tuple;
  for(int i=0;i<dataSetAttribute->GetScalars()->GetNumberOfTuples(); i++)
  {
    tuple = i;
    dataSetAttribute->GetScalars()->GetTuple(i, &value);
    if(value > mThreshold)
      break;
  }

  double skinPoint[3];
  probeFilterMapper->GetInput()->GetPoint(tuple, skinPoint);

  //Make an sphere actor to show where the calculated point is
  if(mPickedPointActor == NULL )
  {
    vtkSmartPointer<vtkSphereSource> pickedPointSphereSource = vtkSphereSource::New();
    pickedPointSphereSource->SetRadius(2);
    vtkSmartPointer<vtkPolyDataMapper> pickedPointMapper = vtkPolyDataMapper::New();
    pickedPointMapper->SetInputConnection(pickedPointSphereSource->GetOutputPort());
    mPickedPointActor = vtkActor::New();
    mPickedPointActor->SetMapper(pickedPointMapper);
    mPickedPointActor->GetProperty()->SetColor(0,1,0);
  }
  if(renderer->HasViewProp(mPickedPointActor))
      renderer->RemoveActor(mPickedPointActor);

  mPickedPointActor->SetPosition(skinPoint[0], skinPoint[1], skinPoint[2]);
  renderer->AddActor(mPickedPointActor);
  mCurrentX = skinPoint[0];
  mCurrentY = skinPoint[1];
  mCurrentZ = skinPoint[2];

  std::stringstream stream;
  stream << "Picked point: (";
  stream << mCurrentX;
  stream << ",";
  stream << mCurrentY;
  stream << ",";
  stream << mCurrentZ;
  stream << ")";
  mMessageManager.sendInfo(stream.str());

  iren->GetRenderWindow()->Render();

  //TODO: Should this really be here???
  //Nope.
  //Send out a signal that the repmanager is connected to.
  //The slot in the repmanager should take care of syncing...
  // ... should this be handled by the view that receives the mouse click?
  InriaRep3DMap* inriaRep3DMapPtr = RepManager::getInstance()->getInria3DReps();
  InriaRep3DMap::iterator itInriaRep3D = inriaRep3DMapPtr->begin();
  if(itInriaRep3D != inriaRep3DMapPtr->end())
    itInriaRep3D->second->getVtkViewImage3D()->SyncSetPosition(skinPoint);
  else
    mMessageManager.sendWarning("The volumetric representation: "+mUid+", could not sync the views.");
}
void VolumetricRep::makePointPermanent()
{

  if(mCurrentX == std::numeric_limits<double>::max() &&
      mCurrentY == std::numeric_limits<double>::max() &&
      mCurrentY == std::numeric_limits<double>::max())
  {
    mMessageManager.sendWarning("Could not make the point permanent, invalid values.");
    return;
  }
  emit addPermanentPoint(mCurrentX, mCurrentY, mCurrentZ);
}
vtkSmartPointer<vtkRenderer> VolumetricRep::getRendererFromRenderWindow(vtkRenderWindowInteractor& iren)
{
  vtkSmartPointer<vtkRenderer> renderer = NULL;
  std::set<ssc::View*>::const_iterator it = mViews.begin();
  for(; it != mViews.end(); ++it)
  {
    if(iren.GetRenderWindow() == (*it)->getRenderWindow())
      renderer = (*it)->getRenderer();
  }
  return renderer;
}
void VolumetricRep::addRepActorsToViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    mMessageManager.sendError("Trying to add rep actors to view renderer, but view is NULL in volumetric rep.");
    return;
  }
  ssc::VolumetricRep::addRepActorsToViewRenderer(view);

  mConnections->Connect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void VolumetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    mMessageManager.sendError("Trying to remove rep actors to view renderer, but view is NULL in volumetric rep.");
    return;
  }
  ssc::VolumetricRep::removeRepActorsFromViewRenderer(view);

  mConnections->Disconnect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void VolumetricRep::pickSurfacePointSlot(vtkObject* object)
{
  this->pickSurfacePoint(object, mCurrentX, mCurrentY, mCurrentY);

}
}//namespace cx
