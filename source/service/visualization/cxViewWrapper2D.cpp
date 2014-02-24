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

/*
 * cxViewWrapper2D.cpp
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */


#include "cxViewWrapper2D.h"
#include <vector>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QMouseEvent>
#include <QWheelEvent>

#include "sscUtilHelpers.h"
#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscToolRep2D.h"
#include "sscOrientationAnnotationRep.h"
#include "sscOrientationAnnotation2DRep.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscManualTool.h"
#include "sscDataManager.h"
#include "cxViewManager.h"
#include "cxToolManager.h"
#include "cxViewGroup.h"
#include "sscDefinitionStrings.h"
#include "sscSlicePlanes3DRep.h"
#include "sscDefinitionStrings.h"
#include "sscSliceComputer.h"
#include "sscGeometricRep2D.h"
#include "sscTexture3DSlicerRep.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "sscGLHelpers.h"
#include "sscData.h"
#include "sscMesh.h"
#include "sscImage.h"
#include "sscPointMetricRep2D.h"
#include "sscLogger.h"
#include "cxViewFollower.h"
#include "cxVisualizationServiceBackend.h"

namespace cx
{

ViewWrapper2D::ViewWrapper2D(ViewWidget* view, VisualizationServiceBackendPtr backend) :
	ViewWrapper(backend),
	mOrientationActionGroup(new QActionGroup(view))
{
//  std::cout << "ViewWrapper2D create" << std::endl;
	mView = view;
	this->connectContextMenu(mView);

	// disable vtk interactor: this wrapper IS an interactor
	mView->getRenderWindow()->GetInteractor()->Disable();
	mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
	double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
	double length = clipDepth*10;
	mView->getRenderer()->GetActiveCamera()->SetPosition(0,0,length);
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(length-clipDepth, length+0.1);
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	addReps();

	setZoom2D(SyncedValue::create(1));
	setOrientationMode(SyncedValue::create(0)); // must set after addreps()

	connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	connect(mView, SIGNAL(resized(QSize)), this, SLOT(viewportChanged()));
	connect(mView, SIGNAL(showSignal(QShowEvent*)), this, SLOT(showSlot()));
	connect(mView, SIGNAL(mousePressSignal(QMouseEvent*)), this, SLOT(mousePressSlot(QMouseEvent*)));
	connect(mView, SIGNAL(mouseMoveSignal(QMouseEvent*)), this, SLOT(mouseMoveSlot(QMouseEvent*)));
	connect(mView, SIGNAL(mouseWheelSignal(QWheelEvent*)), this, SLOT(mouseWheelSlot(QWheelEvent*)));

	this->updateView();
}

ViewWrapper2D::~ViewWrapper2D()
{
//  std::cout << "ViewWrapper2D delete" << std::endl;
	if (mView)
		mView->removeReps();
}

void ViewWrapper2D::appendToContextMenu(QMenu& contextMenu)
{
	QAction* obliqueAction = new QAction("Oblique", &contextMenu);
	obliqueAction->setCheckable(true);
	obliqueAction->setData(qstring_cast(otOBLIQUE));
	obliqueAction->setChecked(getOrientationType() == otOBLIQUE);
	connect(obliqueAction, SIGNAL(triggered()), this, SLOT(orientationActionSlot()));

	QAction* ortogonalAction = new QAction("Ortogonal", &contextMenu);
	ortogonalAction->setCheckable(true);
	ortogonalAction->setData(qstring_cast(otORTHOGONAL));
	ortogonalAction->setChecked(getOrientationType() == otORTHOGONAL);
	//ortogonalAction->setChecked(true);
	connect(ortogonalAction, SIGNAL(triggered()), this, SLOT(orientationActionSlot()));

	//TODO remove actiongroups?
	mOrientationActionGroup->addAction(obliqueAction);
	mOrientationActionGroup->addAction(ortogonalAction);

	QAction* global2DZoomAction = new QAction("Global 2D Zoom", &contextMenu);
	global2DZoomAction->setCheckable(true);
	global2DZoomAction->setChecked(viewManager()->getGlobal2DZoom());
	connect(global2DZoomAction, SIGNAL(triggered()), this, SLOT(global2DZoomActionSlot()));

	contextMenu.addSeparator();
	contextMenu.addAction(obliqueAction);
	contextMenu.addAction(ortogonalAction);
	contextMenu.addSeparator();
	contextMenu.addAction(global2DZoomAction);
}

void ViewWrapper2D::setViewGroup(ViewGroupDataPtr group)
{
	ViewWrapper::setViewGroup(group);

	connect(group.get(), SIGNAL(optionsChanged()), this, SLOT(optionChangedSlot()));
	this->optionChangedSlot();

}

void ViewWrapper2D::optionChangedSlot()
{
	ViewGroupData::Options options = mGroupData->getOptions();

	if (mPickerGlyphRep)
	{
		mPickerGlyphRep->setMesh(options.mPickerGlyph);
	}
}

/** Slot for the orientation action.
 *  Set the orientation mode.
 */
void ViewWrapper2D::orientationActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());if(!theAction)
	return;

	ORIENTATION_TYPE type = string2enum<ORIENTATION_TYPE>(theAction->data().toString());
	mOrientationMode->set(type);
}

	/** Slot for the global zoom action
	 *  Set the global zoom flag in the view manager.
	 */
