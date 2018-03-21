/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxGraphicalPrimitives.h"

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
#include "cxTypeConversions.h"
#include "cxBoundingBox3D.h"
#include "vtkArrowSource.h"
#include "vtkMatrix4x4.h"
#include "vtkCaptionActor2D.h"
#include "vtkTextProperty.h"
#include "cxVtkHelperClasses.h"
#include "vtkPolyDataNormals.h"

namespace cx
{


GraphicalGeometricBase::GraphicalGeometricBase(vtkPolyDataAlgorithmPtr source, vtkRendererPtr renderer)
{
	mProperty = vtkPropertyPtr::New();
	mActor = vtkActorPtr::New();
	mActor->SetProperty(mProperty);
}

GraphicalGeometricBase::~GraphicalGeometricBase()
{
	this->setRenderer(NULL);
}

void GraphicalGeometricBase::setSource(vtkPolyDataAlgorithmPtr source)
{
	mData = vtkPolyDataPtr();
	mSource = source;

	if (mSource)
		getMapper()->SetInputConnection(mSource->GetOutputPort());
	else
		getMapper()->SetInputConnection(NULL);
}

void GraphicalGeometricBase::setRenderer(vtkRendererPtr renderer)
{
	if (mRenderer)
		mRenderer->RemoveActor(mActor);

	mRenderer = renderer;

	if (mRenderer)
		mRenderer->AddActor(mActor);
}

void GraphicalGeometricBase::setVisibility(bool visible)
{
	mActor->SetVisibility(visible);
}


void GraphicalGeometricBase::setBackfaceCulling(bool val)
{
	mActor->GetProperty()->SetBackfaceCulling(val);
}

void GraphicalGeometricBase::setFrontfaceCulling(bool val)
{
	mActor->GetProperty()->SetFrontfaceCulling(val);
}

void GraphicalGeometricBase::setColor(double red, double green, double blue)
{
	mActor->GetProperty()->SetColor(red, green, blue);
}

void GraphicalGeometricBase::setColor(Vector3D color)
{
	mActor->GetProperty()->SetColor(color.begin());
}

void GraphicalGeometricBase::setPosition(Vector3D point)
{
	mActor->SetPosition(point.begin());
}

void GraphicalGeometricBase::setOpacity(double val)
{
	mActor->GetProperty()->SetOpacity(val);
}

void GraphicalGeometricBase::setUserMatrix(vtkMatrix4x4 *matrix)
{
	mActor->SetUserMatrix(matrix);
}

void GraphicalGeometricBase::setPointSize(int pointSize)
{
	if(pointSize<=0)
		return;
	mProperty->SetPointSize(pointSize);
}

void GraphicalGeometricBase::setScalarVisibility(bool show)
{
	getMapper()->SetScalarVisibility(show);
}

Vector3D GraphicalGeometricBase::getPosition() const
{
	return Vector3D(mActor->GetPosition());
}

vtkActorPtr GraphicalGeometricBase::getActor()
{
	return mActor;
}

vtkPropertyPtr GraphicalGeometricBase::getProperty()
{
	return mProperty;
}

vtkPolyDataPtr GraphicalGeometricBase::getPolyData()
{
	if (mSource)
		return mSource->GetOutput();
	else
		return mData;
}

vtkPolyDataAlgorithmPtr GraphicalGeometricBase::getSource()
{
	return mSource;
}

//--------------------------------------------------------
//--------------------------------------------------------
//-------------------------------------------------------

GraphicalPolyData3D::GraphicalPolyData3D(vtkPolyDataAlgorithmPtr source, vtkRendererPtr renderer) :
	GraphicalGeometricBase(source,renderer)
{
	mMapper =  vtkPolyDataMapperPtr::New();

	mActor->SetMapper(mMapper);
	setSource(source);
	setRenderer(renderer);
}

void GraphicalPolyData3D::setIsWireFrame(bool val)
{
	mActor->GetProperty()->SetRepresentationToWireframe();
}

void GraphicalPolyData3D::setData(vtkPolyDataPtr data)
{
	mSource = vtkPolyDataAlgorithmPtr();
	mData = data;

	mMapper->SetInputData(mData);
}

void GraphicalPolyData3D::setTexture(vtkTexturePtr texture)
{
	mActor->SetTexture(texture);
}


vtkMapperPtr GraphicalPolyData3D::getMapper()
{
	return mMapper;
}

//--------------------------------------------------------
//--------------------------------------------------------
//-------------------------------------------------------

GraphicalGlyph3DData::GraphicalGlyph3DData(vtkPolyDataAlgorithmPtr source, vtkRendererPtr renderer) :
	GraphicalGeometricBase(source,renderer)
{
	mMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
	vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
	mMapper->SetSourceConnection(arrowSource->GetOutputPort());
	mMapper->ScalarVisibilityOn();
	mMapper->SetUseLookupTableScalarRange(1);
	mMapper->SetScalarMode(VTK_SCALAR_MODE_USE_POINT_FIELD_DATA);
	mActor->SetMapper(mMapper);
	setSource(source);
	setRenderer(renderer);
}


void GraphicalGlyph3DData::setData(vtkPolyDataPtr data)
{
	mData = data;
	if (data)
		mMapper->SetInputData(mData);
}

void GraphicalGlyph3DData::setOrientationArray(const char* orientationArray)
{
	mMapper->SetOrientationArray(orientationArray);
}

void GraphicalGlyph3DData::setColorArray(const char* colorArray)
{
	if(strlen(colorArray)>0)
	{
		setScalarVisibility(true);
	}else
	{
		setScalarVisibility(false);
	}
	mMapper->SelectColorArray(colorArray);
}


void GraphicalGlyph3DData::setLUT(const char* lut)
{
	vtkSmartPointer<vtkColorSeries> colorSeries = vtkSmartPointer<vtkColorSeries>::New();
	vtkSmartPointer<vtkLookupTable> table = vtkLookupTable::New();
	colorSeries->SetColorSchemeByName(lut);
	colorSeries->BuildLookupTable(table , vtkColorSeries::ORDINAL);
	mMapper->SetLookupTable(table);
}


void GraphicalGlyph3DData::setScaleFactor(double scaleFactor)
{
	if(scaleFactor<=0) return;
	mMapper->SetScaleFactor(scaleFactor);
}

vtkMapperPtr GraphicalGlyph3DData::getMapper()
{
	return mMapper;
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

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

	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
	normals->SetInputConnection(source->GetOutputPort());
	normals->Update();

	mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(normals->GetOutputPort());

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

void GraphicalPoint3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
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

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

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

void GraphicalLine3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
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

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

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

void GraphicalArc3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
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

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

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

void GraphicalArrow3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
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
	Transform3D S = createTransformScale(Vector3D(length,1,1));
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

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

Rect3D::Rect3D(vtkRendererPtr renderer, QColor color)
{
	mRenderer = renderer;
	mapper = vtkPolyDataMapperPtr::New();
	actor = vtkActorPtr::New();
	setColorAndOpacity(actor->GetProperty(), color);
	actor->SetMapper(mapper);
	if (mRenderer)
		mRenderer->AddActor(actor);

	mPolyData = vtkPolyDataPtr::New();
	mPoints = vtkPointsPtr::New();
	mSide = vtkCellArrayPtr::New();

	vtkIdType cells[5] = { 0,1,2,3,0 };
	mSide->InsertNextCell(5, cells);

	mPolyData->SetPoints(mPoints);
	mapper->SetInputData(mPolyData);
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

void Rect3D::setColor(QColor color)
{
	setColorAndOpacity(actor->GetProperty(), color);
}

void Rect3D::updatePosition(const DoubleBoundingBox3D bb, const Transform3D& M)
{
	mPoints = vtkPointsPtr::New();
	mPoints->InsertPoint(0, M.coord(bb.corner(0,0,0)).begin());
	mPoints->InsertPoint(1, M.coord(bb.corner(0,1,0)).begin());
	mPoints->InsertPoint(2, M.coord(bb.corner(1,1,0)).begin());
	mPoints->InsertPoint(3, M.coord(bb.corner(1,0,0)).begin());
	mPolyData->SetPoints(mPoints);
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

FollowerText3D::FollowerText3D( vtkRendererPtr renderer)
{
	mText = vtkVectorTextPtr::New();
	vtkPolyDataMapperPtr mapper = vtkPolyDataMapperPtr::New();
	mapper->SetInputConnection(mText->GetOutputPort());
	mFollower = vtkFollowerPtr::New();
	mFollower->SetMapper(mapper);
	Vector3D mTextScale(2,2,2);
	mFollower->SetScale(mTextScale.begin());

	this->setSizeInNormalizedViewport(true, 0.025);
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
			mViewportListener.reset(new ViewportListener);
			mViewportListener->setCallback(boost::bind(&FollowerText3D::scaleText, this));
		}
	}
	else
	{
		mViewportListener.reset();
	}

	this->setSize(size);
}

void FollowerText3D::setColor(QColor color)
{
	setColorAndOpacity(mFollower->GetProperty(), color);
}

void FollowerText3D::setText(QString text)
{
	mText->SetText(cstring_cast(text));
}

void FollowerText3D::setPosition(Vector3D pos)
{
	mFollower->SetPosition(pos.begin());
}

vtkFollowerPtr FollowerText3D::getActor()
{
	return mFollower;
}

/** Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void FollowerText3D::scaleText()
{
	if (!mViewportListener || !mViewportListener->isListening())
	{
		mFollower->SetScale(Vector3D(mSize,mSize,mSize).begin());
		return;
	}

	double size = mViewportListener->getVpnZoom(Vector3D(mFollower->GetPosition()));

	double scale = mSize/size;
	//  std::cout << "s= " << size << "  ,scale= " << scale << std::endl;
	Vector3D mTextScale(scale,scale,scale);
	if (mFollower)
		mFollower->SetScale(mTextScale.begin());
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

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

void CaptionText3D::placeBelowCenter()
{
	mText->SetPosition(-15, -30);
	mText->SetPosition2(15, -10);
}

void CaptionText3D::placeAboveCenter()
{
	mText->SetPosition(-15, 2);
}

void CaptionText3D::setVisibility(bool visible)
{
	mText->SetVisibility(visible);
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

void CaptionText3D::setColor(QColor color)
{
	mText->GetCaptionTextProperty()->SetColor(getColorAsVector3D(color).begin());
}

void CaptionText3D::setText(QString text)
{
	mText->SetCaption(cstring_cast(text));
}

void CaptionText3D::setPosition(Vector3D pos)
{
	mPos = pos;
	mText->SetAttachmentPoint(pos.begin());
}

Vector3D CaptionText3D::getPosition() const
{
	return mPos;
}

vtkCaptionActor2DPtr CaptionText3D::getActor()
{
	return mText;
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------

}
