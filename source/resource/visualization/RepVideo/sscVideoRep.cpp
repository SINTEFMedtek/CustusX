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

/*
 * sscRT2DRep.cpp
 *
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */
#include "sscVideoRep.h"

#include "boost/bind.hpp"

#include <vtkRenderer.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <vtkCamera.h>

#include "sscView.h"
#include "sscTool.h"
#include "sscVideoSourceGraphics.h"
#include "sscVideoSource.h"
#include "sscViewportListener.h"

namespace cx
{

VideoFixedPlaneRep::VideoFixedPlaneRep(const QString& uid, const QString& name) :
	RepImpl(uid, name)
{
	SpaceProviderPtr nullProvider;

	mRTGraphics.reset(new VideoSourceGraphics(nullProvider));
	connect(mRTGraphics.get(), SIGNAL(newData()), this, SLOT(newDataSlot()));
	mRTGraphics->setShowInToolSpace(false);
	mRTGraphics->setClipToSector(false);

	mInfoText.reset(new TextDisplay("", QColor::fromRgbF(1.0, 0.8, 0.0), 16));
	mInfoText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mInfoText->setCentered();
	mInfoText->setPosition(0.5, 0.05);

	mStatusText.reset(new TextDisplay("", QColor::fromRgbF(1.0, 0.8, 0.0), 20));
	mStatusText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mStatusText->setCentered();
	mStatusText->setPosition(0.5, 0.5);
	mStatusText->updateText("Not Connected");

	mOrientationVText.reset(new TextDisplay("V", QColor::fromRgbF(1.0, 0.9, 0.0), 30));
	mOrientationVText->getActor()->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	mOrientationVText->setCentered();
	mOrientationVText->setPosition(0.05, 0.95);

	mProbeOrigin.reset(new GraphicalPolyData3D());
	mProbeOrigin->setColor(Vector3D(1, 165.0/255.0, 0)); // orange
	mProbeSector.reset(new GraphicalPolyData3D());
	mProbeSector->setColor(Vector3D(1, 165.0/255.0, 0)); // orange
	mProbeClipRect.reset(new GraphicalPolyData3D());
	mProbeClipRect->setColor(Vector3D(1, 0.9, 0)); // yellow

	mViewportListener.reset(new ViewportListener());
	mViewportListener->setCallback(boost::bind(&VideoFixedPlaneRep::setCamera, this));
}

VideoFixedPlaneRep::~VideoFixedPlaneRep()
{
}

void VideoFixedPlaneRep::setShowSector(bool on)
{
	mShowSector = on;
	this->updateSector();
}

bool VideoFixedPlaneRep::getShowSector() const
{
	return mShowSector;
}

void VideoFixedPlaneRep::updateSector()
{
	bool show = mTool && this->getShowSector() && mTool->getProbeSector().getType()!=ProbeDefinition::tNONE;

	mProbeOrigin->getActor()->SetVisibility(show);
	mProbeSector->getActor()->SetVisibility(show);
	mProbeClipRect->getActor()->SetVisibility(show);
	if (!show)
		return;

	mProbeData.setData(mTool->getProbeSector());

	mProbeOrigin->setData(mProbeData.getOriginPolyData());
	mProbeSector->setData(mProbeData.getSectorSectorOnlyLinesOnly());
	mProbeClipRect->setData(mProbeData.getClipRectLinesOnly());
}

void VideoFixedPlaneRep::setTool(ToolPtr tool)
{
	mRTGraphics->setTool(tool);
	mTool = tool;
}

void VideoFixedPlaneRep::setRealtimeStream(VideoSourcePtr data)
{
	mData = data;
	mRTGraphics->setRealtimeStream(data);
}

void VideoFixedPlaneRep::newDataSlot()
{
	if (!mData)
		return;

	mInfoText->updateText(mData->getInfoString());
	mStatusText->updateText(mData->getStatusString());
	mStatusText->getActor()->SetVisibility(!mData->validData());
	mOrientationVText->getActor()->SetVisibility(mData->validData());
	this->setCamera();
	this->updateSector();
}

/**We need this here, even if it belongs in singlelayout.
 * Reason: must call setcamera after last change of size of plane actor.
 * TODO fix it.
 */
void VideoFixedPlaneRep::setCamera()
{
	if (!mRenderer)
		return;
	mViewportListener->stopListen();
	vtkCamera* camera = mRenderer->GetActiveCamera();
	camera->ParallelProjectionOn();
	mRenderer->ResetCamera();

	DoubleBoundingBox3D bounds(mRTGraphics->getActor()->GetBounds());
	if (similar(bounds.range()[0], 0.0) || similar(bounds.range()[1], 0.0))
		return;

	double* vpRange = mRenderer->GetAspect();

	double vw = vpRange[0];
	double vh = vpRange[1];

	double w = bounds.range()[0];
	double h = bounds.range()[1];

	double scale = 1;
	double w_vp = vh * (w/h); // width of image in viewport space
	if (w_vp > vw) // if image too wide: reduce scale
		scale = w_vp/vw;

    //Set camera position and focus so that it looks at the video stream center
    double position[3];
    camera->GetPosition(position);
    position[0] = bounds.center()[0];
    position[1] = bounds.center()[1];
    camera->SetPosition(position);

    camera->GetFocalPoint(position);
    position[0] = bounds.center()[0];
    position[1] = bounds.center()[1];
    camera->SetFocalPoint(position);

	camera->SetParallelScale(h/2*scale*1.01); // exactly fill the viewport height
	mViewportListener->startListen(mRenderer);
}


void VideoFixedPlaneRep::addRepActorsToViewRenderer(View* view)
{
	mView = view;
	mRenderer = view->getRenderer();
	mViewportListener->startListen(mRenderer);

	view->getRenderer()->AddActor(mRTGraphics->getActor());
	view->getRenderer()->AddActor(mInfoText->getActor());
	view->getRenderer()->AddActor(mStatusText->getActor());
	view->getRenderer()->AddActor(mOrientationVText->getActor());

	mProbeClipRect->setRenderer(view->getRenderer());
	mProbeOrigin->setRenderer(view->getRenderer());
	mProbeSector->setRenderer(view->getRenderer());
}

void VideoFixedPlaneRep::removeRepActorsFromViewRenderer(View* view)
{
	mRenderer = vtkRendererPtr();
	view->getRenderer()->RemoveActor(mRTGraphics->getActor());
	view->getRenderer()->RemoveActor(mInfoText->getActor());
	view->getRenderer()->RemoveActor(mStatusText->getActor());
	view->getRenderer()->RemoveActor(mOrientationVText->getActor());
	mProbeOrigin->setRenderer(NULL);
	mProbeSector->setRenderer(NULL);
	mProbeClipRect->setRenderer(NULL);

	mViewportListener->stopListen();
}

} // namespace cx