void ViewWrapper2D::global2DZoomActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());if(!theAction)
	return;

	viewManager()->setGlobal2DZoom(!viewManager()->getGlobal2DZoom());
}

void ViewWrapper2D::addReps()
{
	// annotation rep
	mOrientationAnnotationRep = OrientationAnnotationSmartRep::New("annotationRep_" + mView->getName(),
					"annotationRep_" + mView->getName());
	mView->addRep(mOrientationAnnotationRep);

	// plane type text rep
	mPlaneTypeText = DisplayTextRep::New("planeTypeRep_" + mView->getName(), "");
	mPlaneTypeText->addText(QColor(Qt::green), "not initialized", Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = DisplayTextRep::New("dataNameText_" + mView->getName(), "");
	mDataNameText->addText(QColor(Qt::green), "not initialized", Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);

	// slice proxy
	mSliceProxy = SliceProxy::create(mBackend->getDataManager());
	mViewFollower.reset(new ViewFollower);
	mViewFollower->setSliceProxy(mSliceProxy);

	// slice rep
	//the mul
	if (settings()->value("useGPU2DRendering").toBool())
	{
		//  this->resetMultiSlicer(); ignore until addimage
	}
	else
	{
//		mSliceRep = SliceRepSW::New("SliceRep_"+mView->getName());
//		mSliceRep->setSliceProxy(mSliceProxy);
//		mView->addRep(mSliceRep);
	}


//#ifdef USE_2D_GPU_RENDER
////  this->resetMultiSlicer(); ignore until addimage
//#else
//	mSliceRep = SliceRepSW::New("SliceRep_"+mView->getName());
//	mSliceRep->setSliceProxy(mSliceProxy);
//	mView->addRep(mSliceRep);
//#endif

	// tool rep
	mToolRep2D = ToolRep2D::New("Tool2D_" + mView->getName());
	mToolRep2D->setSliceProxy(mSliceProxy);
	mToolRep2D->setUseCrosshair(true);
//  mToolRep2D->setUseToolLine(false);
	mView->addRep(mToolRep2D);

	mPickerGlyphRep = GeometricRep2D::New("PickerGlyphRep_" + mView->getName());
	mPickerGlyphRep->setSliceProxy(mSliceProxy);
	if (mGroupData)
	{
		mPickerGlyphRep->setMesh(mGroupData->getOptions().mPickerGlyph);
	}
	mView->addRep(mPickerGlyphRep);
}

void ViewWrapper2D::settingsChangedSlot(QString key)
{
	if (key == "View/showDataText")
	{
		this->updateView();
	}
	if (key == "View/showOrientationAnnotation")
	{
		this->updateView();
	}
	if (key == "useGPU2DRendering")
	{
		this->updateView();
	}
	if (key == "Navigation/anyplaneViewOffset")
	{
		this->updateView();
	}
}

bool ViewWrapper2D::overlayIsEnabled()
{
	return true;
}

/**Hack: gpu slicer recreate and fill with images every time,
 * due to internal instabilities.
 *
 */
void ViewWrapper2D::resetMultiSlicer()
{
	if (mSliceRep)
	{
		mView->removeRep(mSliceRep);
		mSliceRep.reset();
	}
	if (mMultiSliceRep)
		mView->removeRep(mMultiSliceRep);
	if (!settings()->value("useGPU2DRendering").toBool())
		return;

//	std::cout << "using gpu multislicer" << std::endl;
	mMultiSliceRep = Texture3DSlicerRep::New("MultiSliceRep_" + mView->getName());
	mMultiSliceRep->setShaderPath(DataLocations::getShaderPath());
	mMultiSliceRep->setSliceProxy(mSliceProxy);
	mView->addRep(mMultiSliceRep);
	if (mGroupData)
		mMultiSliceRep->setImages(mGroupData->getImages());
	this->viewportChanged();
}

Vector3D ViewWrapper2D::viewToDisplay(Vector3D p_v) const
{
	vtkRendererPtr renderer = mView->getRenderer();
	renderer->SetViewPoint(p_v.begin());
	renderer->ViewToDisplay(); ///pang
	Vector3D p_d(renderer->GetDisplayPoint());
	return p_d;
}

Vector3D ViewWrapper2D::displayToWorld(Vector3D p_d) const
{
	vtkRendererPtr renderer = mView->getRenderer();
	renderer->SetDisplayPoint(p_d.begin());
	renderer->DisplayToWorld();
	double* p_wH = renderer->GetWorldPoint();
	Vector3D p_w = Vector3D(p_wH) / p_wH[3]; // convert from homogenous to cartesan coords
	return p_w;
}

/**Call when viewport size or zoom has changed.
 * Recompute camera zoom and  reps requiring vpMs.
 */
void ViewWrapper2D::viewportChanged()
{
	if (!mView->getRenderer()->IsActiveCameraCreated())
		return;

	mView->setZoomFactor(mZoom2D->get().toDouble());

	double viewHeight = mView->heightMM() / getZoomFactor2D();
//  double parallelScale = mView->heightMM() / 2.0 / getZoomFactor2D();
	mView->getRenderer()->GetActiveCamera()->SetParallelScale(viewHeight / 2);


	// Heuristic guess for a good clip depth. The point is to show 3D data clipped in 2D
	// with a suitable thickness projected onto the plane.
	double clipDepth = 2.0; // i.e. all 3D props rendered outside this range is not shown.
	double length = clipDepth*10;
	clipDepth = viewHeight/120 + 1.5;
	mView->getRenderer()->GetActiveCamera()->SetPosition(0,0,length);
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(length-clipDepth, length+0.1);
//	std::cout << "height: " << viewHeight << ", d=" << clipDepth << std::endl;

	mSliceProxy->setToolViewportHeight(viewHeight);
	double anyplaneViewOffset = settings()->value("Navigation/anyplaneViewOffset").toDouble();
	mSliceProxy->initializeFromPlane(mSliceProxy->getComputer().getPlaneType(), false, Vector3D(0, 0, 1), true, viewHeight, anyplaneViewOffset, true);
//	mSliceProxy->setToolViewOffset(mSliceProxy->getComputer().get, viewHeight, anyplaneViewOffset, true);

	DoubleBoundingBox3D BB_vp = getViewport();
	Transform3D vpMs = get_vpMs();
	DoubleBoundingBox3D BB_s = transform(vpMs.inv(), BB_vp);
	PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();

	mToolRep2D->setViewportData(vpMs, BB_vp);
	if (mSlicePlanes3DMarker)
	{
		mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, BB_s);
	}

	mViewFollower->setView(BB_s);
}

