#include "sscProbeRep.h"

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
ProbeRepPtr ProbeRep::New(const QString& uid, const QString& name)
{
	ProbeRepPtr retval(new ProbeRep(uid, name));
	retval->mSelf = retval;
	return retval;
}
ProbeRep::ProbeRep(const QString& uid, const QString& name) :
	RepImpl(uid, name),
	mThreshold(25),
	mResolution(1000),
	mPickedPoint(),
	mPickedPointActor(NULL),
	mConnections(vtkEventQtSlotConnectPtr::New())
{}
ProbeRep::~ProbeRep()
{}
QString ProbeRep::getType() const
{
	return "ssc::ProbeRep";
}
int ProbeRep::getThreshold()
{
  return mThreshold;
}
ImagePtr ProbeRep::getImage()
{
  return mImage;
}

void ProbeRep::setImage(ImagePtr image)
{
	if (image==mImage)
		return;

//	if (mImage)
//	{
//		disconnect(this, SIGNAL(addPermanentPoint(double, double, double, unsigned int)),
//				mImage.get(), SLOT(addLandmarkSlot(double, double, double, unsigned int)));
//	}
	mImage = image;
//  if (mImage)
//  {
//    mThreshold = (mImage->getPosMax()-mImage->getPosMin())/10;
//    connect(this, SIGNAL(addPermanentPoint(double, double, double, unsigned int)),
//        mImage.get(), SLOT(addLandmarkSlot(double, double, double, unsigned int)));
//  }
	if (mImage)
	  mThreshold = mImage->getPosMin() + (mImage->getPosMax()-mImage->getPosMin())/10;
}
void ProbeRep::setResolution(const int resolution)
{
	mResolution = resolution;
}
void ProbeRep::setTool(ToolPtr tool)
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
Vector3D ProbeRep::pickLandmark(const Vector3D& clickPosition, vtkRendererPtr renderer)
{
	//std::cout << "ProbeRep::pickLandmark" << std::endl;
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
	//std::cout << "ProbeRep::pickLandmark: click:<"<<clickPosition<<"> world:<"<< worldClickPoint<<">"<< std::endl;

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

	//TODO: we need to know the result of this outside this function, so we know
	//if the point should be made permanent or not
	//bool snapped = snapToExistingPoints(p0, p1, intersection);

	//set the last point picked
	//moved to showTemporaryPointSlot(..)
	/*mPickedPoint = intersection;
	emit pointPicked(mPickedPoint[0], mPickedPoint[1], mPickedPoint[2]);*/

	//Make an sphere actor to show where the calculated point is
	this->showTemporaryPointSlot(intersection[0], intersection[1], intersection[2]);
  
  emit pointPicked(mPickedPoint[0], mPickedPoint[1], mPickedPoint[2]);
	return intersection;
}
/**
 * \param[in] index the index you want to give the landmark
 */
void ProbeRep::makeLandmarkPermanent(unsigned index)
{
	emit addPermanentPoint(mPickedPoint[0],
						   mPickedPoint[1],
						   mPickedPoint[2],
						   index);
}
void ProbeRep::pickLandmarkSlot(vtkObject* renderWindowInteractor)
{
  //std::cout << "ProbeRep::pickLandmarkSlot" << std::endl;
	vtkRenderWindowInteractorPtr iren =
		vtkRenderWindowInteractor::SafeDownCast(renderWindowInteractor);

	if(iren == NULL)
		return;
	if (!mImage)
	  return;

	int pickedPoint[2]; //<x,y>
	iren->GetEventPosition(pickedPoint); //mouse positions are measured in pixels

	mCurrentRenderer = this->getRendererFromRenderWindow(*iren);
	if(mCurrentRenderer == NULL)
		return;

//  std::cout << "ProbeRep::pickLandmarkSlot-2" << std::endl;
	Vector3D clickPoint(pickedPoint[0], pickedPoint[1], 0);
  //std::cout << "ProbeRep::pickLandmarkSlot: screenpos = " << clickPoint << std::endl;
	this->pickLandmark(clickPoint, mCurrentRenderer);
}
/**
 * @param x world coordinat, ref space
 * @param y world coordinat, ref space
 * @param z world coordinat, ref space
 */
