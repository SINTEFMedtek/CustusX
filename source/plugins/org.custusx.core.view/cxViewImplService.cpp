/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewImplService.h"

#include <QAction>
#include <ctkPluginContext.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include "cxViewGroup.h"
#include "cxRepManager.h"
#include "cxVisServices.h"
#include "cxSessionStorageServiceProxy.h"
#include "cxXMLNodeWrapper.h"
#include "cxLogger.h"
#include "cxViewGroupData.h"
#include "cxClippers.h"
#include "cxRenderWindowFactory.h"
#include "cxRenderLoop.h"
#include "cxViewImplService.h"
#include "cxSlicePlanes3DRep.h"
#include "cxLayoutRepository.h"
#include "cxCameraControl.h"
#include "cxCameraStyleInteractor.h"
#include "cxSettings.h"
#include "cxPatientModelService.h"
#include "cxSyncedValue.h"
#include "cxInteractiveCropper.h"
#include "cxNavigation.h"
#include "cxViewCollectionWidget.h"
#include "cxViewWrapper.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxViewWrapperVideo.h"
#include "cxProfile.h"

namespace cx
{

ViewImplService::ViewImplService(ctkPluginContext *context) :
	mContext(context )
{
	mServices = VisServices::create(context);
	mSession = SessionStorageServiceProxy::create(mContext);
	mClippers = ClippersPtr(new Clippers(mServices));

	connect(mSession.get(), &SessionStorageService::sessionChanged, this, &ViewImplService::onSessionChanged);
	connect(mSession.get(), &SessionStorageService::cleared, this, &ViewImplService::onSessionCleared);
	connect(mSession.get(), &SessionStorageService::isLoading, this, &ViewImplService::onSessionLoad);
	connect(mSession.get(), &SessionStorageService::isSaving, this, &ViewImplService::onSessionSave);

	this->init();
}

ViewImplService::~ViewImplService()
{
}

void ViewImplService::init()
{
	mRenderWindowFactory = RenderWindowFactoryPtr(new RenderWindowFactory());

	mRenderLoop.reset(new RenderLoop());
	connect(mRenderLoop.get(), &RenderLoop::preRender, this, &ViewImplService::updateViews);
	connect(mRenderLoop.get(), &RenderLoop::fps, this, &ViewImplService::fps);
	connect(mRenderLoop.get(), &RenderLoop::renderFinished, this, &ViewImplService::renderFinished);

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
		ViewGroupPtr group(new ViewGroup(mServices, QString::number(i)));
		mViewGroups.push_back(group);
	}

	// moved here from initialize() ... ensures object is fully callable after construction
	mCameraStyleInteractor.reset(new CameraStyleInteractor);

	mActiveLayout = QStringList() << "" << "";
	mLayoutWidgets.resize(mActiveLayout.size(), NULL);

	mInteractiveCropper.reset(new InteractiveCropper(mServices->patient()->getActiveData()));
	connect(this, SIGNAL(activeViewChanged()), this, SLOT(updateCameraStyleActions()));

	this->loadGlobalSettings();
	// connect to layoutrepo after load of global
	connect(mLayoutRepository.get(), &LayoutRepository::layoutChanged, this, &ViewImplService::onLayoutRepositoryChanged);
	this->initializeGlobal2DZoom();
	this->initializeActiveView();

	// set start layout
	this->setActiveLayout("LAYOUT_3D_ACS_SINGLE", 0);

	mRenderLoop->setRenderingInterval(settings()->value("renderingInterval").toInt());
	this->enableRender(true);
}

std::vector<ViewGroupPtr> ViewImplService::getViewGroups()
{
	return mViewGroups;
}

void ViewImplService::enableRender(bool val)
{
	if (val)
		mRenderLoop->start();
	else
		mRenderLoop->stop();

	emit renderingEnabledChanged();
}

bool ViewImplService::renderingIsEnabled() const
{
	return mRenderLoop->isRunning();
}


void ViewImplService::initializeGlobal2DZoom()
{
	mGlobal2DZoomVal = SyncedValue::create(1);

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
		mViewGroups[i]->getData()->initializeGlobal2DZoom(mGlobal2DZoomVal);
}

void ViewImplService::initializeActiveView()
{
	mActiveView = SyncedValue::create("");
	connect(mActiveView.get(), SIGNAL(changed()), this, SIGNAL(activeViewChanged()));

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
		mViewGroups[i]->initializeActiveView(mActiveView);
}


NavigationPtr ViewImplService::getNavigation(int group)
{
	mCameraControl->refreshView(this->get3DView(group));
	return NavigationPtr(new Navigation(mServices, mCameraControl));
}

QWidget *ViewImplService::getLayoutWidget(int index)
{
	if (index >= mLayoutWidgets.size())
		return NULL;
   return mLayoutWidgets[index];
}

