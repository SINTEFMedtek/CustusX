// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxViewWrapperVideo.h"
#include <vector>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "sscUtilHelpers.h"
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscToolRep2D.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscDefinitionStrings.h"
#include "sscSlicePlanes3DRep.h"
#include "sscDefinitionStrings.h"
#include "sscSliceComputer.h"
#include "sscGeometricRep2D.h"
#include "sscVideoRep.h"
#include "cxSettings.h"
#include "cxViewManager.h"
#include "cxToolManager.h"
#include "cxViewGroup.h"
#include "cxVideoService.h"

#include "sscData.h"
#include "sscMesh.h"
#include "cxViewWrapper.h"
#include "sscVideoRep.h"

namespace cx
{

ViewWrapperVideo::ViewWrapperVideo(ssc::ViewWidget* view)
{
	mView = view;
	this->connectContextMenu(mView);

	// disable vtk interactor: this wrapper IS an interactor
	mView->getRenderWindow()->GetInteractor()->Disable();
	mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
	double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(-clipDepth / 2.0, clipDepth / 2.0);

//	connect(ssc::dataManager(), SIGNAL(streamLoaded()), this, SLOT(configureSlot()));
	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(configureSlot()));
	connect(videoService(), SIGNAL(activeVideoSourceChanged()), this, SLOT(configureSlot()));
	mDominantToolProxy = DominantToolProxy::New();
	//This connect stops video streaming?

	addReps();

	this->configureSlot();
}

ViewWrapperVideo::~ViewWrapperVideo()
{
	if (mView)
		mView->removeReps();
}

ssc::ViewWidget* ViewWrapperVideo::getView()
{
	return mView;
}

void ViewWrapperVideo::appendToContextMenu(QMenu& contextMenu)
{
	QAction* showSectorAction = new QAction("Show Sector", &contextMenu);
	showSectorAction->setCheckable(true);
	if (mStreamRep)
		showSectorAction->setChecked(mStreamRep->getShowSector());
	connect(showSectorAction, SIGNAL(triggered(bool)), this, SLOT(showSectorActionSlot(bool)));

	contextMenu.addSeparator();
	contextMenu.addAction(showSectorAction);
}

void ViewWrapperVideo::showSectorActionSlot(bool checked)
{
	mStreamRep->setShowSector(checked);
	settings()->setValue("showSectorInRTView", checked);
}

/** Setup connections to stream. Called when a stream is loaded into the datamanager, or if a probe is initialized
 *
 */
void ViewWrapperVideo::configureSlot()
{
	// if probe tool exist, connect to probeChanged()
	if (mTool)
		disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));
	mTool = ToolManager::getInstance()->findFirstProbe();
	if (mTool)
		connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

	this->probeChangedSlot();

	// if no probe: use a raw video source
	if (videoService()->getActiveVideoSource() && !mTool)
			this->setupRep(videoService()->getActiveVideoSource(), ssc::ToolPtr());
}

void ViewWrapperVideo::probeChangedSlot()
{
	if (!mTool)
		return;

	// if probe has a stream, connect stream and probe to rep.
	this->setupRep(mTool->getProbe()->getRTSource(), mTool);
}

void ViewWrapperVideo::setupRep(ssc::VideoSourcePtr source, ssc::ToolPtr tool)
{
	//Don't do anything if source is the same
	if (mSource == source)
		return;
	if (mSource)
	{
		disconnect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
	}
	mSource = source;
	if (mSource)
	{
		connect(mSource.get(), SIGNAL(newFrame()), this, SLOT(updateSlot()));
	}

	if (!mSource)
		return;

	if (!mStreamRep)
	{
		mStreamRep.reset(new ssc::VideoFixedPlaneRep("rtrep", "rtrep"));
		mView->addRep(mStreamRep);
	}

	mStreamRep->setRealtimeStream(mSource);
	mStreamRep->setTool(tool);
	mDataNameText->setText(0, "initialized");
	mStreamRep->setShowSector(settings()->value("showSectorInRTView").toBool());

	ssc::messageManager()->sendInfo(
					"Setup rt rep with source="
					+ source->getName() + " and tool="
					+ (tool ? tool->getName() : "none"));
}

void ViewWrapperVideo::updateSlot()
{
	if (!mSource)
		return;
	mDataNameText->setText(0, mSource->getName());
}

void ViewWrapperVideo::addReps()
{
	// plane type text rep
	mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_" + mView->getName(), "");
	mPlaneTypeText->addText(ssc::Vector3D(0, 1, 0), "RT", ssc::Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = ssc::DisplayTextRep::New("dataNameText_" + mView->getName(), "");
	mDataNameText->addText(ssc::Vector3D(0, 1, 0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);
}

//------------------------------------------------------------------------------
}
