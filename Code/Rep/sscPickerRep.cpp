#include "sscPickerRep.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkImageData.h>
#include <vtkLineSource.h>
//#include <vtkDoubleArray.h>
#include <vtkProbeFilter.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>
#include <vtkPolyDataMapper.h>
//#include <vtkDataSetAttributes.h>
#include <vtkEventQtSlotConnect.h>

#include "sscMessageManager.h"
#include "sscImage.h"
#include "sscView.h"
#include "sscTool.h"
#include "sscToolManager.h"

namespace ssc
{
PickerRepPtr PickerRep::New(const QString& uid, const QString& name)
{
	PickerRepPtr retval(new PickerRep(uid, name));
	retval->mSelf = retval;
	return retval;
}
PickerRep::PickerRep(const QString& uid, const QString& name) :
	RepImpl(uid, name),
	mThreshold(25),
	mResolution(1000),
	mPickedPoint(),
  mSphereRadius(2),
	mConnections(vtkEventQtSlotConnectPtr::New())
{
  mViewportListener.reset(new ssc::ViewportListener);
  mViewportListener->setCallback(boost::bind(&PickerRep::scaleSphere, this));

  mView = NULL;
  mEnabled = false;
  mConnected = false;
}

void PickerRep::scaleSphere()
{
  if (!mGraphicalPoint)
    return;

  double size = mViewportListener->getVpnZoom();
  double sphereSize = mSphereRadius/100/size;
  mGraphicalPoint->setRadius(sphereSize);
}

PickerRep::~PickerRep()
{}
QString PickerRep::getType() const
{
	return "ssc::PickerRep";
}
int PickerRep::getThreshold()
{
  return mThreshold;
}
ImagePtr PickerRep::getImage()
{
  return mImage;
}

void PickerRep::setSphereRadius(double radius)
{
  mSphereRadius = radius;
  if (mGraphicalPoint)
    mGraphicalPoint->setRadius(mSphereRadius);
}

void PickerRep::setImage(ImagePtr image)
{
	if (image==mImage)
		return;
	mImage = image;
	if (mImage)
	  mThreshold = mImage->getMin() + (mImage->getMax()-mImage->getMin())/10;
}

void PickerRep::setResolution(const int resolution)
{
	mResolution = resolution;
}

void PickerRep::setTool(ToolPtr tool)
{
  if (tool==mTool)
    return;
  
  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
               this, SLOT(receiveTransforms(Transform3D, double)));
  }
  
  mTool = tool;
  
  if (mTool)
  {
    receiveTransforms(mTool->get_prMt(), 0);
    
    connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)),
            this, SLOT(receiveTransforms(Transform3D, double)));     
  }
}
  
/**
 * Trace a ray from clickPosition along the camera view direction and intersect
 * the image.
 * \param[in] clickPosition the click position in DISPLAY coordinates
 * \param[in] renderer the renderer from which to get the camera
 * \return the point where the ray intersects the image
 */
