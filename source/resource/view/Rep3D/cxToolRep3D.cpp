/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#include "cxToolRep3D.h"

#include "boost/bind.hpp"
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkSTLReader.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>

#include "cxGraphicalPrimitives.h"
#include "cxProbe.h"
#include "cxView.h"
#include "cxVideoSourceGraphics.h"
#include "cxTypeConversions.h"
#include "cxToolTracer.h"
#include "cxTool.h"
#include "cxSpaceProvider.h"
#include "cxSettings.h"

namespace cx
{

ToolRep3D::ToolRep3D(SpaceProviderPtr spaceProvider) :
	RepImpl(),
	mSpaceProvider(spaceProvider),
	mSphereRadiusInNormalizedViewport(false),
	mStipplePattern(0xFFFF)
{
	mTooltipPointColor = settings()->value("View/toolTipPointColor").value<QColor>();
	mOffsetPointColor = settings()->value("View/toolOffsetPointColor").value<QColor>();
	mOffsetLineColor = settings()->value("View/toolOffsetLineColor").value<QColor>();

	mSphereRadius = 2;
	mStayHiddenAfterVisible = false;
	mStayVisibleAfterHide = false;
	mOffsetPointVisibleAtZeroOffset = false;
	mToolActor = vtkActorPtr::New();
	mPolyDataMapper = vtkPolyDataMapperPtr::New();
//	mSTLReader = vtkSTLReaderPtr::New();

	mOffsetPoint.reset(new GraphicalPoint3D());
	mOffsetLine.reset(new GraphicalLine3D());
	mTooltipPoint.reset(new GraphicalPoint3D());

	mProbeSector.reset(new ProbeSector());
	mProbeSectorPolyDataMapper = vtkPolyDataMapperPtr::New();
	mProbeSectorActor = vtkActorPtr::New();

	mTracer = ToolTracer::create(mSpaceProvider);
}

ToolRep3D::~ToolRep3D()
{}

ToolTracerPtr ToolRep3D::getTracer()
{
	return mTracer;
}

ToolRep3DPtr ToolRep3D::New(SpaceProviderPtr spaceProvider, const QString& uid)
{
	return wrap_new(new ToolRep3D(spaceProvider), uid);
}

QString ToolRep3D::getType() const
{
	return "ToolRep3D";
}

ToolPtr ToolRep3D::getTool()
{
	return mTool;
}

void ToolRep3D::updateToolActor()
{
	if (!mTool)
	{
		mToolActor->SetMapper(NULL);
		return;
	}

	vtkPolyDataPtr model = mTool->getGraphicsPolyData();

	if (model)
	{
		vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
		normals->SetInputData(model);
		mPolyDataMapper->SetInputConnection(normals->GetOutputPort());
		mPolyDataMapper->Update();
		mToolActor->SetMapper(mPolyDataMapper);
	}

	//some color to 3D cursor
	mToolActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
	if (mTool->hasType(Tool::TOOL_MANUAL))
	{
		setColorAndOpacity(mToolActor->GetProperty(), mTooltipPointColor);
	}

	this->setModified();
	mToolActor->SetVisibility(mTool->getVisible());
}

void ToolRep3D::setTool(ToolPtr tool)
{
	if (tool == mTool)
		return;

	mTracer->setTool(tool);

	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(setModified()));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
		disconnect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(updateToolActor()));
	}

	mTool = tool;

	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D, double)), this, SLOT(setModified()));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(receiveVisible(bool)));
		connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SLOT(tooltipOffsetSlot(double)));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(probeSectorChanged()));
		connect(mTool.get(), SIGNAL(toolProbeSector()), this, SLOT(updateToolActor()));
	}

	this->updateToolActor();
	this->setModified();
	this->probeSectorChanged();
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
		mViewportListener.reset(new ViewportListener);
		mViewportListener->setCallback(boost::bind(&ToolRep3D::scaleSpheres, this));
	}
	else
	{
		mViewportListener.reset();
	}
}

/**
 * @brief ToolRep3D::setTooltipPointColor
 * @param color
 * Sets the color of the crosshair and the sphere.
 */
void ToolRep3D::setTooltipPointColor(const QColor& color)
{
	if(mToolActor)
		setColorAndOpacity(mToolActor->GetProperty(), color);
	if(mTooltipPoint)
		mTooltipPoint->setColor(color);
}

void ToolRep3D::setToolOffsetPointColor(const QColor& color)
{
	if(mOffsetPoint)
		mOffsetPoint->setColor(color);
}

void ToolRep3D::addRepActorsToViewRenderer(ViewPtr view)
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
}

void ToolRep3D::removeRepActorsFromViewRenderer(ViewPtr view)
{
	view->getRenderer()->RemoveActor(mTracer->getActor());
	view->getRenderer()->RemoveActor(mToolActor);
	view->getRenderer()->RemoveActor(mProbeSectorActor);

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

void ToolRep3D::onModifiedStartRender()
{
	this->update();
}


void ToolRep3D::update()
{
	Transform3D prMt = Transform3D::Identity();
	if (mTool)
		prMt = mTool->get_prMt();
	Transform3D rMpr = mSpaceProvider->get_rMpr();

	Transform3D rMt = rMpr * prMt;
	mToolActor->SetUserMatrix(rMt.getVtkMatrix());

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
	Transform3D rMpr = mSpaceProvider->get_rMpr();

	if (this->showProbe())
	{
		mProbeSector->setData(mTool->getProbe()->getProbeDefinition());
		Transform3D tMu = mProbeSector->get_tMu();

		mProbeSectorPolyDataMapper->SetInputData(mProbeSector->getSectorLinesOnly());
		if (mProbeSectorPolyDataMapper->GetInput())
		{
			mProbeSectorActor->SetMapper(mProbeSectorPolyDataMapper);
		}
		mProbeSectorActor->SetUserMatrix((rMpr * prMt * tMu).getVtkMatrix());
		mProbeSectorActor->SetVisibility(mTool->getVisible());
	}
	else
		mProbeSectorActor->SetVisibility(false);
}

void ToolRep3D::updateOffsetGraphics()
{
	bool visible = mTool && mTool->getVisible();

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
        if(mTool->hasType(Tool::TOOL_US_PROBE))
            mTooltipPoint->getActor()->SetVisibility(false);
        else
            mTooltipPoint->getActor()->SetVisibility(visible && mOffsetPointVisibleAtZeroOffset);
		mOffsetPoint->getActor()->SetVisibility(false);
		mOffsetLine->getActor()->SetVisibility(false);
	}

	if (!mTool)
		return;
	Transform3D rMpr = mSpaceProvider->get_rMpr();
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
	if (!visible && mStayVisibleAfterHide)
		return; // don't hide
	mToolActor->SetVisibility(visible);

	if (mStayHiddenAfterVisible)
		mToolActor->SetVisibility(false);
	else
		mToolActor->SetVisibility(mTool->getVisible());

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
	return mTool && (mTool->hasType(Tool::TOOL_US_PROBE)) && (mTool->getProbe()->getProbeDefinition().getType()!=ProbeDefinition::tNONE);
}

} // namespace cx
