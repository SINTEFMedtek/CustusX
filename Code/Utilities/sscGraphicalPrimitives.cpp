#include "sscGraphicalPrimitives.h"

#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkArcSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkFollower.h>
#include <vtkVectorText.h>
#include <vtkCamera.h>
#include "sscTypeConversions.h"
#include "sscBoundingBox3D.h"

namespace ssc
{

GraphicalPoint3D::GraphicalPoint3D(vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkSphereSourcePtr::New();
	source->SetRadius(4);
//  default:
//  source->SetThetaResolution(8);
//  source->SetPhiResolution(8);
	// 24*16 = 384, 8*8=64, 16*12=192
	source->SetThetaResolution(16);
  source->SetPhiResolution(12);

	mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(source->GetOutputPort());

	actor = vtkActorPtr::New();
	actor->SetMapper(mapper);
	if (mRenderer)
	{
		mRenderer->AddActor(actor);
	}
}

GraphicalPoint3D::~GraphicalPoint3D()
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(actor);
	}
}

void GraphicalPoint3D::setRadius(double radius)
{
	source->SetRadius(radius);
}

void GraphicalPoint3D::setColor(Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
}

void GraphicalPoint3D::setValue(Vector3D point)
{
	actor->SetPosition(point.begin());
}

Vector3D GraphicalPoint3D::getValue() const
{
	return Vector3D(actor->GetPosition());
}

vtkActorPtr GraphicalPoint3D::getActor()
{
	return actor;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


GraphicalLine3D::GraphicalLine3D( vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkLineSourcePtr::New();
	mapper = vtkPolyDataMapperPtr::New() ;
	actor = vtkActorPtr::New() ;
	
	mapper->SetInputConnection( source->GetOutputPort() );
	actor->SetMapper (mapper );
	if (mRenderer)
		mRenderer->AddActor(actor);	
}

GraphicalLine3D::~GraphicalLine3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(actor);
}

void GraphicalLine3D::setColor(Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
}

void GraphicalLine3D::setValue(Vector3D point1, Vector3D point2)
{
	source->SetPoint1(point1.begin());
	source->SetPoint2(point2.begin());
}

void GraphicalLine3D::setStipple(int stipple)
{
	actor->GetProperty()->SetLineStipplePattern(stipple);
}