Vector3D PickerRep::pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer)
{
	//std::cout << "PickerRep::pickLandmark" << std::endl;
	//Get camera position and focal point in world coordinates
	vtkCamera* camera = renderer->GetActiveCamera();
	Vector3D cameraPosition(camera->GetPosition());
	Vector3D cameraFocalPoint(camera->GetFocalPoint());
	Vector3D viewDirection = (cameraFocalPoint - cameraPosition).normal();

	//Get the cameras focal point in display coordinates.
	//We need a depth value for the z-buffer.
	double* displayCoords;
	renderer->SetWorldPoint(cameraFocalPoint[0], cameraFocalPoint[1],
			cameraFocalPoint[2], 1);
	renderer->WorldToDisplay();
	displayCoords = renderer->GetDisplayPoint();
	Vector3D displayClickPosition = clickPosition;
	displayClickPosition[2] = displayCoords[2];

	//Convert the click point into world coordinates
	double* worldCoords;
	renderer->SetDisplayPoint(displayClickPosition[0], displayClickPosition[1], displayClickPosition[2]);
	renderer->DisplayToWorld();
	worldCoords = renderer->GetWorldPoint();
	Vector3D worldClickPoint(worldCoords[0]/worldCoords[3],
							 worldCoords[1]/worldCoords[3],
							 worldCoords[2]/worldCoords[3]);
	//std::cout << "PickerRep::pickLandmark: click:<"<<clickPosition<<"> world:<"<< worldClickPoint<<">"<< std::endl;

	//Compute the direction of the probe ray
	Vector3D probeRayDirection;
	if (camera->GetParallelProjection()) // for parallel: use the same direction anyway.
		probeRayDirection = viewDirection;
	else
		probeRayDirection = (worldClickPoint - cameraPosition).normal(); // for projections

	//Compute the probe rays endpoints, where the ray intersects the front and
	//back clipping plane of the camera
	double* clipRange = camera->GetClippingRange(); //front and back Z value <z0,z1>
	double cam2click = dot(worldClickPoint - cameraPosition, probeRayDirection);
	Vector3D p0 = worldClickPoint + probeRayDirection * (clipRange[0] - cam2click);
	Vector3D p1 = worldClickPoint + probeRayDirection * (clipRange[1] - cam2click);

	Vector3D intersection;
	if (!this->intersectData(p0, p1, intersection))
	{
	  return intersection;
	}

//	//Make an sphere actor to show where the calculated point is
//	this->showTemporaryPointSlot(intersection[0], intersection[1], intersection[2]);
  
  mPickedPoint = intersection;
  if (mGraphicalPoint)
    mGraphicalPoint->setValue(mPickedPoint);


  emit pointPicked(mPickedPoint);
	return intersection;
}

///**
// * \param[in] index the index you want to give the landmark
// */
//void PickerRep::makeLandmarkPermanent(unsigned index)
//{
//	emit addPermanentPoint(mPickedPoint[0],
//						   mPickedPoint[1],
//						   mPickedPoint[2],
//						   index);
//}

void PickerRep::pickLandmarkSlot(vtkObject* renderWindowInteractor)
{
  //std::cout << "PickerRep::pickLandmarkSlot" << std::endl;
	vtkRenderWindowInteractorPtr iren = vtkRenderWindowInteractor::SafeDownCast(renderWindowInteractor);

	if(iren == NULL)
		return;
	if (!mImage)
	  return;

	int pickedPoint[2]; //<x,y>
	iren->GetEventPosition(pickedPoint); //mouse positions are measured in pixels

	vtkRendererPtr renderer = this->getRendererFromRenderWindow(*iren);
	if(renderer == NULL)
		return;

	Vector3D clickPoint(pickedPoint[0], pickedPoint[1], 0);
	this->pickLandmark(clickPoint, renderer);
}

///**
// * @param x world coordinat, ref space
// * @param y world coordinat, ref space
// * @param z world coordinat, ref space
// */
//void PickerRep::showTemporaryPointSlot(double x, double y, double z)
//{
//  mPickedPoint = Vector3D(x,y,z);
//  if (mGraphicalPoint)
//    mGraphicalPoint->setValue(mPickedPoint);
//}

/**
 * @param threshold sets a threshold for the picking ray
 */
void PickerRep::setThresholdSlot(const int threshold)
{
  mThreshold = threshold;
}

void PickerRep::receiveTransforms(Transform3D prMt, double timestamp)
{
  Transform3D rMpr = *ToolManager::getInstance()->get_rMpr();
  Transform3D rMt = rMpr*prMt;
  Vector3D p_r = rMt.coord(ssc::Vector3D(0,0,mTool->getTooltipOffset()));

//  this->showTemporaryPointSlot(rMt(0,3), rMt(1,3), rMt(2,3));

  mPickedPoint = p_r;
  if (mGraphicalPoint)
    mGraphicalPoint->setValue(mPickedPoint);

}

void PickerRep::setEnabled(bool on)
{
//  std::cout << "PickerRep::setEnabled " << on << std::endl;

  if (mEnabled==on)
    return;

  mEnabled = on;

  if (mEnabled)
  {
    this->connectInteractor();
    if (mGraphicalPoint)
      mGraphicalPoint->getActor()->SetVisibility(true);
  }
  else
  {
    this->disconnectInteractor();
    if (mGraphicalPoint)
      mGraphicalPoint->getActor()->SetVisibility(false);
  }
}


void PickerRep::connectInteractor()
{
  if (!mView)
    return;
  if (mConnected)
    return;
  mConnections->Connect(mView->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickLandmarkSlot(vtkObject*)));
  mConnected = true;
}

