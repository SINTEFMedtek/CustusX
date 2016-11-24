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

#include "cxViewManager.h"

#include "boost/bind.hpp"
#include <QGridLayout>
#include <QWidget>
#include <QTime>
#include <QAction>
#include <QtCore>
#include <qtextstream.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkOpenGLRenderWindow.h>

#include "cxLayoutData.h"
#include "cxVolumetricRep.h"
#include "cxLogger.h"
#include "cxXmlOptionItem.h"
#include "cxTrackingService.h"
#include "cxSlicePlanes3DRep.h"
#include "cxSliceProxy.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxViewWrapperVideo.h"
#include "cxSettings.h"
#include "cxProfile.h"
#include "cxInteractiveCropper.h"
#include "vtkForwardDeclarations.h"
#include "cxInteractiveClipper.h"
#include "cxImage.h"
#include "cxCameraStyle.h"
#include "cxCyclicActionLogger.h"
#include "cxViewCollectionWidget.h"
#include "cxRenderLoop.h"
#include "cxLayoutRepository.h"

#include "cxVisServices.h"
#include "cxXMLNodeWrapper.h"
#include "cxCameraControl.h"
#include "cxNavigation.h"
#include "cxPatientModelService.h"
#include "cxCameraStyleInteractor.h"
#include "cxSharedOpenGLContext.h"
//#include "cxSharedContextCreatedCallback.h"
#include "cxRenderWindowFactory.h"


