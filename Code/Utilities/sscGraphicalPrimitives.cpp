#include "sscGraphicalPrimitives.h"

#include "boost/bind.hpp"
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
#include "vtkArrowSource.h"
#include "vtkMatrix4x4.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"

namespace ssc
{

GraphicalPolyData3D::GraphicalPolyData3D(vtkPolyDataAlgorithmPtr source, vtkRendererPtr renderer)
{
	mMapper = vtkPolyDataMapperPtr::New();

	mActor = vtkActorPtr::New();
	mActor->SetMapper(mMapper);

	this->setSource(source);
	this->setRenderer(renderer);
}

void GraphicalPolyData3D::setSource(vtkPolyDataAlgorithmPtr source)
{
	mSource = source;

	if (mSource)
		mMapper->SetInputConnection(mSource->GetOutputPort());
	else
		mMapper->SetInputConnection(NULL);
}

GraphicalPolyData3D::~GraphicalPolyData3D()
{
	this->setRenderer(NULL);
}

void GraphicalPolyData3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
		mRenderer->RemoveActor(mActor);

	mRenderer = renderer;

	if (mRenderer)
		mRenderer->AddActor(mActor);
}


void GraphicalPolyData3D::setColor(Vector3D color)
{
	mActor->GetProperty()->SetColor(color.begin());
}

void GraphicalPolyData3D::setPosition(Vector3D point)
{
	mActor->SetPosition(point.begin());
}

Vector3D GraphicalPolyData3D::getPosition() const
{
	return Vector3D(mActor->GetPosition());
}

vtkActorPtr GraphicalPolyData3D::getActor()
{
	return mActor;
}

vtkPolyDataPtr GraphicalPolyData3D::getPolyData()
{
	return mSource->GetOutput();
}

vtkPolyDataAlgorithmPtr GraphicalPolyData3D::getSource()
{
	return mSource;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

GraphicalPoint3D::GraphicalPoint3D(vtkRendererPtr renderer)
{
//	mRenderer = renderer;
	source = vtkSphereSourcePtr::New();
	source->SetRadius(4);
//  default:
//  source->SetThetaResolution(8);
//  source->SetPhiResolution(8);
	// 24*16 = 384, 8*8=64, 16*12=192
	source->SetThetaResolution(16);
	source->SetPhiResolution(12);
	source->LatLongTessellationOn();
	mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(source->GetOutputPort());

	actor = vtkActorPtr::New();
	actor->SetMapper(mapper);

	this->setRenderer(renderer);
}

GraphicalPoint3D::~GraphicalPoint3D()
{
	this->setRenderer(NULL);
}

void GraphicalPoint3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(actor);
	}

	mRenderer = renderer;

	if (mRenderer)
	{
		mRenderer->AddActor(actor);
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

vtkPolyDataPtr GraphicalPoint3D::getPolyData()
{
	return source->GetOutput();
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
	this->setRenderer(renderer);
}

GraphicalLine3D::~GraphicalLine3D()
{
	this->setRenderer(NULL);
}

void GraphicalLine3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(actor);
	}

	mRenderer = renderer;

	if (mRenderer)
	{
		mRenderer->AddActor(actor);
	}
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

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


GraphicalArrow3D::GraphicalArrow3D( vtkRendererPtr renderer)
{
	mRenderer = renderer;
	source = vtkArrowSourcePtr::New();
	source->SetTipResolution(24);
	source->SetShaftResolution(24);
	mapper = vtkPolyDataMapperPtr::New() ;
	actor = vtkActorPtr::New() ;

	mapper->SetInputConnection( source->GetOutputPort() );
	actor->SetMapper (mapper );
	if (mRenderer)
		mRenderer->AddActor(actor);
}

GraphicalArrow3D::~GraphicalArrow3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(actor);
}

void GraphicalArrow3D::setColor(Vector3D color)
{
	actor->GetProperty()->SetColor(color.begin());
}

