// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscToolRep3D.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkSTLReader.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "sscGraphicalPrimitives.h"
#include "sscToolManager.h"
#include "sscProbe.h"
#include "sscView.h"
#include "sscVideoSourceGraphics.h"
#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "sscToolTracer.h"

namespace ssc
{

ToolRep3D::ToolRep3D(const QString& uid, const QString& name) :
	RepImpl(uid, name),
	mSphereRadiusInNormalizedViewport(false),
	mTooltipPointColor(1.0, 0.8, 0.0),
	mOffsetPointColor(1.0, 0.8, 0.0),
	mOffsetLineColor(1.0, 0.8, 0.0),
	mStipplePattern(0xFFFF)
{
	mSphereRadius = 2;
	mStayHiddenAfterVisible = false;
	mStayVisibleAfterHide = false;
	mOffsetPointVisibleAtZeroOffset = false;
	mToolActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapperPtr::New();
	mSTLReader = vtkSTLReaderPtr::New();

	mOffsetPoint.reset(new GraphicalPoint3D());
	mOffsetLine.reset(new GraphicalLine3D());
	mTooltipPoint.reset(new GraphicalPoint3D());

	mProbeSector.reset(new ProbeSector());
	mProbeSectorPolyDataMapper = vtkPolyDataMapperPtr::New();
	mProbeSectorActor = vtkActorPtr::New();

	bool useMask = true; // if true, use mask instead of texture to render the sector. Mask is identical to the algo used in reconstruction.
	mRTStream.reset(new VideoSourceGraphics(useMask));

	mTracer.reset(new ToolTracer());
}

ToolRep3D::~ToolRep3D()
{
}

ToolTracerPtr ToolRep3D::getTracer()
{
	return mTracer;
}

ToolRep3DPtr ToolRep3D::New(const QString& uid, const QString& name)
{
	ToolRep3DPtr retval(new ToolRep3D(uid, name));
	retval->mSelf = retval;
	return retval;
}

QString ToolRep3D::getType() const
{
	return "ssc::ToolRep3D";
}

ToolPtr ToolRep3D::getTool()
{
	return mTool;
}

void ToolRep3D::setTool(ToolPtr tool)
{
	if (tool == mTool)
		return;

	mTracer->setTool(tool);

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));

		mToolActor->SetMapper(NULL);
	}

	mTool = tool;

	// setup new
	if (mTool)
	{
		vtkPolyDataPtr model;

		QString filename = mTool->getGraphicsFileName();
		if (!filename.isEmpty() && filename.endsWith("STL"))
		{
			mSTLReader->SetFileName(cstring_cast(filename));
			model = mSTLReader->GetOutput(); //read a 3D model file of the tool
		}
		else
		{
			model = mTool->getGraphicsPolyData(); // creates a cone, default
		}

		if (model)
		{
			vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
			normals->SetInput(model);
			normals->Update();
			model = normals->GetOutput();
			mPolyDataMapper->SetInput(model);

			mToolActor->SetMapper(mPolyDataMapper);
		}

		//some color to 3D cursor
		mToolActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
		if (mTool->hasType(Tool::TOOL_MANUAL))
		{
			mToolActor->GetProperty()->SetColor(mTooltipPointColor.begin());
		}

		receiveTransforms(mTool->get_prMt(), 0);
		mToolActor->SetVisibility(mTool->getVisible());

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(receiveTransforms(Transform3D, double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));

		this->probeSectorChanged();
	}
}

bool ToolRep3D::hasTool(ToolPtr tool) const
{
	return (mTool == tool);
}

void ToolRep3D::setSphereRadius(double radius)
{
	mSphereRadius = radius;
	if (mOffsetPoint)
		mOffsetPoint->setRadius(mSphereRadius);
	if (mTooltipPoint)
		mTooltipPoint->setRadius(mSphereRadius);
}