namespace cx
{

ViewManagerPtr ViewManager::create(VisServicesPtr backend)
{
	ViewManagerPtr retval;
	retval.reset(new ViewManager(backend));
	return retval;
}

ViewManager::ViewManager(VisServicesPtr backend) :
				mGlobalObliqueOrientation(false)
{
	mBackend = backend;
	mRenderWindowFactory = RenderWindowFactoryPtr(new RenderWindowFactory());
//	connect(mRenderWindowFactory.get(), &RenderWindowFactory::sharedOpenGLContextCreated, this, &ViewManager::setSharedOpenGLContext);

	mRenderLoop.reset(new RenderLoop());
	connect(mRenderLoop.get(), &RenderLoop::preRender, this, &ViewManager::updateViews);
	connect(mRenderLoop.get(), &RenderLoop::fps, this, &ViewManager::fps);
	connect(mRenderLoop.get(), &RenderLoop::renderFinished, this, &ViewManager::renderFinished);

	mSlicePlanesProxy.reset(new SlicePlanesProxy());
	mLayoutRepository.reset(new LayoutRepository());
	mCameraControl.reset(new CameraControl());

	mRenderLoop->setLogging(settings()->value("renderSpeedLogging").toBool());
	mRenderLoop->setSmartRender(settings()->value("smartRender", true).toBool());
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	const unsigned VIEW_GROUP_COUNT = 5; // set this to enough
	// initialize view groups:
	for (unsigned i = 0; i < VIEW_GROUP_COUNT; ++i)
	{
		ViewGroupPtr group(new ViewGroup(mBackend, QString::number(i)));
		mViewGroups.push_back(group);
	}
//	this->setSharedOpenGLContextInViewGroups(this->mRenderWindowFactory->getSharedOpenGLContext());

	// moved here from initialize() ... ensures object is fully callable after construction
	mCameraStyleInteractor.reset(new CameraStyleInteractor);

	mActiveLayout = QStringList() << "" << "";
	mLayoutWidgets.resize(mActiveLayout.size(), NULL);

	mInteractiveCropper.reset(new InteractiveCropper(mBackend->patient()->getActiveData()));
	connect(this, SIGNAL(activeViewChanged()), this, SLOT(updateCameraStyleActions()));

    this->loadGlobalSettings();
	// connect to layoutrepo after load of global
	connect(mLayoutRepository.get(), &LayoutRepository::layoutChanged, this, &ViewManager::onLayoutRepositoryChanged);
	this->initializeGlobal2DZoom();
    this->initializeActiveView();

	// set start layout
	this->setActiveLayout("LAYOUT_3D_ACS_SINGLE", 0);

	mRenderLoop->setRenderingInterval(settings()->value("renderingInterval").toInt());
	this->enableRender(true);
}

std::vector<ViewGroupPtr> ViewManager::getViewGroups()
{
	return mViewGroups;
}

ViewManager::~ViewManager()
{
}

void ViewManager::enableRender(bool val)
{
	if (val)
		mRenderLoop->start();
	else
		mRenderLoop->stop();

	emit renderingEnabledChanged();
}

bool ViewManager::renderingIsEnabled() const
{
	return mRenderLoop->isRunning();
}


void ViewManager::initializeGlobal2DZoom()
{
	mGlobal2DZoomVal = SyncedValue::create(1);

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
		mViewGroups[i]->getData()->initializeGlobal2DZoom(mGlobal2DZoomVal);
}

void ViewManager::initializeActiveView()
{
	mActiveView = SyncedValue::create("");
	connect(mActiveView.get(), SIGNAL(changed()), this, SIGNAL(activeViewChanged()));

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
		mViewGroups[i]->initializeActiveView(mActiveView);
}


NavigationPtr ViewManager::getNavigation(int group)
{
	mCameraControl->refreshView(this->get3DView(group));
	return NavigationPtr(new Navigation(mBackend, mCameraControl));
}

QWidget *ViewManager::getLayoutWidget(int index)
{
	if (index >= mLayoutWidgets.size())
        return NULL;
   return mLayoutWidgets[index];
}

QWidget *ViewManager::createLayoutWidget(QWidget* parent, int index)
{
	if (index >= mLayoutWidgets.size())
		return NULL;

	if (!mLayoutWidgets[index])
	{
        bool optimizedViews = settings()->value("optimizedViews").toBool();

		if (optimizedViews)
		{
			mLayoutWidgets[index] = ViewCollectionWidget::createOptimizedLayout(mRenderWindowFactory);
		}
		else
		{
			mLayoutWidgets[index] = ViewCollectionWidget::createViewWidgetLayout(mRenderWindowFactory);
		}

		connect(mLayoutWidgets[index].data(), &QObject::destroyed, this, &ViewManager::layoutWidgetDestroyed);
		mRenderLoop->addLayout(mLayoutWidgets[index]);

		this->rebuildLayouts();
	}
	return mLayoutWidgets[index];
}

/**
 * When GUI deletes the layout widget, we must clear resources here.
 * Our own destructor gets called in a secondary thread (from pluginframework->stop),
 * which shouldnt destroy GL stuff.
 *
 */
void ViewManager::layoutWidgetDestroyed(QObject* object)
{
//	ViewCollectionWidget* widget = dynamic_cast<ViewCollectionWidget*>(object);
	for (unsigned i=0; i<mLayoutWidgets.size(); ++i)
	{
		if (mLayoutWidgets[i] == object)
			mLayoutWidgets[i] = NULL;
	}

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->removeViews();
	}

	this->setActiveView("");
	mSlicePlanesProxy->clearViewports();

	QString uid = this->getActiveLayout();
	mActiveLayout[0] = ""; // hack: force trigger a change
	this->setActiveLayout(uid, 0);
}

void ViewManager::updateViews()
{
	for(unsigned i=0; i<mViewGroups.size(); ++i)
	{
		ViewGroupPtr group = mViewGroups[i];
		for (unsigned j=0; j<group->getWrappers().size(); ++j)
			group->getWrappers()[j]->updateView();
	}
}

void ViewManager::settingsChangedSlot(QString key)
{
	if (key == "smartRender")
	{
		mRenderLoop->setSmartRender(settings()->value("smartRender", true).toBool());
	}
	if (key == "renderingInterval")
	{
		mRenderLoop->setRenderingInterval(settings()->value("renderingInterval").toInt());
	}
	if (key == "renderSpeedLogging")
	{
		mRenderLoop->setLogging(settings()->value("renderSpeedLogging").toBool());
	}
}