/**Return the viewport in vtk pixels. (viewport space)
 */
DoubleBoundingBox3D ViewWrapper2D::getViewport() const
{
	QSize size = mView->size();
	Vector3D p0_d(0, 0, 0);
	Vector3D p1_d(size.width(), size.height(), 0);
	DoubleBoundingBox3D BB_vp(p0_d, p1_d);
	return BB_vp;
}

/**Compute transform from slice space (vtk world/ssc after slicing) to vtk viewport.
 */
Transform3D ViewWrapper2D::get_vpMs() const
{
	// world == slice
	// display == vp

	QSize size = mView->size();

	Vector3D p0_d(0, 0, 0);
	Vector3D p1_d(size.width(), size.height(), 1);

	Vector3D p0_w = displayToWorld(p0_d);
	Vector3D p1_w = displayToWorld(p1_d);

	p0_w[2] = 0;
	p1_w[2] = 1;

	DoubleBoundingBox3D BB_vp(p0_d, p1_d);
	DoubleBoundingBox3D BB_s(p0_w, p1_w);

	Transform3D vpMs = createTransformNormalize(BB_s, BB_vp);
	return vpMs;
}

void ViewWrapper2D::showSlot()
{
	dominantToolChangedSlot();
	viewportChanged();
}

void ViewWrapper2D::initializePlane(PLANE_TYPE plane)
{
//  mOrientationAnnotationRep->setPlaneType(plane);
	mPlaneTypeText->setText(0, qstring_cast(plane));
	double viewHeight = mView->heightMM() / this->getZoomFactor2D();
	mSliceProxy->initializeFromPlane(plane, false, Vector3D(0, 0, 1), true, viewHeight, 0.25);
//	double anyplaneViewOffset = settings()->value("Navigation/anyplaneViewOffset").toDouble();
//	mSliceProxy->initializeFromPlane(plane, false, Vector3D(0, 0, 1), true, 1, 0);
	mOrientationAnnotationRep->setSliceProxy(mSliceProxy);

	// do this to force sync global and local type - must think on how we want this to work
	this->changeOrientationType(getOrientationType());

	bool isOblique = mSliceProxy->getComputer().getOrientationType() == otOBLIQUE;
	mToolRep2D->setUseCrosshair(!isOblique);
//  mToolRep2D->setUseToolLine(!isOblique);

}

