/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewWrapperVideo.h"
//#include <vector>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QAction>
#include <QMenu>

#include "cxView.h"
#include "cxVideoRep.h"
#include "cxDisplayTextRep.h"

#include "cxTypeConversions.h"

#include "cxSettings.h"
#include "cxTrackingService.h"
#include "cxVideoService.h"
#include "cxVisServices.h"
#include "vtkRenderWindowInteractor.h"
#include "cxTool.h"
#include "cxVideoSource.h"

namespace cx
{

ViewWrapperVideo::ViewWrapperVideo(ViewPtr view, VisServicesPtr services) :
	ViewWrapper(services)
{
	mView = view;
	this->connectContextMenu(mView);

	// disable vtk interactor: this wrapper IS an interactor
	mView->getRenderWindow()->GetInteractor()->Disable();
	mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
	double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(-clipDepth / 2.0, clipDepth / 2.0);

	connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &ViewWrapperVideo::connectStream);
	connect(mServices->video().get(), SIGNAL(activeVideoSourceChanged()), this, SLOT(connectStream()));
	connect(mServices->tracking().get(), SIGNAL(activeToolChanged(QString)), this, SLOT(connectStream()));

	this->addReps();

	this->connectStream();

	this->updateView();
}

ViewWrapperVideo::~ViewWrapperVideo()
{
	if (mView)
		mView->removeReps();
}

ViewPtr ViewWrapperVideo::getView()
{
	return mView;
}

void ViewWrapperVideo::setViewGroup(ViewGroupDataPtr group)
{
	ViewWrapper::setViewGroup(group);
	this->connectStream();
}

void ViewWrapperVideo::appendToContextMenu(QMenu& contextMenu)
{
	QAction* showSectorAction = new QAction("Show Sector", &contextMenu);
	showSectorAction->setCheckable(true);
	if (mStreamRep)
		showSectorAction->setChecked(mStreamRep->getShowSector());
	connect(showSectorAction, SIGNAL(triggered(bool)), this, SLOT(showSectorActionSlot(bool)));

	contextMenu.addSeparator();

	QMenu* sourceMenu = new QMenu("Video Source", &contextMenu);
	std::vector<VideoSourcePtr> sources = mServices->video()->getVideoSources();
	this->addStreamAction("active", sourceMenu);
	for (unsigned i=0; i<sources.size(); ++i)
		this->addStreamAction(sources[i]->getUid(), sourceMenu);
	contextMenu.addMenu(sourceMenu);

	contextMenu.addAction(showSectorAction);
}

void ViewWrapperVideo::showSectorActionSlot(bool checked)
{
	mStreamRep->setShowSector(checked);
	settings()->setValue("showSectorInRTView", checked);
}

void ViewWrapperVideo::addStreamAction(QString uid, QMenu* contextMenu)
{
	QAction* action = new QAction(uid, contextMenu);

	VideoSourcePtr selected = this->getSourceFromService(mGroupData->getVideoSource());
	VideoSourcePtr current = this->getSourceFromService(uid);

	action->setData(QVariant(uid));
	action->setCheckable(true);
	if (uid=="active")
		action->setChecked(mGroupData->getVideoSource()=="active");
	else
		action->setChecked(selected && (selected==current));

	connect(action, SIGNAL(triggered()), this, SLOT(streamActionSlot()));
	contextMenu->addAction(action);
}

void ViewWrapperVideo::streamActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;
	if (!theAction->isChecked())
		return;

	QString uid = theAction->data().toString();
	mGroupData->setVideoSource(uid);
}

void ViewWrapperVideo::videoSourceChangedSlot(QString uid)
{
	this->connectStream();
}

void ViewWrapperVideo::connectStream()
{
	if (!mGroupData)
		return;
	VideoSourcePtr source = this->getSourceFromService(mGroupData->getVideoSource());

	QString uid;
	if (source)
		uid = source->getUid();

	ToolPtr newTool;
	ToolPtr tool = mServices->tracking()->getFirstProbe();
	if (tool && tool->getProbe())
	{
		if (tool->getProbe()->getAvailableVideoSources().count(uid))
		{
			newTool = tool;
			source = tool->getProbe()->getRTSource(uid);
		}
	}

	this->setupRep(source, newTool);
}

VideoSourcePtr ViewWrapperVideo::getSourceFromService(QString uid)
{
	if (uid=="active")
		return mServices->video()->getActiveVideoSource();

	std::vector<VideoSourcePtr> source = mServices->video()->getVideoSources();

	for (unsigned i=0; i< source.size(); ++i)
	{
		if (source[i]->getUid()==uid)
			return source[i];
	}
	return VideoSourcePtr();
}

void ViewWrapperVideo::setupRep(VideoSourcePtr source, ToolPtr tool)
{
//	std::cout << "ViewWrapperVideo::setupRep() " << source.get() << "  " << source->getUid() << std::endl;

	//Don't do anything if source is the same
	if (( mSource == source )&&( tool==mTool ))
		return;
	if (mSource)
	{
		disconnect(mSource.get(), &VideoSource::newFrame, this, &ViewWrapperVideo::updateSlot);
	}
	mSource = source;
	if (mSource)
	{
		connect(mSource.get(), &VideoSource::newFrame, this, &ViewWrapperVideo::updateSlot);
	}

	if (!mSource)
		return;

	if (!mStreamRep)
	{
		mStreamRep.reset(new VideoFixedPlaneRep("rtrep", "rtrep"));
		mView->addRep(mStreamRep);
	}

	mStreamRep->setRealtimeStream(mSource);
	mStreamRep->setTool(tool);
	mStreamRep->setShowSector(settings()->value("showSectorInRTView").toBool());

//	report(
//					"Setup video rep with source="
//					+ source->getName() + " and tool="
//					+ (tool ? tool->getName() : "none"));
}

void ViewWrapperVideo::updateSlot()
{
	this->updateView();
}

QString ViewWrapperVideo::getDataDescription()
{
	if (mSource)
		return mSource->getName();
	return "not initialized";
}

QString ViewWrapperVideo::getViewDescription()
{
	return "RT";
}

//------------------------------------------------------------------------------
}