void ToolRep3D::setSphereRadiusInNormalizedViewport(bool on)
{
	if (mSphereRadiusInNormalizedViewport == on)
		return;

	mSphereRadiusInNormalizedViewport = on;

	if (on)
	{
		mViewportListener.reset(new ssc::ViewportListener);
		mViewportListener->setCallback(boost::bind(&ToolRep3D::scaleSpheres, this));
//    if (mView)
//      mViewportListener->startListen(mView->getRenderer());
	}
	else
	{
		mViewportListener.reset();
	}
}

void ToolRep3D::addRepActorsToViewRenderer(View *view)
{
	view->getRenderer()->AddActor(mTracer->getActor());

	view->getRenderer()->AddActor(mToolActor);
	view->getRenderer()->AddActor(mProbeSectorActor);

	mOffsetPoint.reset(new GraphicalPoint3D(view->getRenderer()));
	mOffsetPoint->setRadius(mSphereRadius);
	mOffsetPoint->setColor(mOffsetPointColor);

	mTooltipPoint.reset(new GraphicalPoint3D(view->getRenderer()));
	mTooltipPoint->setRadius(mSphereRadius);
	mTooltipPoint->setColor(mTooltipPointColor);

	mOffsetLine.reset(new GraphicalLine3D(view->getRenderer()));
	mOffsetLine->setColor(mOffsetLineColor);
	mOffsetLine->setStipple(mStipplePattern);

	mTooltipPoint->getActor()->SetVisibility(false);
	mOffsetPoint->getActor()->SetVisibility(false);
	mOffsetLine->getActor()->SetVisibility(false);

	if (mViewportListener)
		mViewportListener->startListen(view->getRenderer());

	if (mRTStream)
		view->getRenderer()->AddActor(mRTStream->getActor());
}

void ToolRep3D::removeRepActorsFromViewRenderer(View *view)
{
	view->getRenderer()->RemoveActor(mTracer->getActor());
	view->getRenderer()->RemoveActor(mToolActor);
	view->getRenderer()->RemoveActor(mProbeSectorActor);
	if (mRTStream)
		view->getRenderer()->RemoveActor(mRTStream->getActor());

	mTooltipPoint.reset(new GraphicalPoint3D());
	mOffsetPoint.reset(new GraphicalPoint3D());
	mOffsetLine.reset(new GraphicalLine3D());

	if (mViewportListener)
		mViewportListener->stopListen();
}

/**Note: Internal method!
 *
 * Scale the text to be a constant fraction of the viewport height
 * Called from a vtk camera observer
 *
 */
void ToolRep3D::scaleSpheres()
{
	if (!mViewportListener)
		return;
	if (!mViewportListener->isListening())
		return;

	double size = mViewportListener->getVpnZoom();
	double sphereSize = mSphereRadius/100/size;

	if (mOffsetPoint)
		mOffsetPoint->setRadius(sphereSize);
	if (mTooltipPoint)
		mTooltipPoint->setRadius(sphereSize);
}

void ToolRep3D::receiveTransforms(Transform3D prMt, double timestamp)
{
	Transform3DPtr rMprPtr = ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = (*rMprPtr) * prMt;
	mToolActor->SetUserMatrix(rMt.getVtkMatrix());
	this->update();
}

void ToolRep3D::update()
{
	Transform3D prMt = Transform3D::Identity();
	if (mTool)
	{
		prMt = mTool->get_prMt();
	}
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();

	if (this->showProbe())
	{
		Transform3D tMu = mProbeSector->get_tMu();
		mProbeSectorActor->SetUserMatrix((rMpr * prMt * tMu).getVtkMatrix());
		mProbeSectorActor->SetVisibility(mTool->getVisible());
	}
	else
		mProbeSectorActor->SetVisibility(false);

	this->updateOffsetGraphics();
}