/** get the orientation type directly from the slice proxy
 */
ORIENTATION_TYPE ViewWrapper2D::getOrientationType() const
{
	return mSliceProxy->getComputer().getOrientationType();
}

/** Slot called when the synced orientation has changed.
 *  Update the slice proxy orientation.
 */
void ViewWrapper2D::orientationModeChanged()
{
//  changeOrientationType(static_cast<ORIENTATION_TYPE>(mOrientationMode->get().toInt()));
//std::cout << "mOrientationModeChanbgedslot" << std::endl;

	ORIENTATION_TYPE type = static_cast<ORIENTATION_TYPE>(mOrientationMode->get().toInt());

if	(type == this->getOrientationType())
	return;
	if (!mSliceProxy)
	return;

	SliceComputer computer = mSliceProxy->getComputer();
	computer.switchOrientationMode(type);

	PLANE_TYPE plane = computer.getPlaneType();
//  mOrientationAnnotationRep->setPlaneType(plane);
					mPlaneTypeText->setText(0, qstring_cast(plane));
					mSliceProxy->setComputer(computer);
				}

	/** Set the synced orientation mode.
	 */
void ViewWrapper2D::changeOrientationType(ORIENTATION_TYPE type)
{
	mOrientationMode->set(type);
}

ViewWidget* ViewWrapper2D::getView()
{
	return mView;
}

/**
 */
void ViewWrapper2D::imageAdded(ImagePtr image)
{
	this->updateView();

	// removed this side effect: unwanted when loading a patient, might also be unwanted to change scene when adding/removing via gui?
	//Navigation().centerToView(mViewGroup->getImages());
}

void ViewWrapper2D::updateView()
{
	QString text;
	if (mGroupData)
	{
		std::vector<ImagePtr> images = mGroupData->getImages();
		ImagePtr image;
		if (!images.empty())
			image = images.back(); // always show last in vector

		if (image)
		{
			Vector3D c = image->get_rMd().coord(image->boundingBox().center());
			mSliceProxy->setDefaultCenter(c);
		}

		// slice rep
		if (settings()->value("useGPU2DRendering").toBool())
		{
			this->resetMultiSlicer();
			text = this->getAllDataNames().join("\n");
		}
		else
		{
			if (mMultiSliceRep)
			{
				mView->removeRep(mMultiSliceRep);
				mMultiSliceRep.reset();
			}

			if (!mSliceRep)
			{
				mSliceRep = SliceRepSW::New("SliceRep_" + mView->getName());
				mSliceRep->setSliceProxy(mSliceProxy);
				mView->addRep(mSliceRep);
			}

			QStringList textList;
			mSliceRep->setImage(image);

			// list all meshes and one image.
			std::vector<MeshPtr> mesh = mGroupData->getMeshes();
			for (unsigned i = 0; i < mesh.size(); ++i)
			textList << qstring_cast(mesh[i]->getName());
			if (image)
			textList << image->getName();
			text = textList.join("\n");
		}
	}

	bool show = settings()->value("View/showDataText").value<bool>();
	if (!show)
		text = QString();

	//update data name text rep
	mDataNameText->setText(0, text);
	mDataNameText->setFontSize(std::max(12, 22 - 2 * text.size()));

	mOrientationAnnotationRep->setVisible(settings()->value("View/showOrientationAnnotation").value<bool>());

//	mViewFollower->ensureCenterWithinView();
}