void ProbeRep::showTemporaryPointSlot(double x, double y, double z)
{
  if(mCurrentRenderer == NULL)
    return;

  if(mPickedPointActor == NULL )
  {
    vtkSphereSourcePtr pickedPointSphereSource = vtkSphereSourcePtr::New();
    pickedPointSphereSource->SetRadius(2);
    vtkPolyDataMapperPtr pickedPointMapper = vtkPolyDataMapperPtr::New();
    pickedPointMapper->SetInputConnection(pickedPointSphereSource->GetOutputPort());
    mPickedPointActor = vtkActorPtr::New();
    mPickedPointActor->SetMapper(pickedPointMapper);
    mPickedPointActor->GetProperty()->SetColor(0,0,1);
  }
  if(mCurrentRenderer->HasViewProp(mPickedPointActor))
      mCurrentRenderer->RemoveActor(mPickedPointActor);

  mPickedPointActor->SetPosition(x, y, z);
  mCurrentRenderer->AddActor(mPickedPointActor);

  //mCurrentRenderer->GetRenderWindow()->Render();

  //update temporary point
  mPickedPoint[0] = x;
  mPickedPoint[1] = y;
  mPickedPoint[2] = z;
  //std::cout << "ProbeRep::showTemporaryPointSlot: pickpos = " << mPickedPoint << std::endl;
//  std::cout << "ProbeRep::showTemporaryPointSlot E" << std::endl;
}
/**
 * @param threshold sets a threshold for the probing ray
 */
void ProbeRep::setThresholdSlot(const int threshold)
{
  mThreshold = threshold;
}
void ProbeRep::receiveTransforms(Transform3D prMt, double timestamp)
{
  Transform3DPtr rMprPtr = ToolManager::getInstance()->get_rMpr();
  Transform3D rMt = (*rMprPtr)*prMt;
  this->showTemporaryPointSlot(rMt[0][3], rMt[1][3], rMt[2][3]);
}
void ProbeRep::addRepActorsToViewRenderer(View* view)
{
  if(view == NULL)
  {
    messageManager()->sendDebug("Cannot add rep actor to a NULL view.");
    return;
  }

  mConnections->Connect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickLandmarkSlot(vtkObject*)));
 // std::cout << "ProbeRep::addRepActorsToViewRenderer"<< std::endl;
}
void ProbeRep::removeRepActorsFromViewRenderer(View* view)
{
  if(view == NULL)
    return;

  mConnections->Disconnect(view->GetRenderWindow()->GetInteractor(),
                       vtkCommand::LeftButtonPressEvent,
                       this,
                       SLOT(pickLandmarkSlot(vtkObject*)));
 // std::cout << "ProbeRep::removeRepActorsFromViewRenderer"<< std::endl;
}
vtkRendererPtr ProbeRep::getRendererFromRenderWindow(vtkRenderWindowInteractor& iren)
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
bool ProbeRep::intersectData(Vector3D p0, Vector3D p1, Vector3D& intersection)
{
 // std::cout << "ProbeRep::intersectData(p0<"<<p0<<">,p1<"<<p1<<">) image="<<mImage->getName()<<std::endl;
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

	double value = -1;
	int i;
	for (i=0; i<dataSetAttribute->GetScalars()->GetNumberOfTuples(); i++)
	{
		dataSetAttribute->GetScalars()->GetTuple(i, &value);
		if (value > mThreshold)
			break;
	}
	if (i==dataSetAttribute->GetScalars()->GetNumberOfTuples())
		return false;

	Vector3D retval(probeFilterMapper->GetInput()->GetPoint(i));
	retval = dMr.inv().coord(retval);
	intersection = retval;
	return true;
}
/**
 * \warning NOT IMPLEMENTED!!!
 * \param[in] p0 start point for the probe line
 * \param[in] p1 end point for the probe line
 * \param[in/out] bestPoint send in the point from the intersection, get out an
 * existing point if one is close by
 * \return whether or not the incoming bestPoint was replaced
 */
bool ProbeRep::snapToExistingPoint(const Vector3D& p0, const Vector3D& p1, Vector3D& bestPoint)
{
	return false;
//	Vector3D tangent = (p1-p0).normal(); //ray tangent
//	vtkDoubleArrayPtr existingLandmarks = mImage->getLandmarks();
//
//	for(int i=0; i<= existingLandmarks->GetNumberOfTuples()-1; i++)
//	{
//		//distance from p0 to the currently best point
//		double distanceStartPointToBestPoint = dot((bestPoint-p0), tangent);
//
//		//distance from p0 to this existing landmark
//		double* landmark = existingLandmarks->GetTuple(i);
//		Vector3D existingLandmark(landmark[0], landmark[1], landmark[3]);
//		double distanceStartPointToExistingPoint = dot((existingLandmark-p0), tangent);
//
//		//TODO continue implementing
//		//incoming point projected onto ray
//		//Vector3D
//	}
//
}

Vector3D ProbeRep::getPosition() const
{
  return mPickedPoint;
}


}//namespace ssc