QWidget *ViewImplService::createLayoutWidget(QWidget* parent, int index)
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

		connect(mLayoutWidgets[index].data(), &QObject::destroyed, this, &ViewImplService::layoutWidgetDestroyed);
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
void ViewImplService::layoutWidgetDestroyed(QObject* object)
{
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

void ViewImplService::updateViews()
{
	for(unsigned i=0; i<mViewGroups.size(); ++i)
	{
		ViewGroupPtr group = mViewGroups[i];
		for (unsigned j=0; j<group->getWrappers().size(); ++j)
			group->getWrappers()[j]->updateView();
	}
}

void ViewImplService::settingsChangedSlot(QString key)
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

InteractiveCropperPtr ViewImplService::getCropper()
{
	return mInteractiveCropper;
}

QString ViewImplService::getActiveLayout(int widgetIndex) const
{
	CX_ASSERT(mActiveLayout.size() > widgetIndex);
	return mActiveLayout[widgetIndex];
}

QString ViewImplService::getActiveView() const
{
	return mActiveView->get().value<QString>();
}

void ViewImplService::setActiveView(QString uid)
{
	mActiveView->set(uid);
}

int ViewImplService::getActiveGroupId() const
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

void ViewImplService::addXml(QDomNode& parentNode)
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

void ViewImplService::parseXml(QDomNode viewmanagerNode)
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

void ViewImplService::clear()
{
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->clearPatientData();
	}
}

/**Look for the index'th 3DView in given group.
 */
ViewPtr ViewImplService::get3DView(int group, int index)
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
void ViewImplService::deactivateCurrentLayout()
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

ViewGroupDataPtr ViewImplService::getGroup(int groupIdx) const
{
	if (( groupIdx>=0 )&&( groupIdx < mViewGroups.size() ))
		return mViewGroups[groupIdx]->getData();
	return ViewGroupDataPtr();
}

unsigned ViewImplService::viewGroupCount() const
{
	int count = 0;
	while(this->getGroup(count))
		++count;
	return count;
}

/**Change layout from current to layout.
 */
void ViewImplService::setActiveLayout(const QString& layout, int widgetIndex)
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

void ViewImplService::rebuildLayouts()
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

void ViewImplService::setSlicePlanesProxyInViewsUpTo2DViewgroup()
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
void ViewImplService::activateViews(ViewCollectionWidget *widget, LayoutData next)
{
	if (!widget)
		return;

	widget->setOffScreenRenderingAndClear(next.getOffScreenRendering());

	for (LayoutData::iterator iter = next.begin(); iter != next.end(); ++iter)
		this->activateView(widget, *iter);
}

void ViewImplService::setRenderingInterval(int interval)
{
	mRenderLoop->setRenderingInterval(interval);
}

void ViewImplService::activateView(ViewCollectionWidget* widget, LayoutViewData viewData)
{
	if (!viewData.isValid())
		return;

	ViewPtr view = widget->addView(viewData.mType, viewData.mRegion);


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
		CX_LOG_WARNING() << "ViewImplService::activateView: got not shared OpenGL context";
	mViewGroups[viewData.mGroup]->addView(wrapper, mRenderWindowFactory->getSharedOpenGLContext());
}

ViewWrapperPtr ViewImplService::createViewWrapper(ViewPtr view, LayoutViewData viewData)
{
	if (viewData.mType == View::VIEW_2D)
	{
		ViewWrapper2DPtr wrapper(new ViewWrapper2D(view, mServices));
		wrapper->initializePlane(viewData.mPlane);
		connect(wrapper.get(), &ViewWrapper2D::pointSampled, this, &ViewImplService::pointSampled);
		return wrapper;
	}
	else if (viewData.mType == View::VIEW_3D)
	{

		ViewWrapper3DPtr wrapper(new ViewWrapper3D(viewData.mGroup + 1, view, mServices));
		if (viewData.mGroup == 0)
			mInteractiveCropper->setView(view);
		return wrapper;
	}
	else if (viewData.mType == View::VIEW_REAL_TIME)
	{
		ViewWrapperVideoPtr wrapper(new ViewWrapperVideo(view, mServices));
		return wrapper;
	}
	else
	{
		reportError(QString("Unknown view type %1").arg(qstring_cast(viewData.mType)));
	}

	return ViewWrapperPtr();
}

LayoutRepositoryPtr ViewImplService::getLayoutRepository()
{
	return mLayoutRepository;
}

void ViewImplService::onLayoutRepositoryChanged(QString uid)
{
	this->saveGlobalSettings();

	bool activeChange = mActiveLayout[0] == uid;
	if (activeChange)
	{
		mActiveLayout[0] = ""; // hack: force trigger a change
		this->setActiveLayout(uid, 0);
	}
}