void ViewWrapper2D::imageRemoved(const QString& uid)
{
	updateView();
}

void ViewWrapper2D::dataAdded(DataPtr data)
{
	if (boost::dynamic_pointer_cast<Image>(data))
	{
		this->imageAdded(boost::dynamic_pointer_cast<Image>(data));
	}
	else if (boost::dynamic_pointer_cast<Mesh>(data))
	{
		this->meshAdded(boost::dynamic_pointer_cast<Mesh>(data));
	}
	else if (boost::dynamic_pointer_cast<PointMetric>(data))
	{
		this->pointMetricAdded(boost::dynamic_pointer_cast<PointMetric>(data));
	}
}

void ViewWrapper2D::dataRemoved(const QString& uid)
{
	this->imageRemoved(uid);
	this->meshRemoved(uid);
	this->pointMetricRemoved(uid);
}

void ViewWrapper2D::meshAdded(MeshPtr mesh)
{
	if (!mesh)
		return;
	if (mGeometricRep.count(mesh->getUid()))
		return;

	GeometricRep2DPtr rep = GeometricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
	rep->setMesh(mesh);
	mView->addRep(rep);
	mGeometricRep[mesh->getUid()] = rep;
	this->updateView();
}

void ViewWrapper2D::meshRemoved(const QString& uid)
{
	if (!mGeometricRep.count(uid))
		return;

	mView->removeRep(mGeometricRep[uid]);
	mGeometricRep.erase(uid);
	this->updateView();
}

void ViewWrapper2D::pointMetricAdded(PointMetricPtr mesh)
{
	if (!mesh)
		return;
	if (mPointMetricRep.count(mesh->getUid()))
		return;

	PointMetricRep2DPtr rep = PointMetricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
    rep->setDataMetric(mesh);
	rep->setFillVisibility(false);
	rep->setOutlineWidth(0.25);
	rep->setOutlineColor(1,0,0);
	rep->setDynamicSize(true);
	mView->addRep(rep);
	mPointMetricRep[mesh->getUid()] = rep;
	this->updateView();
}

void ViewWrapper2D::pointMetricRemoved(const QString& uid)
{
	if (!mPointMetricRep.count(uid))
		return;

	mView->removeRep(mPointMetricRep[uid]);
	mPointMetricRep.erase(uid);
	this->updateView();
}


void ViewWrapper2D::dominantToolChangedSlot()
{
	ToolPtr dominantTool = toolManager()->getDominantTool();
	mSliceProxy->setTool(dominantTool);
}

void ViewWrapper2D::setOrientationMode(SyncedValuePtr value)
{
	if (mOrientationMode)
		disconnect(mOrientationMode.get(), SIGNAL(changed()), this, SLOT(orientationModeChanged()));
	mOrientationMode = value;
	if (mOrientationMode)
		connect(mOrientationMode.get(), SIGNAL(changed()), this, SLOT(orientationModeChanged()));

	orientationModeChanged();
}

void ViewWrapper2D::setZoom2D(SyncedValuePtr value)
{
	if (mZoom2D)
		disconnect(mZoom2D.get(), SIGNAL(changed()), this, SLOT(viewportChanged()));
	mZoom2D = value;
	if (mZoom2D)
		connect(mZoom2D.get(), SIGNAL(changed()), this, SLOT(viewportChanged()));

	viewportChanged();
}

void ViewWrapper2D::setZoomFactor2D(double zoomFactor)
{
	zoomFactor = constrainValue(zoomFactor, 0.2, 10.0);
	mZoom2D->set(zoomFactor);
	viewportChanged();
}

double ViewWrapper2D::getZoomFactor2D() const
{
	return mZoom2D->get().toDouble();
}

