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
#include <vtkDoubleArray.h>
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
    disconnect(this, SIGNAL(addPermanentPoint(double, double, double, unsigned int)),
          mImage.get(), SLOT(addLandmarkSlot(double, double, double, unsigned int)));
  }
  ssc::VolumetricRep::setImage(image);
  connect(this, SIGNAL(addPermanentPoint(double, double, double, unsigned int)),
        mImage.get(), SLOT(addLandmarkSlot(double, double, double, unsigned int)));
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
    mMessageManager->sendError("Could not pick the point because the renderwindowinteractor is NULL.");
    return;
  }

  int pickedPoint[3];
  iren->GetEventPosition(pickedPoint);

  int selectionX = pickedPoint[0];
  int selectionY = pickedPoint[1];
  int selectionZ = pickedPoint[2];

  mCurrentRenderer = this->getRendererFromRenderWindow(*iren);
  if(mCurrentRenderer == NULL)
  {
    mMessageManager->sendError("Could not pick the point because the renderer is NULL.");
    return;
  }

  //Get camera focal point and position
  double cameraPos[4], cameraFocalPoint[4];
  vtkCamera* camera = mCurrentRenderer->GetActiveCamera();
  camera->ParallelProjectionOn();
  camera->GetPosition((double*)cameraPos);
  cameraPos[3] = 1.0;
  camera->GetFocalPoint((double*)cameraFocalPoint);
  cameraFocalPoint[3] = 1.0;

  //Get the cameras focal point in display coordinates.
  //We need a depth value for the z-buffer.
  double* displayCoords;
  mCurrentRenderer->SetWorldPoint(cameraFocalPoint[0], cameraFocalPoint[1], cameraFocalPoint[2], cameraFocalPoint[3]);
  mCurrentRenderer->WorldToDisplay();
  displayCoords = mCurrentRenderer->GetDisplayPoint();
  selectionZ = displayCoords[2];

  //Convert the selection point into world coordinates
  double* worldCoords;
  mCurrentRenderer->SetDisplayPoint(selectionX, selectionY, selectionZ);
  mCurrentRenderer->DisplayToWorld();
  worldCoords = mCurrentRenderer->GetWorldPoint();

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
    mMessageManager->sendError("Cannot process the picked point.");
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

  //Creating the line from the camera through the picked point
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
  this->showTemporaryPointSlot(skinPoint[0], skinPoint[1], skinPoint[2]);

  mCurrentX = skinPoint[0];
  mCurrentY = skinPoint[1];
  mCurrentZ = skinPoint[2];

  emit pointPicked(mCurrentX, mCurrentY, mCurrentZ);

  std::stringstream stream;
  stream << "Picked point: (";
  stream << mCurrentX;
  stream << ",";
  stream << mCurrentY;
  stream << ",";
  stream << mCurrentZ;
  stream << ")";
  mMessageManager->sendInfo(stream.str());

  iren->GetRenderWindow()->Render();
}
void VolumetricRep::makePointPermanent(unsigned int index)
{

  if(mCurrentX == std::numeric_limits<double>::max() &&
      mCurrentY == std::numeric_limits<double>::max() &&
      mCurrentY == std::numeric_limits<double>::max())
  {
    mMessageManager->sendWarning("Could not make the point permanent, invalid values.");
    return;
  }
  if(!this->doesLandmarkAlreadyExist(mCurrentX, mCurrentY, mCurrentZ))
    emit addPermanentPoint(mCurrentX, mCurrentY, mCurrentZ, index);
}
void VolumetricRep::pickSurfacePointSlot(vtkObject* object)
{
  this->pickSurfacePoint(object, mCurrentX, mCurrentY, mCurrentY);
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
    mMessageManager->sendError("Trying to add rep actors to view renderer, but view is NULL in volumetric rep.");
    return;
  }
  ssc::VolumetricRep::addRepActorsToViewRenderer(view);
  view->getRenderer()->ResetCamera();

  mConnections->Connect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
void VolumetricRep::showTemporaryPointSlot(double x, double y, double z)
{
  if(mCurrentRenderer == NULL)
  {
    mMessageManager->sendWarning("Could not determine what renderer to add the temporary point to. Try picking a point in the rep first.");
    return;
  }

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
  if(mCurrentRenderer->HasViewProp(mPickedPointActor))
      mCurrentRenderer->RemoveActor(mPickedPointActor);

  mPickedPointActor->SetPosition(x, y, z);
  mCurrentRenderer->AddActor(mPickedPointActor);

  mCurrentRenderer->GetRenderWindow()->Render();
}
void VolumetricRep::removeRepActorsFromViewRenderer(ssc::View* view)
{
  if(view == NULL)
  {
    mMessageManager->sendError("Trying to remove rep actors to view renderer, but view is NULL in volumetric rep.");
    return;
  }
  ssc::VolumetricRep::removeRepActorsFromViewRenderer(view);

  mConnections->Disconnect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickSurfacePointSlot(vtkObject*)));
}
bool VolumetricRep::doesLandmarkAlreadyExist(double x, double y, double z)
{
  if(mImage.get() == NULL)
    return true; //because then it woun't be added...

  vtkDoubleArrayPtr landmarks =  mImage->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  for(int row=1; row<=numberOfLandmarks; row++)
  {
    double* point = landmarks->GetTuple(row-1);
    if((x == point[0]) &&
       (y == point[1]) &&
       (z == point[2]))
    {
      mMessageManager->sendWarning("Landmark already exists! Skipping this one.");
      return true;
    }
  }
  return false;
}
}//namespace cx
