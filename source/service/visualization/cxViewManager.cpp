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

#include "cxViewManager.h"

#include <QGridLayout>
#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QAction>
#include <QtCore>
#include <qtextstream.h>
#include <vtkRenderWindow.h>
#include <vtkImageData.h>
#include "cxViewWrapper2D.h"
//#include "sscGLHelpers.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "cxLayoutData.h"

#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "sscXmlOptionItem.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "sscSliceProxy.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxViewWrapperVideo.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxInteractiveCropper.h"
#include "vtkForwardDeclarations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxInteractiveClipper.h"
#include "sscImage.h"
#include "cxCameraStyle.h"
#include "cxCyclicActionLogger.h"
#include "cxLayoutWidget.h"
#include "cxRenderLoop.h"

#include "sscLogger.h"

namespace cx
{

ViewManager *ViewManager::mTheInstance = NULL;
ViewManager* viewManager()
{
	return ViewManager::getInstance();
}
ViewManager* ViewManager::getInstance()
{
	return mTheInstance;
}

ViewManager* ViewManager::createInstance()
{
	if (mTheInstance == NULL)
	{
		mTheInstance = new ViewManager();
		}
	return mTheInstance;
}

void ViewManager::destroyInstance()
{
	delete mTheInstance;
	mTheInstance = NULL;
}

ViewManager::ViewManager() :
				mGlobal2DZoom(true),
				mGlobalObliqueOrientation(false)
{
	mRenderLoop.reset(new RenderLoop());
	connect(mRenderLoop.get(), SIGNAL(preRender()), this, SLOT(updateViews()));
	connect(mRenderLoop.get(), SIGNAL(fps(int)), this, SIGNAL(fps(int)));

	mSlicePlanesProxy.reset(new SlicePlanesProxy());

	connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearSlot()));

	this->addDefaultLayouts();
	this->loadGlobalSettings();

	mRenderLoop->setSmartRender(settings()->value("smartRender", true).toBool());
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	const unsigned VIEW_GROUP_COUNT = 5; // set this to enough
	// initialize view groups:
	for (unsigned i = 0; i < VIEW_GROUP_COUNT; ++i)
	{
		mViewGroups.push_back(ViewGroupPtr(new ViewGroup()));
	}

	this->syncOrientationMode(SyncedValue::create(0));
}

ViewManager::~ViewManager()
{
}

void ViewManager::initialize()
{
	mCameraStyle.reset(new CameraStyle()); // uses the global viewmanager() instance - must be created after creation of this.

	mActiveLayout = QStringList() << "" << "";
	mLayoutWidgets.resize(mActiveLayout.size(), NULL);

	mInteractiveCropper.reset(new InteractiveCropper());
	mInteractiveClipper.reset(new InteractiveClipper());
	connect(this, SIGNAL(activeLayoutChanged()), mInteractiveClipper.get(), SIGNAL(changed()));
	connect(mInteractiveCropper.get(), SIGNAL(changed()), mRenderLoop.get(), SLOT(requestPreRenderSignal()));
	connect(mInteractiveClipper.get(), SIGNAL(changed()), mRenderLoop.get(), SLOT(requestPreRenderSignal()));

	// set start layout
	this->setActiveLayout("LAYOUT_3D_ACS_SINGLE", 0);

	mRenderLoop->setRenderingInterval(settings()->value("renderingInterval").toInt());
	mRenderLoop->start();

	mGlobalZoom2DVal = SyncedValue::create(1);
	this->setGlobal2DZoom(mGlobal2DZoom);
}