void GraphicalArrow3D::setValue(Vector3D base, Vector3D normal, double length)
{
	// find an arbitrary vector k perpendicular to normal:
	Vector3D k = cross(Vector3D(1,0,0), normal);
	if (similar(k, Vector3D(0,0,0)))
		k = cross(Vector3D(0,1,0), normal);
	k = k.normalized();
	Transform3D M = createTransformIJC(normal, k, base);

//	std::cout << "GraphicalArrow3D::setValue  " << base << " - " << normal << std::endl;
	Transform3D S = createTransformScale(ssc::Vector3D(length,1,1));
	M = M * S;
	// let arrow shape increase slowly with length:
//	source->SetTipLength(0.35/sqrt(length));
//	source->SetTipRadius(0.1*sqrt(length));
//	source->SetShaftRadius(0.03*sqrt(length));
	source->SetTipLength(0.35);
	source->SetTipRadius(0.1*(length));
	source->SetShaftRadius(0.03*(length));
	actor->SetUserMatrix(M.getVtkMatrix());
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


FollowerText3D::FollowerText3D( vtkRendererPtr renderer)
{
//  mRenderer = renderer;
//  if (!mRenderer)
//  	return;

	mText = vtkVectorTextPtr::New();
	vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInput(mText->GetOutput());
	mFollower = vtkFollowerPtr::New();
	mFollower->SetMapper(mapper);
//  mFollower->SetCamera(mRenderer->GetActiveCamera());
	ssc::Vector3D mTextScale(2,2,2);
	mFollower->SetScale(mTextScale.begin());

	this->setSizeInNormalizedViewport(true, 0.025);
//  mRenderer->AddActor(mFollower);
	this->setRenderer(renderer);
}

void FollowerText3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(mFollower);
		mViewportListener->stopListen();
	}

	mRenderer = renderer;

	if (mRenderer)
	{
		mRenderer->AddActor(mFollower);
		mFollower->SetCamera(mRenderer->GetActiveCamera());
		if (mViewportListener)
			mViewportListener->startListen(mRenderer);
	}
}

FollowerText3D::~FollowerText3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(mFollower);
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
		if (!mViewportListener)
		{
			mViewportListener.reset(new ssc::ViewportListener);
			mViewportListener->setCallback(boost::bind(&FollowerText3D::scaleText, this));
		}
	}
	else
	{
		mViewportListener.reset();
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
	if (!mViewportListener || !mViewportListener->isListening())
	{
		mFollower->SetScale(ssc::Vector3D(mSize,mSize,mSize).begin());
		return;
	}

	double size = mViewportListener->getVpnZoom();

	double scale = mSize/size;
//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;
	ssc::Vector3D mTextScale(scale,scale,scale);
	if (mFollower)
		mFollower->SetScale(mTextScale.begin());
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------


CaptionText3D::CaptionText3D( vtkRendererPtr renderer)
{
	mText = vtkCaptionActor2DPtr::New();
	mText->BorderOff();
	mText->LeaderOff();

	mText->GetCaptionTextProperty()->BoldOff();
	mText->GetCaptionTextProperty()->ItalicOff();
	mText->GetCaptionTextProperty()->ShadowOff();
	mText->SetWidth(10);
	mText->SetHeight(0.03);

	this->setRenderer(renderer);
}

void CaptionText3D::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
	{
		mRenderer->RemoveActor(mText);
	}

	mRenderer = renderer;

	if (mRenderer)
	{
		mRenderer->AddActor(mText);
	}
}

CaptionText3D::~CaptionText3D()
{
	if (mRenderer)
		mRenderer->RemoveActor(mText);
}

void CaptionText3D::setSize(double val)
{
	mText->SetHeight(val);
}

void CaptionText3D::setColor(Vector3D color)
{
	mText->GetCaptionTextProperty()->SetColor(color.begin());
}

void CaptionText3D::setText(QString text)
{
	mText->SetCaption(cstring_cast(text));
}

void CaptionText3D::setPosition(ssc::Vector3D pos)
{
	mText->SetAttachmentPoint(pos.begin());
}

vtkCaptionActor2DPtr CaptionText3D::getActor()
{
	return mText;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

}
