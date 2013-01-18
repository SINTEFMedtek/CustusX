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

//#include "sscView.h"

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
#include "sscGLHelpers.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleUnicam.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkInteractorStyleTrackballActor.h"
#include "vtkInteractorStyleFlight.h"

#include "sscVolumetricRep.h"
#include "sscMessageManager.h"
#include "sscXmlOptionItem.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscSlicePlanes3DRep.h"
#include "sscSliceProxy.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxViewWrapper2D.h"
#include "cxViewWrapper3D.h"
#include "cxViewWrapperVideo.h"
#include "cxSettings.h"
#include "cxDataLocations.h"
#include "cxInteractiveCropper.h"
#include "cxRenderTimer.h"
#include "vtkForwardDeclarations.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxInteractiveClipper.h"
#include "sscImage.h"

namespace cx
{

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------

ViewManager *ViewManager::mTheInstance = NULL;
ViewManager* viewManager()
{
	return ViewManager::getInstance();
}
ViewManager* ViewManager::getInstance()
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
				mLayout(new QGridLayout()),
				mMainWindowsCentralWidget(new QWidget()),
				mRenderingTimer(new QTimer(this)),
				mGlobal2DZoom(true),
				mGlobalObliqueOrientation(false),
				mModified(false),
				mViewCache2D(mMainWindowsCentralWidget,	"View2D"),
				mViewCache3D(mMainWindowsCentralWidget, "View3D"),
				mViewCacheRT(mMainWindowsCentralWidget, "ViewRT")
{
	mRenderTimer.reset(new CyclicActionTimer("Main Render timer"));
	mSlicePlanesProxy.reset(new ssc::SlicePlanesProxy());
//	mSlicePlanesProxy->getProperties().m3DFontSize = 50;

	connect(patientService()->getPatientData().get(), SIGNAL(isSaving()), this, SLOT(duringSavePatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(isLoading()), this, SLOT(duringLoadPatientSlot()));
	connect(patientService()->getPatientData().get(), SIGNAL(cleared()), this, SLOT(clearSlot()));

	this->addDefaultLayouts();
	this->loadGlobalSettings();

	mSmartRender = settings()->value("smartRender", true).toBool();
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	mLayout->setSpacing(2);
	mLayout->setMargin(4);
	mMainWindowsCentralWidget->setLayout(mLayout);

	const unsigned VIEW_GROUP_COUNT = 5; // set this to enough
	// initialize view groups:
	for (unsigned i = 0; i < VIEW_GROUP_COUNT; ++i)
	{
		mViewGroups.push_back(ViewGroupPtr(new ViewGroup()));
	}

	mInteractiveCropper.reset(new InteractiveCropper());
	mInteractiveClipper.reset(new InteractiveClipper());
	connect(this, SIGNAL(activeLayoutChanged()), mInteractiveClipper.get(), SIGNAL(changed()));
	connect(mInteractiveCropper.get(), SIGNAL(changed()), this, SLOT(setModifiedSlot()));
	connect(mInteractiveClipper.get(), SIGNAL(changed()), this, SLOT(setModifiedSlot()));

	this->syncOrientationMode(SyncedValue::create(0));

	// set start layout
	this->setActiveLayout("LAYOUT_3D_ACS_SINGLE");

	this->setRenderingInterval(settings()->value("renderingInterval").toInt());

	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(renderAllViewsSlot()));

	mGlobalZoom2DVal = SyncedValue::create(1);
	this->setGlobal2DZoom(mGlobal2DZoom);
}

ViewManager::~ViewManager()
{
}

void ViewManager::setModifiedSlot()
{
	mModified = true;
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

std::map<QString, ssc::ImagePtr> ViewManager::getVisibleImages()
{
	std::map<QString, ssc::ImagePtr> retval;
	for(unsigned i=0; i<mViewGroups.size(); ++i)
	{
		ViewGroupPtr group = mViewGroups[i];
		std::vector<ssc::ImagePtr> images = group->getImages();
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
		mSmartRender = settings()->value("smartRender", true).toBool();
	}
	if (key == "renderingInterval")
	{
		this->setRenderingInterval(settings()->value("renderingInterval").toInt());
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

void ViewManager::setRegistrationMode(ssc::REGISTRATION_STATUS mode)
{
	ViewGroupDataPtr data = this->getViewGroups()[0]->getData();
	ViewGroupData::Options options = data->getOptions();

	options.mShowLandmarks = false;
	options.mShowPointPickerProbe = false;

	if (mode == ssc::rsIMAGE_REGISTRATED)
	{
		options.mShowLandmarks = true;
		options.mShowPointPickerProbe = true;
	}
	if (mode == ssc::rsPATIENT_REGISTRATED)
	{
		options.mShowLandmarks = true;
		options.mShowPointPickerProbe = false;
	}

	data->setOptions(options);
}

QString ViewManager::getActiveLayout() const
{
	return mActiveLayout;
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
//  ssc::messageManager()->sendInfo("Active view set to ["+mActiveView + "]");
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

//  QDomElement activeLayoutNode = doc.createElement("activeLayout");
//  activeLayoutNode.appendChild(doc.createTextNode(mActiveLayout));
//  viewManagerNode.appendChild(activeLayoutNode);

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

	QDomElement clippedImageNode = doc.createElement("clippedImage");
	QString clippedImage = (mInteractiveClipper->getImage()) ? mInteractiveClipper->getImage()->getUid() : "";
	clippedImageNode.appendChild(doc.createTextNode(clippedImage));
	viewManagerNode.appendChild(clippedImageNode);
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
// removed because the layout is better stored in the global settings file (bug #364)
//    else if(child.toElement().tagName() == "activeLayout")
//    {
//      const QString activeLayoutString = child.toElement().text();
//      if(!activeLayoutString.isEmpty())
//        this->setActiveLayout(activeLayoutString);
//    }
		else if (child.toElement().tagName() == "activeView")
		{
			activeViewString = child.toElement().text();
			//set active view after all viewgroups are properly set up
			//if(!activeViewString.isEmpty())
			//this->setActiveView(getViewWrapper(activeViewString.toStdString()));
		}
		else if (child.toElement().tagName() == "clippedImage")
		{
			QString clippedImage = child.toElement().text();
//			std::cout << "ClippedImage  " << clippedImage << std::endl;
			mInteractiveClipper->setImage(ssc::dataManager()->getImage(clippedImage));
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

QWidget* ViewManager::stealCentralWidget()
{
	return mMainWindowsCentralWidget;
}

/**Look for the index'th 3DView in given group.
 */
View3DQPtr ViewManager::get3DView(int group, int index)
{
	int count = 0;
	std::vector<ViewWidgetQPtr> views = mViewGroups[group]->getViews();
	for (unsigned i = 0; i < views.size(); ++i)
	{
		if(!views[i])
			continue;
		View3DQPtr retval = dynamic_cast<View3D*>(views[i].data());
		if (!retval)
			continue;
		if (index == count++)
			return retval;
	}
	return View3DQPtr();
}

/**deactivate the current layout, leaving an empty layout
 */
void ViewManager::deactivateCurrentLayout()
{
	mViewCache2D.clearUsedViews();
	mViewCache3D.clearUsedViews();
	mViewCacheRT.clearUsedViews();
	mViewMap.clear();

	for (unsigned i = 0; i < mViewGroups.size(); ++i)
	{
		mViewGroups[i]->removeViews();
	}

	this->setStretchFactors(LayoutRegion(0, 0, 10, 10), 0);
	this->setActiveView("");
	mSlicePlanesProxy->clearViewports();
}

/**Change layout from current to layout.
 */
void ViewManager::setActiveLayout(const QString& layout)
{
	if (mActiveLayout == layout)
		return;

//	std::cout << "ViewManager::setActiveLayout " << layout << std::endl;

	LayoutData next = this->getLayoutData(layout);
	if (next.getUid().isEmpty())
		return;

	this->deactivateCurrentLayout();

//  std::cout << streamXml2String(next) << std::endl;

	for (LayoutData::iterator iter = next.begin(); iter != next.end(); ++iter)
	{
		LayoutData::ViewData view = *iter;

		if (view.mGroup < 0 || view.mPlane == ssc::ptCOUNT)
			continue;

		if (view.mPlane == ssc::ptNOPLANE || view.mPlane == ssc::ptCOUNT)
		{
			if (view.mType == ssc::ViewWidget::VIEW_3D)
				this->activate3DView(view.mGroup, view.mRegion);
			else if (view.mType == ssc::ViewWidget::VIEW_REAL_TIME)
				this->activateRTStreamView(view.mGroup, view.mRegion);
		}
		else
		{
			this->activate2DView(view.mGroup, view.mPlane, view.mRegion);
		}
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
			foundSlice = foundSlice || wrappers[j]->getView()->getType() == ssc::ViewWidget::VIEW_2D;
		}
		if (foundSlice)
			break;
	}

	mActiveLayout = layout;
	emit
	activeLayoutChanged();

	ssc::messageManager()->sendInfo("Layout changed to " + this->getLayoutData(mActiveLayout).getName());
}

void ViewManager::setRenderingInterval(int interval)
{
    if (interval==mRenderingTimer->interval())
        return;

	mRenderingTimer->stop();
	if (interval == 0)
		interval = 30;
	mRenderingTimer->start(interval);
}

/** Set the stretch factors of columns and rows in mLayout.
 */
void ViewManager::setStretchFactors(LayoutRegion region, int stretchFactor)
{
	// set stretch factors for the affected cols to 1 in order to get even distribution
	for (int i = region.pos.col; i < region.pos.col + region.span.col; ++i)
	{
		mLayout->setColumnStretch(i, stretchFactor);
	}
	// set stretch factors for the affected rows to 1 in order to get even distribution
	for (int i = region.pos.row; i < region.pos.row + region.span.row; ++i)
	{
		mLayout->setRowStretch(i, stretchFactor);
	}
}

void ViewManager::activateView(ViewWrapperPtr wrapper, int group, LayoutRegion region)
{
	ssc::ViewWidget* view = wrapper->getView();
	mViewMap[view->getUid()] = view;
	mViewGroups[group]->addView(wrapper);
	mLayout->addWidget(view, region.pos.row, region.pos.col, region.span.row, region.span.col);
	this->setStretchFactors(region, 1);

	view->show();
}

void ViewManager::activate2DView(int group, ssc::PLANE_TYPE plane, LayoutRegion region)
{
	View2D* view = mViewCache2D.retrieveView();
	// use only default color for 2d views.
//  QColor background = settings()->value("backgroundColor").value<QColor>();
//  view->setBackgroundColor(background);
	ViewWrapper2DPtr wrapper(new ViewWrapper2D(view));
	wrapper->initializePlane(plane);
	this->activateView(wrapper, group, region);
}

void ViewManager::activate3DView(int group, LayoutRegion region)
{
	View3D* view = mViewCache3D.retrieveView();
//  moved to wrapper
//  QColor background = settings()->value("backgroundColor").value<QColor>();
//  view->setBackgroundColor(background);
	ViewWrapper3DPtr wrapper(new ViewWrapper3D(group + 1, view));
	if (group == 0)
	{
		mInteractiveCropper->setView(view);
	}

	this->activateView(wrapper, group, region);
}

void ViewManager::activateRTStreamView(int group, LayoutRegion region)
{
	ssc::ViewWidget* view = mViewCacheRT.retrieveView();
//  QColor background = settings()->value("backgroundColor").value<QColor>();
//  view->setBackgroundColor(background);
	ViewWrapperVideoPtr wrapper(new ViewWrapperVideo(view));
	this->activateView(wrapper, group, region);
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
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		this->addDefaultLayout(layout);
	}
	{
		// 3D ACS
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS", "3D ACS", 3, 4);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(1, ssc::ptAXIAL, LayoutRegion(0, 3));
		layout.setView(1, ssc::ptCORONAL, LayoutRegion(1, 3));
		layout.setView(1, ssc::ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D Any
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD", "3D AnyDual", 2, 4);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(1, ssc::ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(1, ssc::ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D ACS in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS_SINGLE", "3D ACS Connected", 3, 4);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(0, ssc::ptAXIAL, LayoutRegion(0, 3));
		layout.setView(0, ssc::ptCORONAL, LayoutRegion(1, 3));
		layout.setView(0, ssc::ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefaultLayout(layout);
	}
	{
		// 3D Any in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD_SINGLE", "3D AnyDual Connected", 2, 4);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(0, ssc::ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(0, ssc::ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of oblique (Anyplane-based) layouts --------
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_Oblique", "Oblique"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x1", "3D Any Dual x1", 1, 3);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ssc::ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptSIDEPLANE, LayoutRegion(0, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x2", "3D Any Dual x2", 2, 3);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 2, 1));
		layout.setView(1, ssc::ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ssc::ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ssc::ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_AnyDual_x3", "Any Dual x3", 2, 3);
		layout.setView(0, ssc::ptANYPLANE, LayoutRegion(0, 0));
		layout.setView(0, ssc::ptSIDEPLANE, LayoutRegion(1, 0));
		layout.setView(1, ssc::ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ssc::ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ssc::ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of orthogonal (ACS-based) layouts ----------
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_Orthogonal", "Orthogonal"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x1", "3D ACS x1", 2, 2);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ssc::ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptCORONAL, LayoutRegion(1, 0));
		layout.setView(1, ssc::ptSAGITTAL, LayoutRegion(1, 1));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x2", "3D ACS x2", 3, 3);
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 0, 3, 1));
		layout.setView(1, ssc::ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ssc::ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ssc::ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ssc::ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ssc::ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x3", "ACS x3", 3, 3);
		layout.setView(0, ssc::ptAXIAL, LayoutRegion(0, 0));
		layout.setView(0, ssc::ptCORONAL, LayoutRegion(1, 0));
		layout.setView(0, ssc::ptSAGITTAL, LayoutRegion(2, 0));
		layout.setView(1, ssc::ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ssc::ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ssc::ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ssc::ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ssc::ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ssc::ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefaultLayout(layout);
	}

	// ------------------------------------------------------
	// --- group of RTsource-based layouts - single viewgroup
	// ------------------------------------------------------
	this->addDefaultLayout(LayoutData::createHeader("LAYOUT_GROUP_RT", "Realtime Source"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_RT_1X1", "RT", 1, 1);
		layout.setView(0, ssc::ViewWidget::VIEW_REAL_TIME, LayoutRegion(0, 0));
		this->addDefaultLayout(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_US_Acquisition", "US Acquisition", 2, 3);
		layout.setView(0, ssc::ptANYPLANE, LayoutRegion(1, 2, 1, 1));
		layout.setView(0, ssc::ViewWidget::VIEW_3D, LayoutRegion(0, 2, 1, 1));
		layout.setView(0, ssc::ViewWidget::VIEW_REAL_TIME, LayoutRegion(0, 0, 2, 2));
		this->addDefaultLayout(layout);
	}

//  {
//    LayoutData layout;
//    layout.resetUid("LAYOUT_ACS_1X3");
//    layout.setName("ACS 1x3");
//    layout.resize(1,3);
//    layout.setView(0, ssc::ptAXIAL,    LayoutRegion(0, 0));
//    layout.setView(0, ssc::ptCORONAL,  LayoutRegion(0, 1));
//    layout.setView(0, ssc::ptSAGITTAL, LayoutRegion(0, 2));
//    this->addDefaultLayout(layout);
//  }
//  {
//    LayoutData layout;
//    layout.resetUid("LAYOUT_ACSACS_2X3");
//    layout.setName("ACSACS 2x3");
//    layout.resize(2,3);
//    layout.setView(0, ssc::ptAXIAL,    LayoutRegion(0, 0));
//    layout.setView(0, ssc::ptCORONAL,  LayoutRegion(0, 1));
//    layout.setView(0, ssc::ptSAGITTAL, LayoutRegion(0, 2));
//    layout.setView(1, ssc::ptAXIAL,    LayoutRegion(1, 0));
//    layout.setView(1, ssc::ptCORONAL,  LayoutRegion(1, 1));
//    layout.setView(1, ssc::ptSAGITTAL, LayoutRegion(1, 2));
//    this->addDefaultLayout(layout);
//  }
//  {
//    LayoutData layout;
//    layout.resetUid("LAYOUT_Any_2X3");
//    layout.setName("Any 2x3");
//    layout.resize(2,3);
//    layout.setView(0, ssc::ptANYPLANE,  LayoutRegion(0, 0));
//    layout.setView(0, ssc::ptSIDEPLANE, LayoutRegion(1, 0));
//    layout.setView(1, ssc::ptANYPLANE,  LayoutRegion(0, 1));
//    layout.setView(1, ssc::ptSIDEPLANE, LayoutRegion(1, 1));
//    layout.setView(2, ssc::ptANYPLANE,  LayoutRegion(0, 2));
//    layout.setView(2, ssc::ptSIDEPLANE, LayoutRegion(1, 2));
//    this->addDefaultLayout(layout);
//  }
//  {
//    LayoutData layout;
//    layout.resetUid("LAYOUT_3DAny_1X2");
//    layout.setName("3DAny 1x2");
//    layout.resize(1,2);
//    layout.setView(0, ssc::ViewWidget::VIEW_3D,   LayoutRegion(0, 0));
//    layout.setView(0, ssc::ptANYPLANE,  LayoutRegion(0, 1));
//    this->addDefaultLayout(layout);
//  }
}

void ViewManager::renderAllViewsSlot()
{
	mRenderTimer->beginRender();
    mLastBeginRender = QDateTime::currentDateTime();
    this->setRenderingInterval(settings()->value("renderingInterval").toInt());

    // updateViews() may be a bit expensive so we don't want to have it as a slot as previously
    // Only set the mModified flag, and update the views here
    if(mModified)
    {
        updateViews();
        mModified = false;
    }

//    mRenderTimer->time("update");

    // do a full render anyway at low rate. This is a convenience hack for rendering
    // occational effects that the smart render is too dumb to see.
    bool smart = mSmartRender;
    int smartInterval = mRenderingTimer->interval() * 40;
    if (mLastFullRender.time().msecsTo(QDateTime::currentDateTime().time()) > smartInterval)
        smart = false;

    for (ViewMap::iterator iter = mViewMap.begin(); iter != mViewMap.end(); ++iter)
    {
        if (iter->second->isVisible())
        {
            if (smart)
                dynamic_cast<ssc::View*>(iter->second)->render(); // render only changed scenegraph (shaky but smooth)
            else
            {
                iter->second->getRenderWindow()->Render(); // previous version: renders even when nothing is changed
            }

            report_gl_error_text(cstring_cast(QString("During rendering of view: ") + iter->first));
        }
    }

    if (!smart)
        mLastFullRender = QDateTime::currentDateTime();

	mRenderTimer->time("render");

	if (mRenderTimer->intervalPassed())
	{
		emit fps(mRenderTimer->getFPS());
//        static int counter=0;
//        if (++counter%3==0)
//            ssc::messageManager()->sendDebug(mRenderTimer->dumpStatisticsSmall());
        mRenderTimer->reset();
	}
//	std::cout << "==============================ViewManager::render" << std::endl;

    // tests show that the application wait between renderings even if the rendering uses more time
    // then the interval. This hack shortens the wait time between renderings but keeps below the
    // input update rate at all times.
    int usage = mLastBeginRender.msecsTo(QDateTime::currentDateTime()); // time spent in rendering
    int leftover = std::max(0, mRenderingTimer->interval() - usage); // time left of the rendering interval
    int timeToNext = std::max(1, leftover); // always wait at least 1ms - give others time to do stuff
//    std::cout << QString("setting interval %1, usage is %2").arg(timeToNext).arg(usage) << std::endl;
    this->setRenderingInterval(timeToNext);
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
	bool activeChange = mActiveLayout == data.getUid();
	unsigned pos = this->findLayoutData(data.getUid());
	if (pos == mLayouts.size())
		mLayouts.push_back(data);
	else
		mLayouts[pos] = data;

	if (activeChange)
	{
		mActiveLayout = "";
		this->setActiveLayout(data.getUid());
	}
	this->saveGlobalSettings();
	emit activeLayoutChanged();
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

QActionGroup* ViewManager::createLayoutActionGroup()
{
	QActionGroup* retval = new QActionGroup(this);
	retval->setExclusive(true);

	// add default layouts
	//std::vector<QString> layouts = this->getAvailableLayouts();
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (!this->isCustomLayout(mLayouts[i].getUid()))
			this->addLayoutAction(mLayouts[i].getUid(), retval);
	}

	// add separator
	QAction* sep = new QAction(retval);
	sep->setSeparator(this);
	//retval->addAction(sep);

	if (mDefaultLayouts.size() != mLayouts.size())
	{
		QAction* action = new QAction("Custom", retval);
		action->setEnabled(false);
	}

	// add custom layouts
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (this->isCustomLayout(mLayouts[i].getUid()))
			this->addLayoutAction(mLayouts[i].getUid(), retval);
	}

	// set checked status
	QString type = this->getActiveLayout();
	QList<QAction*> actions = retval->actions();
	for (int i = 0; i < actions.size(); ++i)
	{
		if (actions[i]->data().toString() == type)
			actions[i]->setChecked(true);
	}

	return retval;
}

/** Add one layout as an action to the layout menu.
 */
QAction* ViewManager::addLayoutAction(QString layout, QActionGroup* group)
{
	LayoutData data = this->getLayoutData(layout);
	if (data.isEmpty())
	{
		QAction* sep = new QAction(group);
		sep->setSeparator(this);
	}
	QAction* action = new QAction(data.getName(), group);
	action->setEnabled(!data.isEmpty());
	action->setCheckable(!data.isEmpty());
	action->setData(QVariant(layout));
	connect(action, SIGNAL(triggered()), this, SLOT(setLayoutActionSlot()));
	return action;
}

/** Called when a layout is selected: introspect the sending action
 *  in order to get correct layout; set it.
 */
void ViewManager::setLayoutActionSlot()
{
	QAction* action = dynamic_cast<QAction*>(sender());if (!action)
	return;
	this->setActiveLayout(action->data().toString());
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
	ssc::XmlOptionFile file = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("viewmanager");

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
	ssc::XmlOptionFile file = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("viewmanager");

	ssc::XmlOptionFile layoutsNode = file.descend("layouts");
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

//void ViewManager::fillModelTree(TreeItemPtr root)
//{
//  //TreeItemPtr item;
//  TreeItemPtr topItem = TreeItemImpl::create(root, "view groups", "", "");
//
//  for (unsigned i = 0; i < mViewGroups.size(); ++i)
//  {
//    ViewGroupPtr group = mViewGroups[i];
//    std::vector<ssc::ViewWidget*> views = group->getViews();
//    if (views.empty())
//      continue;
//    TreeItemPtr groupItem = TreeItemImpl::create(topItem, "group"+qstring_cast(i), "view group", qstring_cast(i));
//    for (unsigned j=0; j<views.size(); ++j)
//    {
//      TreeItemPtr viewItem = TreeItemImpl::create(groupItem, qstring_cast(views[j]->getName()), qstring_cast(views[j]->getTypeString()), "");
//      std::vector<ssc::RepPtr> reps = views[j]->getReps();
//      for (unsigned k=0; k<reps.size(); ++k)
//      {
//        QString name = reps[k]->getName();
//        if (name.isEmpty())
//          name = reps[k]->getType();
//        TreeItemImpl::create(viewItem, qstring_cast(name), qstring_cast(reps[k]->getType()), "");
//      }
//    }
//
//    std::vector<ssc::ImagePtr> images = group->getImages();
//    for (unsigned j=0; j<images.size(); ++j)
//    {
//      TreeItemPtr imageItem = TreeItemImage::create(groupItem, images[j]->getName());
//    }
//  }
//}

QActionGroup* ViewManager::createInteractorStyleActionGroup()
{
	QActionGroup* camGroup = new QActionGroup(this);
	camGroup->setExclusive(true);

	this->addInteractorStyleAction("Unicam", camGroup, "vtkInteractorStyleUnicam", QIcon(":/icons/camera-u.png"),
					"Set 3D interaction to a single-button style, useful for touch screens.");
	this->addInteractorStyleAction("Normal Camera", camGroup, "vtkInteractorStyleTrackballCamera",
					QIcon(":/icons/camera-n.png"), "Set 3D interaction to the normal camera-oriented style.");
	this->addInteractorStyleAction("Object", camGroup, "vtkInteractorStyleTrackballActor",
					QIcon(":/icons/camera-o.png"), "Set 3D interaction to a object-oriented style.");
	this->addInteractorStyleAction("Flight", camGroup, "vtkInteractorStyleFlight", QIcon(":/icons/camera-f.png"),
					"Set 3D interaction to a flight style.");

	return camGroup;
}

void ViewManager::addInteractorStyleAction(QString caption, QActionGroup* group, QString className, QIcon icon,
				QString helptext)
{
	ssc::ViewWidget* view = viewManager()->get3DView();
	if (!view)
		return;
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();

	QAction* action = new QAction(caption, group);
	action->setIcon(icon);
	action->setCheckable(true);
	action->setData(className);
	action->setToolTip(helptext);
	action->setWhatsThis(helptext);
	action->setChecked(QString(interactor->GetInteractorStyle()->GetClassName()) == className);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(setInteractionStyleActionSlot()));
}

void ViewManager::setInteractionStyleActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());if(!theAction)
	return;

	QString uid = theAction->data().toString();

	ssc::ViewWidget* view = viewManager()->get3DView();
	vtkRenderWindowInteractor* interactor = view->getRenderWindow()->GetInteractor();

	if (uid=="vtkInteractorStyleTrackballCamera")
		interactor->SetInteractorStyle(vtkInteractorStyleTrackballCameraPtr::New());
	else if (uid=="vtkInteractorStyleUnicam")
		interactor->SetInteractorStyle(vtkInteractorStyleUnicamPtr::New());
//  else if (uid=="vtkInteractorStyleTrackballActor")
//    interactor->SetInteractorStyle(vtkInteractorStyleTrackballActorPtr::New());
	else if (uid=="vtkInteractorStyleFlight")
		interactor->SetInteractorStyle(vtkInteractorStyleFlightPtr::New());

	ssc::messageManager()->sendInfo("Set Interactor: " + QString(interactor->GetInteractorStyle()->GetClassName()));
}

void ViewManager::autoShowData(ssc::DataPtr data)
{
	if (settings()->value("Automation/autoShowNewData").toBool())
	{
		this->getViewGroups()[0]->getData()->addDataSorted(data);
	}
}


} //namespace cx