QWidget *ViewManager::getLayoutWidget(int index)
{
	SSC_ASSERT(index < mLayoutWidgets.size());
	if (!mLayoutWidgets[index])
	{
		mLayoutWidgets[index] = new LayoutWidget;
		this->rebuildLayouts();
	}
	return mLayoutWidgets[index];
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

std::map<QString, ImagePtr> ViewManager::getVisibleImages()
{
	std::map<QString, ImagePtr> retval;
	for(unsigned i=0; i<mViewGroups.size(); ++i)
	{
		ViewGroupPtr group = mViewGroups[i];
		std::vector<ImagePtr> images = group->getImages();
		for (unsigned j=0; j<images.size(); ++j)
		{
			retval[images[j]->getUid()] = images[j];
		}
	}
	return retval;
}

void ViewManager::duringSavePatientSlot()
{
	QDomElement managerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers");
	this->addXml(managerNode);
}

void ViewManager::duringLoadPatientSlot()
{
	QDomElement viewmanagerNode = patientService()->getPatientData()->getCurrentWorkingElement("managers/viewManager");
	this->parseXml(viewmanagerNode);
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
}

InteractiveClipperPtr ViewManager::getClipper()
{
	return mInteractiveClipper;
}

InteractiveCropperPtr ViewManager::getCropper()
{
	return mInteractiveCropper;
}

void ViewManager::setRegistrationMode(REGISTRATION_STATUS mode)
{
	ViewGroupDataPtr data = this->getViewGroups()[0]->getData();
	ViewGroupData::Options options = data->getOptions();

	options.mShowLandmarks = false;
	options.mShowPointPickerProbe = false;

	if (mode == rsIMAGE_REGISTRATED)
	{
		options.mShowLandmarks = true;
		options.mShowPointPickerProbe = true;
	}
	if (mode == rsPATIENT_REGISTRATED)
	{
		options.mShowLandmarks = true;
		options.mShowPointPickerProbe = false;
	}

	data->setOptions(options);
}

QString ViewManager::getActiveLayout(int widgetIndex) const
{
	SSC_ASSERT(mActiveLayout.size() > widgetIndex);
	return mActiveLayout[widgetIndex];
}

ViewWrapperPtr ViewManager::getActiveView() const
{
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		ViewWrapperPtr viewWrapper = mViewGroups[i]->getViewWrapperFromViewUid(mActiveView);
		if (viewWrapper)
		{
			return viewWrapper;
		}
	}
	return ViewWrapperPtr();
}

void ViewManager::setActiveView(QString viewUid)
{
	if (mActiveView == qstring_cast(viewUid))
		return;
	mActiveView = qstring_cast(viewUid);
	emit activeViewChanged();
}

int ViewManager::getActiveViewGroup() const
{
	int retval = -1;

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		ViewWrapperPtr viewWrapper = mViewGroups[i]->getViewWrapperFromViewUid(mActiveView);
		if (viewWrapper)
			retval = i;
	}

	return retval;
}

void ViewManager::syncOrientationMode(SyncedValuePtr val)
{
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->syncOrientationMode(val);
	}
}

void ViewManager::setGlobal2DZoom(bool global)
{
	mGlobal2DZoom = global;

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->setGlobal2DZoom(mGlobal2DZoom, mGlobalZoom2DVal);
	}
}

bool ViewManager::getGlobal2DZoom()
{
	return mGlobal2DZoom;
}

void ViewManager::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement viewManagerNode = doc.createElement("viewManager");
	parentNode.appendChild(viewManagerNode);

	QDomElement global2DZoomNode = doc.createElement("global2DZoom");
	global2DZoomNode.appendChild(doc.createTextNode(string_cast(mGlobal2DZoom).c_str()));
	viewManagerNode.appendChild(global2DZoomNode);

	QDomElement activeViewNode = doc.createElement("activeView");
	activeViewNode.appendChild(doc.createTextNode(mActiveView));
	viewManagerNode.appendChild(activeViewNode);

	QDomElement slicePlanes3DNode = doc.createElement("slicePlanes3D");
	slicePlanes3DNode.setAttribute("use", mSlicePlanesProxy->getVisible());
	slicePlanes3DNode.setAttribute("opaque", mSlicePlanesProxy->getDrawPlanes());
	viewManagerNode.appendChild(slicePlanes3DNode);

	QDomElement viewGroupsNode = doc.createElement("viewGroups");
	viewManagerNode.appendChild(viewGroupsNode);
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		QDomElement viewGroupNode = doc.createElement("viewGroup");
		viewGroupNode.setAttribute("index", i);
		viewGroupsNode.appendChild(viewGroupNode);

		mViewGroups[i]->addXml(viewGroupNode);
	}

	if (mInteractiveClipper)
	{
		QDomElement clippedImageNode = doc.createElement("clippedImage");
		QString clippedImage = (mInteractiveClipper->getImage()) ? mInteractiveClipper->getImage()->getUid() : "";
		clippedImageNode.appendChild(doc.createTextNode(clippedImage));
		viewManagerNode.appendChild(clippedImageNode);
	}
}