/**Part of the mouse interactor:
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mousePressSlot(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (this->getOrientationType() == otORTHOGONAL)
		{
			setAxisPos(qvp2vp(event->pos()));
		}
		else
		{
			mClickPos = qvp2vp(event->pos());
			this->shiftAxisPos(Vector3D(0,0,0)); // signal the maual tool that something is happening (important for playback tool)
		}
	}
}

/**Part of the mouse interactor:
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mouseMoveSlot(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		if (this->getOrientationType() == otORTHOGONAL)
		{
			setAxisPos(qvp2vp(event->pos()));
		}
		else
		{
			Vector3D p = qvp2vp(event->pos());
			this->shiftAxisPos(p - mClickPos);
			mClickPos = p;
		}
	}
}


/**Part of the mouse interactor:
 * Interpret mouse wheel as a zoom operation.
 */
void ViewWrapper2D::mouseWheelSlot(QWheelEvent* event)
{
	// scale zoom in log space
	double val = log10(getZoomFactor2D());
	val += event->delta() / 120.0 / 20.0; // 120 is normal scroll resolution, x is zoom resolution
	double newZoom = pow(10.0, val);

	this->setZoomFactor2D(newZoom);

	Navigation().centerToTooltip(); // side effect: center on tool
}

/**Convert a position in Qt viewport space (pixels with origin in upper-left corner)
 * to vtk viewport space (pixels with origin in lower-left corner).
 */
Vector3D ViewWrapper2D::qvp2vp(QPoint pos_qvp)
{
	QSize size = mView->size();
	Vector3D pos_vp(pos_qvp.x(), size.height() - pos_qvp.y(), 0.0); // convert from left-handed qt space to vtk space display/viewport
	return pos_vp;
}

/**Move the tool pos / axis pos to a new position given
 * by delta movement in vp space.
 */
void ViewWrapper2D::shiftAxisPos(Vector3D delta_vp)
{
	delta_vp = -delta_vp;
	ManualToolPtr tool = cxToolManager::getInstance()->getManualTool();

	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D rMpr = dataManager()->get_rMpr();
	Transform3D prMt = tool->get_prMt();
	Vector3D delta_s = get_vpMs().inv().vector(delta_vp);

	Vector3D delta_pr = (rMpr.inv() * sMr.inv()).vector(delta_s);

	// MD is the actual tool movement in patient space, matrix form
	Transform3D MD = createTransformTranslate(delta_pr);
	// set new tool position to old modified by MD:
	tool->set_prMt(MD * prMt);
}

/**Move the tool pos / axis pos to a new position given
 * by the input click position in vp space.
 */
void ViewWrapper2D::setAxisPos(Vector3D click_vp)
{
	ManualToolPtr tool = cxToolManager::getInstance()->getManualTool();

	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D rMpr = dataManager()->get_rMpr();
	Transform3D prMt = tool->get_prMt();

	// find tool position in s
	Vector3D tool_t(0, 0, tool->getTooltipOffset());
	Vector3D tool_s = (sMr * rMpr * prMt).coord(tool_t);

	// find click position in s.
	Vector3D click_s = get_vpMs().inv().coord(click_vp);

	// compute the new tool position in slice space as a synthesis of the plane part of click and the z part of original.
	Vector3D cross_s(click_s[0], click_s[1], tool_s[2]);
	// compute the position change and transform to patient.
	Vector3D delta_s = cross_s - tool_s;
	Vector3D delta_pr = (rMpr.inv() * sMr.inv()).vector(delta_s);

	// MD is the actual tool movement in patient space, matrix form
	Transform3D MD = createTransformTranslate(delta_pr);
	// set new tool position to old modified by MD:
	tool->set_prMt(MD * prMt);
}

void ViewWrapper2D::setSlicePlanesProxy(SlicePlanesProxyPtr proxy)
{
	mSlicePlanes3DMarker = SlicePlanes3DMarkerIn2DRep::New("uid");
	PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();
	mSlicePlanes3DMarker->setProxy(plane, proxy);

	DoubleBoundingBox3D BB_vp = getViewport();
	Transform3D vpMs = get_vpMs();
	mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, transform(vpMs.inv(), BB_vp));

	mView->addRep(mSlicePlanes3DMarker);
}

//------------------------------------------------------------------------------
}