InteractiveCropperPtr ViewManager::getCropper()
{
	return mInteractiveCropper;
}

QString ViewManager::getActiveLayout(int widgetIndex) const
{
	CX_ASSERT(mActiveLayout.size() > widgetIndex);
	return mActiveLayout[widgetIndex];
}

QString ViewManager::getActiveView() const
{
	return mActiveView->get().value<QString>();
}

void ViewManager::setActiveView(QString uid)
{
	mActiveView->set(uid);
}

int ViewManager::getActiveViewGroup() const
{
	int retval = -1;
	QString activeView = mActiveView->value<QString>();

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		ViewWrapperPtr viewWrapper = mViewGroups[i]->getViewWrapperFromViewUid(activeView);
		if (viewWrapper)
			retval = i;
	}

	return retval;
}

void ViewManager::addXml(QDomNode& parentNode)
{
	XMLNodeAdder parent(parentNode);
	XMLNodeAdder base(parent.addElement("viewManager"));

	base.addTextToElement("global2DZoom", qstring_cast(mGlobal2DZoomVal->get().toDouble()));

	QDomElement slicePlanes3DNode = base.addElement("slicePlanes3D");
	slicePlanes3DNode.setAttribute("use", mSlicePlanesProxy->getVisible());
	slicePlanes3DNode.setAttribute("opaque", mSlicePlanesProxy->getDrawPlanes());

	XMLNodeAdder viewGroupsNode(base.addElement("viewGroups"));
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		QDomElement viewGroupNode = viewGroupsNode.addElement("viewGroup");
		viewGroupNode.setAttribute("index", i);
		mViewGroups[i]->addXml(viewGroupNode);
	}
}

void ViewManager::parseXml(QDomNode viewmanagerNode)
{
	XMLNodeParser base(viewmanagerNode);

	QString clippedImage = base.parseTextFromElement("clippedImage");

	base.parseDoubleFromElementWithDefault("global2DZoom", mGlobal2DZoomVal->get().toDouble());

	QDomElement slicePlanes3DNode = base.parseElement("slicePlanes3D");
	mSlicePlanesProxy->setVisible(slicePlanes3DNode.attribute("use").toInt());
	mSlicePlanesProxy->setDrawPlanes(slicePlanes3DNode.attribute("opaque").toInt());

	QDomElement viewgroups = base.parseElement("viewGroups");
	QDomNode viewgroup = viewgroups.firstChild();
	while (!viewgroup.isNull())
	{
		if (viewgroup.toElement().tagName() != "viewGroup")
		{
			viewgroup = viewgroup.nextSibling();
			continue;
		}
		int index = viewgroup.toElement().attribute("index").toInt();

		if (index < 0 || index >= int(mViewGroups.size()))
		{
			viewgroup = viewgroup.nextSibling();
			continue;
		}

		mViewGroups[index]->parseXml(viewgroup);

		viewgroup = viewgroup.nextSibling();
	}
}

void ViewManager::clear()
{
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->clearPatientData();
	}
}

/**Look for the index'th 3DView in given group.
 */
ViewPtr ViewManager::get3DView(int group, int index)
{
	int count = 0;
	std::vector<ViewPtr> views = mViewGroups[group]->getViews();
	for (unsigned i = 0; i < views.size(); ++i)
	{
		if(!views[i])
			continue;
		if (views[i]->getType()!=View::VIEW_3D)
			continue;
		if (index == count++)
			return views[i];
	}
	return ViewPtr();
}


/**deactivate the current layout, leaving an empty layout
 */
void ViewManager::deactivateCurrentLayout()
{
	for (unsigned i=0; i<mLayoutWidgets.size(); ++i)
	{
		if (mLayoutWidgets[i])
			mLayoutWidgets[i]->clearViews();
	}

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->removeViews();
	}

	this->setActiveView("");
	mSlicePlanesProxy->clearViewports();
}