void ViewManager::parseXml(QDomNode viewmanagerNode)
{
	QString activeViewString;
	QDomNode child = viewmanagerNode.firstChild();
	while (!child.isNull())
	{
		if (child.toElement().tagName() == "global2DZoom")
		{
			const QString global2DZoomString = child.toElement().text();
			if (!global2DZoomString.isEmpty() && global2DZoomString.toInt() == 0)
				this->setGlobal2DZoom(false);
			else
				this->setGlobal2DZoom(true);
		}
		else if (child.toElement().tagName() == "activeView")
		{
			activeViewString = child.toElement().text();
		}
		else if (child.toElement().tagName() == "clippedImage")
		{
			QString clippedImage = child.toElement().text();
			mInteractiveClipper->setImage(dataManager()->getImage(clippedImage));
		}
		child = child.nextSibling();
	}

	QDomElement slicePlanes3DNode = viewmanagerNode.namedItem("slicePlanes3D").toElement();
	mSlicePlanesProxy->setVisible(slicePlanes3DNode.attribute("use").toInt());
	mSlicePlanesProxy->setDrawPlanes(slicePlanes3DNode.attribute("opaque").toInt());

	QDomElement viewgroups = viewmanagerNode.namedItem("viewGroups").toElement();
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

	this->setActiveView(activeViewString);
}

void ViewManager::clearSlot()
{
	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->clearPatientData();
	}
}

/**Look for the index'th 3DView in given group.
 */
ViewWidgetQPtr ViewManager::get3DView(int group, int index)
{
	int count = 0;
	std::vector<ViewWidgetQPtr> views = mViewGroups[group]->getViews();
	for (unsigned i = 0; i < views.size(); ++i)
	{
		if(!views[i])
			continue;
		if (views[i]->getType()!=View::VIEW_3D)
			continue;
		if (index == count++)
			return views[i];
	}
	return ViewWidgetQPtr();
}

/**deactivate the current layout, leaving an empty layout
 */
void ViewManager::deactivateCurrentLayout()
{
	mRenderLoop->clearViews();

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

/**Change layout from current to layout.
 */
void ViewManager::setActiveLayout(const QString& layout, int widgetIndex)
{
	SSC_ASSERT(mActiveLayout.size() > widgetIndex);

	if (mActiveLayout[widgetIndex] == layout)
		return;

	mActiveLayout[widgetIndex] = layout;

	this->rebuildLayouts();

	emit activeLayoutChanged();

	QString layoutName = this->getLayoutData(layout).getName();
	messageManager()->sendInfo(QString("Layout %1 changed to %2").arg(widgetIndex).arg(layoutName));
}

void ViewManager::rebuildLayouts()
{
	this->deactivateCurrentLayout();

	for (unsigned i=0; i<mLayoutWidgets.size(); ++i)
	{
		LayoutData next = this->getLayoutData(mActiveLayout[i]);
		if (mLayoutWidgets[i] && !next.getUid().isEmpty())
			this->activateViews(mLayoutWidgets[i], next);
	}

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
			foundSlice = foundSlice || wrappers[j]->getView()->getType() == ViewWidget::VIEW_2D;
		}
		if (foundSlice)
			break;
	}
}

void ViewManager::activateViews(LayoutWidget *widget, LayoutData next)
{
	if (!widget)
		return;

	for (LayoutData::iterator iter = next.begin(); iter != next.end(); ++iter)
	{
		LayoutData::ViewData view = *iter;

		if (view.mGroup < 0 || view.mPlane == ptCOUNT)
			continue;

		if (view.mPlane == ptNOPLANE || view.mPlane == ptCOUNT)
		{
			if (view.mType == ViewWidget::VIEW_3D)
				this->activate3DView(widget, view.mGroup, view.mRegion);
			else if (view.mType == ViewWidget::VIEW_REAL_TIME)
				this->activateRTStreamView(widget, view.mGroup, view.mRegion);
		}
		else
		{
			this->activate2DView(widget, view.mGroup, view.mPlane, view.mRegion);
		}
	}
}

void ViewManager::setRenderingInterval(int interval)
{
	mRenderLoop->setRenderingInterval(interval);
}

