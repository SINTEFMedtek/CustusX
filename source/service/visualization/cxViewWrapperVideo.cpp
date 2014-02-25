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
//#include <vector>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QAction>
#include <QMenu>

#include "sscView.h"
#include "sscVideoRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"

#include "cxSettings.h"
#include "cxToolManager.h"
#include "cxVideoService.h"
#include "cxVisualizationServiceBackend.h"

namespace cx
{

ViewWrapperVideo::ViewWrapperVideo(ViewWidget* view, VisualizationServiceBackendPtr backend) :
	ViewWrapper(backend)
{
	mView = view;
	this->connectContextMenu(mView);

	// disable vtk interactor: this wrapper IS an interactor
	mView->getRenderWindow()->GetInteractor()->Disable();
	mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
	double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(-clipDepth / 2.0, clipDepth / 2.0);

	connect(mBackend->getToolManager(), SIGNAL(configured()), this, SLOT(connectStream()));
	connect(mBackend->getVideoService(), SIGNAL(activeVideoSourceChanged()), this, SLOT(connectStream()));
	connect(mBackend->getToolManager(), SIGNAL(dominantToolChanged(QString)), this, SLOT(connectStream()));

	addReps();

	this->connectStream();
}

ViewWrapperVideo::~ViewWrapperVideo()
{
	if (mView)
		mView->removeReps();
}

ViewWidget* ViewWrapperVideo::getView()
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
	std::vector<VideoSourcePtr> sources = mBackend->getVideoService()->getVideoSources();
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
	ToolPtr tool = mBackend->getToolManager()->findFirstProbe();
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
		return mBackend->getVideoService()->getActiveVideoSource();

	std::vector<VideoSourcePtr> source = mBackend->getVideoService()->getVideoSources();

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
		mStreamRep.reset(new VideoFixedPlaneRep("rtrep", "rtrep"));
		mView->addRep(mStreamRep);
	}

	mStreamRep->setRealtimeStream(mSource);
	mStreamRep->setTool(tool);
//	mDataNameText->setText(0, "initialized");
	mDataNameText->setText(0, mSource->getName());
	mStreamRep->setShowSector(settings()->value("showSectorInRTView").toBool());

//	messageManager()->sendInfo(
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
	mPlaneTypeText = DisplayTextRep::New("planeTypeRep_" + mView->getName(), "");
	mPlaneTypeText->addText(QColor(Qt::green), "RT", Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = DisplayTextRep::New("dataNameText_" + mView->getName(), "");
	mDataNameText->addText(QColor(Qt::green), "not initialized", Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);
}

//------------------------------------------------------------------------------
}