ViewGroupDataPtr ViewManager::getViewGroup(int groupIdx) const
{
	if (( groupIdx>=0 )&&( groupIdx < mViewGroups.size() ))
		return mViewGroups[groupIdx]->getData();
	return ViewGroupDataPtr();
}

unsigned ViewManager::viewGroupCount() const
{
	int count = 0;
	while(this->getViewGroup(count))
		++count;
	return count;
}

/**Change layout from current to layout.
 */
void ViewManager::setActiveLayout(const QString& layout, int widgetIndex)
{
	if(!mLayoutRepository->exists(layout))
		return;

	CX_ASSERT(mActiveLayout.size() > widgetIndex);

	if (mActiveLayout[widgetIndex] == layout)
		return;

	mActiveLayout[widgetIndex] = layout;

	this->rebuildLayouts();

	if (!mViewGroups[0]->getViews().empty())
		this->setActiveView(mViewGroups[0]->getViews()[0]->getUid());

	emit activeLayoutChanged();

	QString layoutName = mLayoutRepository->get(layout).getName();
	report(QString("Layout %1 changed to %2").arg(widgetIndex).arg(layoutName));
}

void ViewManager::rebuildLayouts()
{
	this->deactivateCurrentLayout();

	for (unsigned i=0; i<mLayoutWidgets.size(); ++i)
	{
		LayoutData next = mLayoutRepository->get(mActiveLayout[i]);
		if (mLayoutWidgets[i] && !next.getUid().isEmpty())
			this->activateViews(mLayoutWidgets[i], next);
	}

	this->setSlicePlanesProxyInViewsUpTo2DViewgroup();

	mCameraControl->refreshView(this->get3DView());
}

void ViewManager::setSlicePlanesProxyInViewsUpTo2DViewgroup()
{
	// Set the same proxy in all wrappers, but stop adding after the
	// first group with 2D views are found.
	// This works well _provided_ that the 3D view is in the first group.
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		bool foundSlice = false;
		std::vector<ViewWrapperPtr> wrappers = mViewGroups[i]->getWrappers();
		for (unsigned j = 0; j < wrappers.size(); ++j)
		{
			wrappers[j]->setSlicePlanesProxy(mSlicePlanesProxy);
			foundSlice = foundSlice || wrappers[j]->getView()->getType() == View::VIEW_2D;
		}
		if (foundSlice)
			break;
	}
}
void ViewManager::activateViews(ViewCollectionWidget *widget, LayoutData next)
{
	if (!widget)
		return;

	widget->setOffScreenRenderingAndClear(next.getOffScreenRendering());

	for (LayoutData::iterator iter = next.begin(); iter != next.end(); ++iter)
		this->activateView(widget, *iter);
}

void ViewManager::setRenderingInterval(int interval)
{
	mRenderLoop->setRenderingInterval(interval);
}

void ViewManager::activateView(ViewCollectionWidget* widget, LayoutViewData viewData)
{
	if (!viewData.isValid())
		return;

	ViewPtr view = widget->addView(viewData.mType, viewData.mRegion);

	/*
	mSharedContextCreatedCallback = SharedContextCreatedCallbackPtr::New();
	mSharedContextCreatedCallback->setViewService(mBackend->view());
	view->getRenderWindow()->AddObserver(vtkCommand::CXSharedContextCreatedEvent, mSharedContextCreatedCallback);
	*/


	vtkRenderWindowInteractorPtr interactor = view->getRenderWindow()->GetInteractor();
	//Turn off rendering in vtkRenderWindowInteractor
	interactor->EnableRenderOff();
	//Increase the StillUpdateRate in the vtkRenderWindowInteractor (default is 0.0001 images per second)
	double rate = settings()->value("stillUpdateRate").value<double>();
	interactor->SetStillUpdateRate(rate);
	// Set the same value when moving (seems counterintuitive, but for us, moving isnt really special.
	// The real challenge is updating while the tracking is active, and this uses the still update rate.
	interactor->SetDesiredUpdateRate(rate);

	ViewWrapperPtr wrapper = this->createViewWrapper(view, viewData);
	if(!mRenderWindowFactory->getSharedOpenGLContext())
		CX_LOG_WARNING() << "ViewManager::activateView: got not shared OpenGL context";
	mViewGroups[viewData.mGroup]->addView(wrapper, mRenderWindowFactory->getSharedOpenGLContext());
}