vtkActorPtr GraphicalLine3D::getActor()
{
	return actor;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


GraphicalArc3D::GraphicalArc3D( vtkRendererPtr renderer)
{
  mRenderer = renderer;
  source = vtkArcSourcePtr::New();
  source->SetResolution(20);
  mapper = vtkPolyDataMapperPtr::New() ;
  actor = vtkActorPtr::New() ;

  mapper->SetInputConnection( source->GetOutputPort() );
  actor->SetMapper (mapper );
  if (mRenderer)
    mRenderer->AddActor(actor);
}

GraphicalArc3D::~GraphicalArc3D()
{
  if (mRenderer)
    mRenderer->RemoveActor(actor);
}

void GraphicalArc3D::setColor(Vector3D color)
{
  actor->GetProperty()->SetColor(color.begin());
}

void GraphicalArc3D::setValue(Vector3D point1, Vector3D point2, Vector3D center)
{
  source->SetPoint1(point1.begin());
  source->SetPoint2(point2.begin());
  source->SetCenter(center.begin());
}

void GraphicalArc3D::setStipple(int stipple)
{
  actor->GetProperty()->SetLineStipplePattern(stipple);
}

vtkActorPtr GraphicalArc3D::getActor()
{
  return actor;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Rect3D::Rect3D(vtkRendererPtr renderer, Vector3D color)
{
  mRenderer = renderer;
  mapper = vtkPolyDataMapperPtr::New();
  actor = vtkActorPtr::New();
  actor->GetProperty()->SetColor(color.begin());
  actor->SetMapper(mapper);
  if (mRenderer)
    mRenderer->AddActor(actor);

  mPolyData = vtkPolyDataPtr::New();
  mPoints = vtkPointsPtr::New();
  mSide = vtkCellArrayPtr::New();

  vtkIdType cells[5] = { 0,1,2,3,0 };
  mSide->InsertNextCell(5, cells);

  mPolyData->SetPoints(mPoints);
  mapper->SetInput(mPolyData);
}

void Rect3D::setLine(bool on, int width)
{
  if (on)
  {
    mPolyData->SetLines(mSide);
    actor->GetProperty()->SetLineWidth(width);
  }
  else
  {
    mPolyData->SetLines(NULL);
  }
}

void Rect3D::setSurface(bool on)
{
  if (on)
  {
    mPolyData->SetPolys(mSide);
    actor->GetProperty()->SetOpacity(1.0); // transparent planes dont work well together with texture volume. Use 1.0
  }
  else
  {
    mPolyData->SetPolys(NULL);
  }
}

Rect3D::~Rect3D()
{
  if (mRenderer)
    mRenderer->RemoveActor(actor);
}

void Rect3D::updatePosition(const DoubleBoundingBox3D bb, const Transform3D& M)
{
  mPoints = vtkPointsPtr::New();
  mPoints->InsertPoint(0, M.coord(bb.corner(0,0,0)).begin());
  mPoints->InsertPoint(1, M.coord(bb.corner(0,1,0)).begin());
  mPoints->InsertPoint(2, M.coord(bb.corner(1,1,0)).begin());
  mPoints->InsertPoint(3, M.coord(bb.corner(1,0,0)).begin());
  mPolyData->SetPoints(mPoints);
  mPolyData->Update();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

//namespace // unnamed
//{

	class FollowerText3DCallback : public vtkCommand
	{
	public:
		FollowerText3DCallback() {}
		static FollowerText3DCallback* New() {return new FollowerText3DCallback;}
		void SetBase(FollowerText3D* rep) {mBase = rep;}
		virtual void Execute(vtkObject* caller, unsigned long, void*)
		{
			if (mBase)
				mBase->scaleText();
		}
		FollowerText3D* mBase;
	};

//}

FollowerText3D::FollowerText3D( vtkRendererPtr renderer)
{
  mRenderer = renderer;
  if (!mRenderer)
  	return;

  mText = vtkVectorText::New();
  vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
  mapper->SetInput(mText->GetOutput());
  mFollower = vtkFollower::New();
  mFollower->SetMapper(mapper);
  mFollower->SetCamera(mRenderer->GetActiveCamera());
  ssc::Vector3D mTextScale(2,2,2);
  mFollower->SetScale(mTextScale.begin());

  mRenderer->AddActor(mFollower);
  this->setSizeInNormalizedViewport(true, 0.025);
}

FollowerText3D::~FollowerText3D()
{
  if (mRenderer)
    mRenderer->RemoveActor(mFollower);

  this->setSizeInNormalizedViewport(false,1);
}

void FollowerText3D::setSize(double val)
{
	mSize = val;
	this->scaleText();
}

void FollowerText3D::setSizeInNormalizedViewport(bool on, double size)
{
	if (on)
	{
		// turn on observer
		if (!mMetricCallback)
		{
		  mMetricCallback = FollowerText3DCallbackPtr::New();
		  mMetricCallback->SetBase(this);
		//  mView->getRenderer()->GetActiveCamera()->AddObserver(vtkCommand::ModifiedEvent, DistanceMetricCallback::New());
		  mRenderer->AddObserver(vtkCommand::ModifiedEvent, mMetricCallback);
		  mRenderer->AddObserver(vtkCommand::ActiveCameraEvent, mMetricCallback);
		  mRenderer->AddObserver(vtkCommand::ResetCameraEvent, mMetricCallback);
		}
	}
	else
	{
		// turn off observer
	  if (mMetricCallback)
	  {
	    mMetricCallback->SetBase(NULL);
	    mRenderer->RemoveObserver(mMetricCallback);
	  }
	}

	this->setSize(size);
}

void FollowerText3D::setColor(Vector3D color)
{
	mFollower->GetProperty()->SetColor(color.begin());
}

void FollowerText3D::setText(QString text)
{
  mText->SetText(cstring_cast(text));
}

void FollowerText3D::setPosition(ssc::Vector3D pos)
{
  mFollower->SetPosition(pos.begin());
}

vtkFollowerPtr FollowerText3D::getActor()
{
  return mFollower;
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void FollowerText3D::scaleText()
{
	if (!mMetricCallback)
	{
    mFollower->SetScale(ssc::Vector3D(mSize,mSize,mSize).begin());
		return;
	}

//  double targetSize = 0.025;// relative to vp height

  //  use the focal point and focal point + vup.
  //  Transform both to view space and remove z-coord.
  //  The distance between then in the view plane can
  //  be used to rescale the text.
  vtkCameraPtr camera = mRenderer->GetActiveCamera();
  ssc::Vector3D p_f(camera->GetFocalPoint());
  ssc::Vector3D vup(camera->GetViewUp());
  ssc::Vector3D p_fup = p_f+vup;
  mRenderer->WorldToView(p_f[0],p_f[1],p_f[2]);
  mRenderer->WorldToView(p_fup[0],p_fup[1],p_fup[2]);
  p_f[2] = 0;
  p_fup[2] = 0;
  double size = (p_f - p_fup).length()/2;
  double scale = mSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;
  ssc::Vector3D mTextScale(scale,scale,scale);
  if (mFollower)
    mFollower->SetScale(mTextScale.begin());

//	std::cout << "FollowerText3D::scaleText() " << scale << " " << size << std::endl;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

}
