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

	addReps();

	this->connectStream();
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

//	QActionGroup sourceGroup = new QActionGroup(&contextMenu);
	QMenu* sourceMenu = new QMenu("Video Source", &contextMenu);
	std::vector<VideoSourcePtr> sources = mServices->video()->getVideoSources();
	this->addStreamAction("active", sourceMenu);
	for (unsigned i=0; i<sources.size(); ++i)
		this->addStreamAction(sources[i]->getUid(), sourceMenu);
	contextMenu.addMenu(sourceMenu);

//	contextMenu.addSeparator();
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
//	std::cout << "ViewWrapperVideo::connectStream() selected=" << mViewGroup->getVideoSource()  << std::endl;
	VideoSourcePtr source = this->getSourceFromService(mGroupData->getVideoSource());
//	if (source)
//		std::cout << "ViewWrapperVideo::connectStream() " << source->getUid() << std::endl;
//	else
//		std::cout << "ViewWrapperVideo::connectStream() NULL" << std::endl;


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

//			if (source)
//				std::cout << "ViewWrapperVideo::connectStream() from probe " << source->getUid() << std::endl;
//			else
//				std::cout << "ViewWrapperVideo::connectStream() from probe NULL" << std::endl;
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
//	mDataNameText->setText(0, "initialized");
	mDataNameText->setText(0, mSource->getName());
	mStreamRep->setShowSector(settings()->value("showSectorInRTView").toBool());

//	report(
//					"Setup video rep with source="
//					+ source->getName() + " and tool="
//					+ (tool ? tool->getName() : "none"));
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
	mPlaneTypeText = DisplayTextRep::New();
	mPlaneTypeText->addText(QColor(Qt::green), "RT", Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = DisplayTextRep::New();
	mDataNameText->addText(QColor(Qt::green), "not initialized", Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);
}

//------------------------------------------------------------------------------
}