void ViewImplService::loadGlobalSettings()
{
	XmlOptionFile file = profile()->getXmlSettings().descend("viewmanager");
	mLayoutRepository->load(file);
}

void ViewImplService::saveGlobalSettings()
{
	XmlOptionFile file = profile()->getXmlSettings().descend("viewmanager");
	mLayoutRepository->save(file);
	file.save();
}

QActionGroup* ViewImplService::getInteractorStyleActionGroup()
{
	return mCameraStyleInteractor->getInteractorStyleActionGroup();
}

void ViewImplService::updateCameraStyleActions()
{
	int active = this->getActiveGroupId();
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
int ViewImplService::findGroupContaining3DViewGivenGuess(int preferredGroup)
{
	if (preferredGroup>=0)
		if (mViewGroups[preferredGroup]->contains3DView())
			return preferredGroup;

	for (unsigned i=0; i<mViewGroups.size(); ++i)
		if (mViewGroups[i]->contains3DView())
			return i;
	return -1;
}

void ViewImplService::autoShowData(DataPtr data)
{
	if (settings()->value("Automation/autoShowNewData").toBool() && data)
	{
		this->autoShowInViewGroups(data);
		this->autoResetCameraToSuperiorView();
		this->autoCenterToImageCenter();
	}
}

CameraControlPtr ViewImplService::getCameraControl() { return mCameraControl; }

void ViewImplService::autoShowInViewGroups(DataPtr data)
{
	QList<unsigned> showInViewGroups = this->getViewGroupsToAutoShowIn();
	foreach (unsigned i, showInViewGroups)
		this->getViewGroups()[i]->getData()->addDataSorted(data->getUid());
}

QList<unsigned> ViewImplService::getViewGroupsToAutoShowIn()
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

void ViewImplService::autoResetCameraToSuperiorView()
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

void ViewImplService::autoCenterToImageCenter()
{
	if(settings()->value("Automation/autoCenterToImageCenterViewWhenAutoShowingNewData").toBool())
	{
		QList<unsigned> showInViewGroups = this->getViewGroupsToAutoShowIn();

		foreach (unsigned i, showInViewGroups)
			this->centerToImageCenterInViewGroup(i);
	}
}

void ViewImplService::centerToImageCenterInViewGroup(unsigned groupNr)
{
	this->getNavigation(groupNr)->centerToDataInViewGroup(this->getGroup(groupNr));
}

CyclicActionLoggerPtr ViewImplService::getRenderTimer()
{
	return mRenderLoop->getRenderTimer();
}

void ViewImplService::setCameraStyle(CAMERA_STYLE_TYPE style, int groupIdx)
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

void ViewImplService::zoomCamera3D(int viewGroup3DNumber, int zoomFactor)
{
	ViewGroupDataPtr viewGroup3D = this->getGroup(viewGroup3DNumber);
	if(!viewGroup3D)
		return;

	viewGroup3D->zoomCamera3D(zoomFactor);
}

void ViewImplService::addDefaultLayout(LayoutData layoutData)
{
	mLayoutRepository->addDefault(layoutData);
}

void ViewImplService::enableContextMenuForViews(bool enable)
{
	for(int i=0; i<mLayoutWidgets.size(); ++i)
	{
		ViewCollectionWidget* widget = mLayoutWidgets[i];
		if(widget)
			widget->enableContextMenuForViews(enable);
	}
}

void ViewImplService::setRegistrationMode(REGISTRATION_STATUS mode)
{
	this->getGroup(0)->setRegistrationMode(mode);
}

bool ViewImplService::isNull()
{
	return false;
}

void ViewImplService::aboutToStop()
{
	this->enableRender(false);
}

ClippersPtr ViewImplService::getClippers()
{
	return this->mClippers;
}

void ViewImplService::centerToImageCenterInActiveViewGroup()
{
	this->centerToImageCenterInViewGroup(this->getActiveGroupId());
}

void ViewImplService::onSessionChanged()
{
}
void ViewImplService::onSessionCleared()
{
	this->clear();
}

//TODO: save/load clippers
void ViewImplService::onSessionLoad(QDomElement& node)
{
	XMLNodeParser root(node);
	QDomElement viewManagerNode = root.descend("managers/viewManager").node().toElement();
	if (!viewManagerNode.isNull())
		this->parseXml(viewManagerNode);

	mClippers->parseXml(node);
}
void ViewImplService::onSessionSave(QDomElement& node)
{
	XMLNodeAdder root(node);
	QDomElement managerNode = root.descend("managers").node().toElement();
	this->addXml(managerNode);
	mClippers->addXml(node);
}

} /* namespace cx */