void ToolRep3D::probeSectorChanged()
{
	if (!mTool)
		return;

	Transform3D prMt = mTool->get_prMt();
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();

	if (this->showProbe())
	{
		mProbeSector->setData(mTool->getProbeSector());
		Transform3D tMu = mProbeSector->get_tMu();

		mProbeSectorPolyDataMapper->SetInput(mProbeSector->getSectorLinesOnly());
		if (mProbeSectorPolyDataMapper->GetInput())
		{
			mProbeSectorActor->SetMapper(mProbeSectorPolyDataMapper);
		}
		mProbeSectorActor->SetUserMatrix((rMpr * prMt * tMu).getVtkMatrix());
		mProbeSectorActor->SetVisibility(mTool->getVisible());

		if (mRTStream)
		{
			mRTStream->setTool(mTool);
			ProbePtr probe = mTool->getProbe();
			if (probe)
			{
				mRTStream->setRealtimeStream(probe->getRTSource());
			}
		}
	}
	else
	{
		mProbeSectorActor->SetVisibility(false);
	}
}

void ToolRep3D::updateOffsetGraphics()
{
	bool visible = mTool && mTool->getVisible() && !mTool->hasType(Tool::TOOL_US_PROBE); // no offset for probes

	if (!mStayVisibleAfterHide || (mOffsetPoint->getActor()->GetVisibility() == false))
	{
		mOffsetPoint->getActor()->SetVisibility(visible);
		mTooltipPoint->getActor()->SetVisibility(visible);
		//Don't show tooltipPoint when in tool view, as this will obscure the offsetPoint
		if (mStayHiddenAfterVisible)
			mTooltipPoint->getActor()->SetVisibility(false);
		mOffsetLine->getActor()->SetVisibility(visible);
	}

	if (similar(0.0, mTool->getTooltipOffset()))
	{
		mTooltipPoint->getActor()->SetVisibility(visible && mOffsetPointVisibleAtZeroOffset);
		mOffsetPoint->getActor()->SetVisibility(false);
		mOffsetLine->getActor()->SetVisibility(false);
	}

	if (!mTool)
		return;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	Transform3D rMt = rMpr * mTool->get_prMt();

	Vector3D p0 = rMt.coord(Vector3D(0, 0, 0));
	Vector3D p1 = rMt.coord(Vector3D(0, 0, mTool->getTooltipOffset()));
	mOffsetPoint->setValue(p1);
	mOffsetLine->setValue(p0, p1);
	mTooltipPoint->setValue(Vector3D(p0));

	this->scaleSpheres();
}

void ToolRep3D::receiveVisible(bool visible)
{
	//  std::cout << "ToolRep3D::receiveVisible " << mTool->getName() << std::endl;

	if (!visible && mStayVisibleAfterHide)
	{
		return; // don't hide
	}
	mToolActor->SetVisibility(visible);

	if (mStayHiddenAfterVisible)
	{
		mToolActor->SetVisibility(false);
	}
	else
	{
		mToolActor->SetVisibility(mTool->getVisible());
	}

	//  std::cout << "Tool3DRep: receiveVisible "<< mTool->getName() << visible << " - " << mStayHiddenAfterVisible << std::endl;

	//updateOffsetGraphics();
	this->update();
}

void ToolRep3D::setStayHiddenAfterVisible(bool val)
{
	mStayHiddenAfterVisible = val;
	if (mTool)
		receiveVisible(mTool->getVisible());
}

/**
 * If true, tool is still rendered as visible after visibility status is hidden.
 * Nice for viewing the last known position of a tool.
 */
void ToolRep3D::setStayVisibleAfterHide(bool val)
{
	mStayVisibleAfterHide = val;
}
void ToolRep3D::setOffsetPointVisibleAtZeroOffset(bool val)
{
	mOffsetPointVisibleAtZeroOffset = val;
}
void ToolRep3D::tooltipOffsetSlot(double val)
{
	updateOffsetGraphics();
}

bool ToolRep3D::showProbe()
{
	return mTool && (mTool->hasType(Tool::TOOL_US_PROBE)) && (mTool->getProbeSector().getType()!=ssc::ProbeData::tNONE);
}
} // namespace ssc