void ViewManager::activateView(LayoutWidget* widget, ViewWrapperPtr wrapper, int group, LayoutRegion region)
{
	ViewWidget* view = wrapper->getView();
	mRenderLoop->addView(view);
	mViewGroups[group]->addView(wrapper);
	widget->addView(view, region);

	view->show();
}

void ViewManager::activate2DView(LayoutWidget* widget, int group, PLANE_TYPE plane, LayoutRegion region)
{
	ViewWidget* view = widget->mViewCache2D->retrieveView();
	view->setType(View::VIEW_2D);

	ViewWrapper2DPtr wrapper(new ViewWrapper2D(view));
	wrapper->initializePlane(plane);
	this->activateView(widget, wrapper, group, region);
}

void ViewManager::activate3DView(LayoutWidget* widget, int group, LayoutRegion region)
{
	ViewWidget* view = widget->mViewCache3D->retrieveView();
	view->setType(View::VIEW_3D);
	ViewWrapper3DPtr wrapper(new ViewWrapper3D(group + 1, view));
	if (group == 0)
	{
		mInteractiveCropper->setView(view);
	}

	this->activateView(widget, wrapper, group, region);
}

void ViewManager::activateRTStreamView(LayoutWidget *widget, int group, LayoutRegion region)
{
	ViewWidget* view = widget->mViewCacheRT->retrieveView();
	view->setType(View::VIEW_REAL_TIME);
	ViewWrapperVideoPtr wrapper(new ViewWrapperVideo(view));
	this->activateView(widget, wrapper, group, region);
}

void ViewManager::addDefaultLayout(LayoutData data)
{
	mDefaultLayouts.push_back(data.getUid());
	mLayouts.push_back(data);
}

/** insert the hardcoded layouts into mLayouts.
 *
 */
void ViewManager::addDefaultLayouts()
{
	mDefaultLayouts.clear();

	/*
	 *
	 3D______________

	 3D
	 3D AD
	 3D ACS

	 Oblique ________

	 3D AnyDual x1
	 3D AnyDual x2
	 AnyDual x3

	 Orthogonal______

	 3D ACS x1
	 3D ACS x2
	 ACS x3

	 RT______________

	 RT
	 Us Acq
	 */

	// ------------------------------------------------------
	// --- group of 3D-based layouts ------------------------
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_3D", "3D"));
	{
		// 3D only
		LayoutData layout = LayoutData::create("LAYOUT_3D", "3D", 1, 1);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		this->addDefaultLayout(layout);
	}
	{
		// 3D ACS
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS", "3D ACS", 3, 4);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 3));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 3));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D Any
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD", "3D AnyDual", 2, 4);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D ACS in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS_SINGLE", "3D ACS Connected", 3, 4);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(0, ptAXIAL, LayoutRegion(0, 3));
		layout.setView(0, ptCORONAL, LayoutRegion(1, 3));
		layout.setView(0, ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D Any in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD_SINGLE", "3D AnyDual Connected", 2, 4);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(0, ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(0, ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of oblique (Anyplane-based) layouts --------
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_Oblique", "Oblique"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x1", "3D Any Dual x1", 1, 3);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(0, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x2", "3D Any Dual x2", 2, 3);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 1));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_AnyDual_x3", "Any Dual x3", 2, 3);
		layout.setView(0, ptANYPLANE, LayoutRegion(0, 0));
		layout.setView(0, ptSIDEPLANE, LayoutRegion(1, 0));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of orthogonal (ACS-based) layouts ----------
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_Orthogonal", "Orthogonal"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x1", "3D ACS x1", 2, 2);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 0));
		layout.setView(1, ptSAGITTAL, LayoutRegion(1, 1));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x2", "3D ACS x2", 3, 3);
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 1));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x3", "ACS x3", 3, 3);
		layout.setView(0, ptAXIAL, LayoutRegion(0, 0));
		layout.setView(0, ptCORONAL, LayoutRegion(1, 0));
		layout.setView(0, ptSAGITTAL, LayoutRegion(2, 0));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of RTsource-based layouts - single viewgroup
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_RT", "Realtime Source"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_RT_1X1", "RT", 1, 1);
		layout.setView(0, ViewWidget::VIEW_REAL_TIME, LayoutRegion(0, 0));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_US_Acquisition", "US Acquisition", 2, 3);
		layout.setView(0, ptANYPLANE, LayoutRegion(1, 2, 1, 1));
		layout.setView(0, ViewWidget::VIEW_3D, LayoutRegion(0, 2, 1, 1));
		layout.setView(0, ViewWidget::VIEW_REAL_TIME, LayoutRegion(0, 0, 2, 2));
		this->addDefaultLayout(layout);
	}
}