ViewWrapperPtr ViewManager::createViewWrapper(ViewPtr view, LayoutViewData viewData)
{
	if (viewData.mType == View::VIEW_2D)
	{
		ViewWrapper2DPtr wrapper(new ViewWrapper2D(view, mBackend));
		wrapper->initializePlane(viewData.mPlane);
		connect(wrapper.get(), &ViewWrapper2D::pointSampled, this, &ViewManager::pointSampled);
		return wrapper;
	}
	else if (viewData.mType == View::VIEW_3D)
	{

		ViewWrapper3DPtr wrapper(new ViewWrapper3D(viewData.mGroup + 1, view, mBackend));
		if (viewData.mGroup == 0)
			mInteractiveCropper->setView(view);
		return wrapper;
	}
	else if (viewData.mType == View::VIEW_REAL_TIME)
	{
		ViewWrapperVideoPtr wrapper(new ViewWrapperVideo(view, mBackend));
		return wrapper;
	}
	else
	{
		reportError(QString("Unknown view type %1").arg(qstring_cast(viewData.mType)));
	}

	return ViewWrapperPtr();
}

LayoutRepositoryPtr ViewManager::getLayoutRepository()
{
	return mLayoutRepository;
}

void ViewManager::onLayoutRepositoryChanged(QString uid)
{
	this->saveGlobalSettings();

	bool activeChange = mActiveLayout[0] == uid;
	if (activeChange)
	{
		mActiveLayout[0] = ""; // hack: force trigger a change
		this->setActiveLayout(uid, 0);
	}
}

void ViewManager::loadGlobalSettings()
{
	XmlOptionFile file = profile()->getXmlSettings().descend("viewmanager");
	mLayoutRepository->load(file);
}

void ViewManager::saveGlobalSettings()
{
	XmlOptionFile file = profile()->getXmlSettings().descend("viewmanager");
	mLayoutRepository->save(file);
	file.save();
}

QActionGroup* ViewManager::getInteractorStyleActionGroup()
{
	return mCameraStyleInteractor->getInteractorStyleActionGroup();
}

void ViewManager::updateCameraStyleActions()
{
	int active = this->getActiveViewGroup();
	int index = this->findGroupContaining3DViewGivenGuess(active);

	if (index<0)
	{
		mCameraStyleInteractor->connectCameraStyle(ViewGroupDataPtr());
	}
	else
	{
		ViewGroupPtr group = this->getViewGroups()[index];
		mCameraStyleInteractor->connectCameraStyle(group->getData());
		mCameraControl->setView(this->get3DView(index, 0));
	}
}

/**Look for the index'th 3DView in given group.
 */
int ViewManager::findGroupContaining3DViewGivenGuess(int preferredGroup)
{
	if (preferredGroup>=0)
		if (mViewGroups[preferredGroup]->contains3DView())
			return preferredGroup;

	for (unsigned i=0; i<mViewGroups.size(); ++i)
		if (mViewGroups[i]->contains3DView())
			return i;
	return -1;
}

void ViewManager::autoShowData(DataPtr data)
{
	if (settings()->value("Automation/autoShowNewData").toBool() && data)
	{
		this->autoShowInViewGroups(data);
		this->autoResetCameraToSuperiorView();
		this->autoCenterToImageCenter();
	}
}

CameraControlPtr ViewManager::getCameraControl() { return mCameraControl; }