void PickerRep::disconnectInteractor()
{
  if (!mView)
    return;
  if (!mConnected)
    return;
  mConnections->Disconnect(mView->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickLandmarkSlot(vtkObject*)));
  mConnected = false;
}


void PickerRep::addRepActorsToViewRenderer(View* view)
{
  if(view == NULL)
  {
    messageManager()->sendDebug("Cannot add rep actor to a NULL view.");
    return;
  }

  if (mEnabled)
    this->connectInteractor();

  mView = view;

  mGraphicalPoint.reset(new ssc::GraphicalPoint3D(mView->getRenderer()));
  mGraphicalPoint->setColor(ssc::Vector3D(0,0,1));
  mGraphicalPoint->setRadius(mSphereRadius);
  mGraphicalPoint->getActor()->SetVisibility(false);

  mViewportListener->startListen(mView->getRenderer());
  this->scaleSphere();
}

void PickerRep::removeRepActorsFromViewRenderer(View* view)
{
  if(view == NULL)
    return;

  this->disconnectInteractor();
  mViewportListener->stopListen();
  mGraphicalPoint.reset();
  mView = NULL;
}

vtkRendererPtr PickerRep::getRendererFromRenderWindow(vtkRenderWindowInteractor& iren)
{
	vtkRendererPtr renderer = NULL;
	std::set<ssc::View*>::const_iterator it = mViews.begin();
	for(; it != mViews.end(); ++it)
	{
		if(iren.GetRenderWindow() == (*it)->getRenderWindow())
		renderer = (*it)->getRenderer();
	}
	return renderer;
}
/**
 * \param[in] p0 start point for the probe line
 * \param[in] p1 end point for the probe line
 * \param[out] intersection the point where the probeline intersects the image
 * \return whether or not the a intersection point was found
 */
bool PickerRep::intersectData(Vector3D p0, Vector3D p1, Vector3D& intersection)
{
 // std::cout << "PickerRep::intersectData(p0<"<<p0<<">,p1<"<<p1<<">) image="<<mImage->getName()<<std::endl;
	//Creating the line from the camera through the picked point into the volume
  ssc::Transform3D dMr = mImage->get_rMd().inv();
  p0 = dMr.coord(p0);
  p1 = dMr.coord(p1);
	vtkLineSourcePtr lineSource = vtkLineSourcePtr::New();
	lineSource->SetPoint1(p0.begin());
	lineSource->SetPoint2(p1.begin());
	lineSource->SetResolution(mResolution);
	lineSource->Update();

	//Creating a probefilter
	vtkProbeFilterPtr probeFilter = vtkProbeFilterPtr::New();
	//probeFilter->SetSource(mImage->getRefVtkImageData());
	probeFilter->SetSource(mImage->getBaseVtkImageData());
	probeFilter->SetInputConnection(lineSource->GetOutputPort());
	probeFilter->Update();

	//Mapper for the probe filter
	vtkPolyDataMapperPtr probeFilterMapper = vtkPolyDataMapperPtr::New();
	probeFilterMapper->SetInput(probeFilter->GetPolyDataOutput());

	//Aske the probe filter for point data, find the first one above a threshold.
	//That is the skin point
	vtkDataSetAttributesPtr dataSetAttribute = vtkDataSetAttributesPtr::New();
	dataSetAttribute = (vtkDataSetAttributes*)(probeFilterMapper->GetInput()->GetPointData());

  double value = -1000000;
  int i;
  for (i=0; i<dataSetAttribute->GetScalars()->GetNumberOfTuples(); i++)
  {
    dataSetAttribute->GetScalars()->GetTuple(i, &value);
    //The VTK structure is padded with zeroes
    //Ugly fix for signed: Ignore 0
    if ((value != 0) && (value > mThreshold))
      break;
  }
	if (i==dataSetAttribute->GetScalars()->GetNumberOfTuples())
		return false;

	Vector3D retval(probeFilterMapper->GetInput()->GetPoint(i));
	retval = dMr.inv().coord(retval);
	intersection = retval;
	return true;
}

Vector3D PickerRep::getPosition() const
{
  return mPickedPoint;
}


}//namespace ssc