LayoutData ViewManager::getLayoutData(const QString uid) const
{
	unsigned pos = this->findLayoutData(uid);
	if (pos != mLayouts.size())
		return mLayouts[pos];
	return LayoutData();
}

std::vector<QString> ViewManager::getAvailableLayouts() const
{
	std::vector<QString> retval;
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		retval.push_back(mLayouts[i].getUid());
	}
	return retval;
}

void ViewManager::setLayoutData(const LayoutData& data)
{
	this->storeLayoutData(data);

	bool activeChange = mActiveLayout[0] == data.getUid();
	if (activeChange)
	{
		mActiveLayout[0] = "";
		this->setActiveLayout(data.getUid(), 0);
		emit activeLayoutChanged();
	}
}

void ViewManager::storeLayoutData(const LayoutData& data)
{
	unsigned pos = this->findLayoutData(data.getUid());
	if (pos == mLayouts.size())
		mLayouts.push_back(data);
	else
		mLayouts[pos] = data;

	this->saveGlobalSettings();
}

QString ViewManager::generateLayoutUid() const
{
	int count = 0;

	for (LayoutDataVector::const_iterator iter = mLayouts.begin(); iter != mLayouts.end(); ++iter)
	{
		if (iter->getUid() == qstring_cast(count))
			count = iter->getUid().toInt() + 1;
	}
	return qstring_cast(count);
}

void ViewManager::deleteLayoutData(const QString uid)
{
	mLayouts.erase(mLayouts.begin() + findLayoutData(uid));
	this->saveGlobalSettings();
	emit activeLayoutChanged();
}

unsigned ViewManager::findLayoutData(const QString uid) const
{
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (mLayouts[i].getUid() == uid)
			return i;
	}
	return mLayouts.size();
}

bool ViewManager::isCustomLayout(const QString& uid) const
{
	bool isLayout = false;
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (uid == mLayouts[i].getUid())
		{
			isLayout = true;
			break;
		}
	}

	bool isDefaultLayout = std::count(mDefaultLayouts.begin(), mDefaultLayouts.end(), uid);

	bool retval = false;
	if (isLayout && !isDefaultLayout)
		retval = true;

	return retval;
}

void ViewManager::loadGlobalSettings()
{
	XmlOptionFile file = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("viewmanager");

	// load custom layouts:
	mLayouts.clear();

	QDomElement layouts = file.getElement("layouts");
	QDomNode layout = layouts.firstChild();
	for (; !layout.isNull(); layout = layout.nextSibling())
	{
		if (layout.toElement().tagName() != "layout")
			continue;

		LayoutData data;
		data.parseXml(layout);

		unsigned pos = this->findLayoutData(data.getUid());
		if (pos == mLayouts.size())
			mLayouts.push_back(data);
		else
			mLayouts[pos] = data;
	}

	this->addDefaultLayouts(); // ensure we overwrite loaded layouts
}

void ViewManager::saveGlobalSettings()
{
	XmlOptionFile file = XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("viewmanager");

	XmlOptionFile layoutsNode = file.descend("layouts");
	layoutsNode.removeChildren();
	for (LayoutDataVector::iterator iter = mLayouts.begin(); iter != mLayouts.end(); ++iter)
	{
		if (!this->isCustomLayout(iter->getUid()))
			continue; // dont store default layouts - they are created automatically.

		QDomElement layoutNode = file.getDocument().createElement("layout");
		layoutsNode.getElement().appendChild(layoutNode);
		iter->addXml(layoutNode);
	}

	file.save();
}

QActionGroup* ViewManager::createInteractorStyleActionGroup()
{
	return mCameraStyle->createInteractorStyleActionGroup();
}

void ViewManager::autoShowData(DataPtr data)
{
	if (settings()->value("Automation/autoShowNewData").toBool())
	{
		this->getViewGroups()[0]->getData()->addDataSorted(data);
	}
}

CyclicActionLoggerPtr ViewManager::getRenderTimer()
{
	return mRenderLoop->getRenderTimer();
}


} //namespace cx