void ViewManager::autoShowInViewGroups(DataPtr data)
{
	QList<unsigned> showInViewGroups = this->getViewGroupsToAutoShowIn();
	foreach (unsigned i, showInViewGroups)
		this->getViewGroups()[i]->getData()->addDataSorted(data->getUid());
}

QList<unsigned> ViewManager::getViewGroupsToAutoShowIn()
{
	QList<unsigned> showInViewGroups;
	if(settings()->value("Automation/autoShowNewDataInViewGroup0").toBool())
		showInViewGroups  << 0;
	if(settings()->value("Automation/autoShowNewDataInViewGroup1").toBool())
		showInViewGroups  << 1;
	if(settings()->value("Automation/autoShowNewDataInViewGroup2").toBool())
		showInViewGroups  << 2;
	if(settings()->value("Automation/autoShowNewDataInViewGroup3").toBool())
		showInViewGroups  << 3;
	if(settings()->value("Automation/autoShowNewDataInViewGroup4").toBool())
		showInViewGroups  << 4;
	return showInViewGroups;
}

void ViewManager::autoResetCameraToSuperiorView()
{
	if(settings()->value("Automation/autoResetCameraToSuperiorViewWhenAutoShowingNewData").toBool())
	{
		for (unsigned i=0; i<mViewGroups.size(); ++i)
			if (mViewGroups[i]->contains3DView())
			{
				mCameraControl->setView(this->get3DView(i));
				mCameraControl->setSuperiorView();
			}
	}
}

void ViewManager::autoCenterToImageCenter()
{
	if(settings()->value("Automation/autoCenterToImageCenterViewWhenAutoShowingNewData").toBool())
	{
		QList<unsigned> showInViewGroups = this->getViewGroupsToAutoShowIn();

		foreach (unsigned i, showInViewGroups)
			this->centerToImageCenterInViewGroup(i);
	}
}

void ViewManager::centerToImageCenterInViewGroup(unsigned groupNr)
{
	this->getNavigation(groupNr)->centerToDataInViewGroup(this->getViewGroup(groupNr));
}

CyclicActionLoggerPtr ViewManager::getRenderTimer()
{
	return mRenderLoop->getRenderTimer();
}

void ViewManager::setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx)
{
	//Set active view before changing camerastyle
	if (!mViewGroups[groupIdx]->getViews().empty())
		this->setActiveView(mViewGroups[groupIdx]->getViews()[0]->getUid());

	QList<QAction*> actions = this->getInteractorStyleActionGroup()->actions();
	for(int i = 0; i < actions.size(); ++i)
	{
		if (actions[i]->data().toString() == enum2string(style))
			actions[i]->trigger();
	}
}

void ViewManager::addDefaultLayout(LayoutData layoutData)
{
	mLayoutRepository->addDefault(layoutData);
}

void ViewManager::enableContextMenuForViews(bool enable)
{
	for(int i=0; i<mLayoutWidgets.size(); ++i)
	{
		ViewCollectionWidget* widget = mLayoutWidgets[i];
		if(widget)
			widget->enableContextMenuForViews(enable);
	}
}

//void ViewManager::setSharedOpenGLContextInViewGroups(SharedOpenGLContextPtr context)
//{
//	CX_LOG_DEBUG() << "ViewManager::setSharedOpenGLContextInViewGroups size:" << mViewGroups.size();
//	for(unsigned i = 0; i < mViewGroups.size(); ++i)
//		mViewGroups[i]->setSharedOpenGLContext(context);
//}

//void ViewManager::setSharedOpenGLContext(SharedOpenGLContextPtr context)
//{
//	CX_LOG_DEBUG() << "ViewManager::setSharedOpenGLContext";
//	this->setSharedOpenGLContextInViewGroups(context);
//}

//SharedOpenGLContextPtr ViewManager::getSharedOpenGLContext()
//{
//	return mSharedOpenGLContext;
//}

} //namespace cx
