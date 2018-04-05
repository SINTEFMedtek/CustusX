/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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

#include "cxUtilHelpers.h"
#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxToolRep2D.h"
#include "cxOrientationAnnotationRep.h"
#include "cxOrientationAnnotation2DRep.h"
#include "cxDisplayTextRep.h"

#include "cxManualTool.h"
#include "cxTrackingService.h"
#include "cxViewGroup.h"
#include "cxDefinitionStrings.h"
#include "cxSlicePlanes3DRep.h"
#include "cxDefinitionStrings.h"
#include "cxSliceComputer.h"
#include "cxGeometricRep2D.h"
#include "cxDataLocations.h"
#include "cxSettings.h"
#include "cxGLHelpers.h"
#include "cxData.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxTrackedStream.h"
#include "cxPointMetricRep2D.h"

#include "cxViewFollower.h"
#include "cxVisServices.h"
#include "cx2DZoomHandler.h"
#include "cxNavigation.h"
#include "cxDataRepContainer.h"
#include "vtkRenderWindowInteractor.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxViewService.h"
#include "cxRegionOfInterestMetric.h"

#include "cxTexture3DSlicerRep.h"

namespace cx
{

ViewWrapper2D::ViewWrapper2D(ViewPtr view, VisServicesPtr backend) :
	ViewWrapper(backend),
	mOrientationActionGroup(new QActionGroup(view.get()))
{
	qRegisterMetaType<Vector3D>("Vector3D");
	mView = view;
	this->connectContextMenu(mView);

	// disable vtk interactor: this wrapper IS an interactor
	mView->getRenderWindow()->GetInteractor()->Disable();
	mView->getRenderer()->GetActiveCamera()->SetParallelProjection(true);
	double clipDepth = 1.0; // 1mm depth, i.e. all 3D props rendered outside this range is not shown.
	double length = clipDepth*10;
	mView->getRenderer()->GetActiveCamera()->SetPosition(0,0,length);
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(length-clipDepth, length+0.1);

	// slice proxy
	mSliceProxy = SliceProxy::create(mServices->patient());

	mDataRepContainer.reset(new DataRepContainer());
	mDataRepContainer->setSliceProxy(mSliceProxy);
	mDataRepContainer->setView(mView);

	mViewFollower = ViewFollower::create(mServices->patient());
	mViewFollower->setSliceProxy(mSliceProxy);

	addReps();

	mZoom2D.reset(new Zoom2DHandler());
	connect(mZoom2D.get(), SIGNAL(zoomChanged()), this, SLOT(viewportChanged()));

	connect(mServices->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));
	connect(mView.get(), SIGNAL(resized(QSize)), this, SLOT(viewportChanged()));
	connect(mView.get(), SIGNAL(shown()), this, SLOT(showSlot()));
	connect(mView.get(), SIGNAL(mousePress(int, int, Qt::MouseButtons)), this, SLOT(mousePressSlot(int, int, Qt::MouseButtons)));
	connect(mView.get(), SIGNAL(mouseMove(int, int, Qt::MouseButtons)), this, SLOT(mouseMoveSlot(int, int, Qt::MouseButtons)));
	connect(mView.get(), SIGNAL(mouseWheel(int, int, int, int, Qt::MouseButtons)), this, SLOT(mouseWheelSlot(int, int, int, int, Qt::MouseButtons)));

	connect(mServices->patient().get(), &PatientModelService::videoAddedToTrackedStream, this, &ViewWrapper2D::videoSourcesChangedSlot);

	this->activeToolChangedSlot();
	this->updateView();
}

ViewWrapper2D::~ViewWrapper2D()
{
	if (mView)
		mView->removeReps();
}

void ViewWrapper2D::changeZoom(double delta)
{
	if (similar(delta, 1.0))
		return;

	double zoom = mZoom2D->getFactor();
//	CX_LOG_CHANNEL_DEBUG("CA") << "changing zoom from " << zoom << " by " << delta;
	zoom *= delta;
//	CX_LOG_CHANNEL_DEBUG("CA") << "            new zoom:" << zoom;
	mZoom2D->setFactor(zoom);
//	CX_LOG_CHANNEL_DEBUG("CA") << "            got zoom:" << mZoom2D->getFactor();
}

void ViewWrapper2D::samplePoint(Vector3D click_vp)
{
	if(!this->isAnyplane())
		return;

	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D vpMs = mView->get_vpMs();

	Vector3D p_s = vpMs.inv().coord(click_vp);
	Vector3D p_r = sMr.inv().coord(p_s);

	emit pointSampled(p_r);
}

void ViewWrapper2D::appendToContextMenu(QMenu& contextMenu)
{
    contextMenu.addSeparator();
	mZoom2D->addActionsToMenu(&contextMenu);

	contextMenu.addSeparator();
	QAction* showManualTool = new QAction("Show Manual Tool 2D", &contextMenu);
	showManualTool->setCheckable(true);
	showManualTool->setChecked(settings()->value("View2D/showManualTool").toBool());
	connect(showManualTool, SIGNAL(triggered(bool)), this, SLOT(showManualToolSlot(bool)));
	contextMenu.addAction(showManualTool);
}

void ViewWrapper2D::setViewGroup(ViewGroupDataPtr group)
{
	ViewWrapper::setViewGroup(group);

	mZoom2D->setGroupData(group);
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

void ViewWrapper2D::addReps()
{
	// annotation rep
	mOrientationAnnotationRep = OrientationAnnotationSmartRep::New();
	mView->addRep(mOrientationAnnotationRep);

	this->ViewWrapper::addReps();

	// tool rep
	mToolRep2D = ToolRep2D::New(mServices->spaceProvider(), "Tool2D_" + mView->getName());
	mToolRep2D->setSliceProxy(mSliceProxy);
	mToolRep2D->setUseCrosshair(true);
	this->toggleShowManualTool();

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
	this->ViewWrapper::settingsChangedSlot(key);

	if (key == "View2D/useGPU2DRendering")
	{
		this->updateView();
	}
	if (key == "View2D/useLinearInterpolationIn2DRendering")
	{
		this->updateView();
	}
	if (key == "Navigation/anyplaneViewOffset")
	{
		this->updateView();
	}
	if (key == "View2D/showManualTool")
	{
		this->toggleShowManualTool();
	}
}

void ViewWrapper2D::toggleShowManualTool()
{
	if (settings()->value("View2D/showManualTool").toBool())
		mView->addRep(mToolRep2D);
	else
		mView->removeRep(mToolRep2D);
}

void ViewWrapper2D::removeAndResetSliceRep()
{
    if (mSliceRep)
    {
        mView->removeRep(mSliceRep);
        mSliceRep.reset();
    }
}

void ViewWrapper2D::removeAndResetMultiSliceRep()
{
	if (mMultiSliceRep)
	{
		mView->removeRep(mMultiSliceRep);
		mMultiSliceRep.reset();
	}
}

bool ViewWrapper2D::createAndAddMultiSliceRep()
{
	if(!mSharedOpenGLContext)
	{
		CX_LOG_WARNING() << "ViewWrapper2D::createAndAddMultiSliceRep(): Got no mSharedOpenGLContext";
		return false;
	}
	if (mMultiSliceRep)
		return true;

	mMultiSliceRep = Texture3DSlicerRep::New(mSharedOpenGLContext);
	mMultiSliceRep->setShaderPath(DataLocations::findConfigFolder("/shaders"));
	mMultiSliceRep->setSliceProxy(mSliceProxy);
	mMultiSliceRep->setRenderWindow(mView->getRenderWindow());

	mView->addRep(mMultiSliceRep);

	return true;
}

/**Hack: gpu slicer recreate and fill with images every time,
 * due to internal instabilities.
 * Fix: now reuses slicer, seem to have fixed issue.
 *
 */
void ViewWrapper2D::recreateMultiSlicer()
{
	this->removeAndResetSliceRep();

	if (!this->useGPU2DRendering())
	{
		this->removeAndResetMultiSliceRep();
		return;
	}

	if(!this->createAndAddMultiSliceRep())
	{
		return;
	}

	if (mGroupData)
		mMultiSliceRep->setImages(this->getImagesToView());
	else
		mMultiSliceRep->setImages(std::vector<ImagePtr>());

	this->viewportChanged();
}

std::vector<ImagePtr> ViewWrapper2D::getImagesToView()
{
	bool include2D = this->isAnyplane();
	std::vector<ImagePtr> images = mGroupData->getImagesAndChangingImagesFromTrackedStreams(DataViewProperties::createSlice2D(), include2D);
	return images;
}

bool ViewWrapper2D::isAnyplane()
{
	PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();
	return plane == ptANYPLANE;
}

/**Call when viewport size or zoom has changed.
 * Recompute camera zoom and  reps requiring vpMs.
 */
void ViewWrapper2D::viewportChanged()
{
	if (!mView->getRenderer()->IsActiveCameraCreated())
		return;	

	mView->setZoomFactor(mZoom2D->getFactor());

	double viewHeight = mView->getViewport_s().range()[1];
	mView->getRenderer()->GetActiveCamera()->SetParallelScale(viewHeight / 2);

	// Heuristic guess for a good clip depth. The point is to show 3D data clipped in 2D
	// with a suitable thickness projected onto the plane.
	double clipDepth = 2.0; // i.e. all 3D props rendered outside this range is not shown.
	double length = clipDepth*10;
	clipDepth = viewHeight/120 + 1.5;
	mView->getRenderer()->GetActiveCamera()->SetPosition(0,0,length);
	mView->getRenderer()->GetActiveCamera()->SetClippingRange(length-clipDepth, length+0.1);

	mSliceProxy->setToolViewportHeight(viewHeight);
	double anyplaneViewOffset = settings()->value("Navigation/anyplaneViewOffset").toDouble();
	mSliceProxy->initializeFromPlane(mSliceProxy->getComputer().getPlaneType(), false, true, viewHeight, anyplaneViewOffset);

	DoubleBoundingBox3D BB_vp = getViewport();
	Transform3D vpMs = mView->get_vpMs();
	DoubleBoundingBox3D BB_s = transform(vpMs.inv(), BB_vp);
	PLANE_TYPE plane = mSliceProxy->getComputer().getPlaneType();

	mToolRep2D->setViewportData(vpMs, BB_vp);
	if (mSlicePlanes3DMarker)
	{
		mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, BB_s);
	}
}

void ViewWrapper2D::applyViewFollower()
{
	if (!mGroupData)
		return;
	QString roiUid = mGroupData->getOptions().mCameraStyle.mAutoZoomROI;
	mViewFollower->setAutoZoomROI(roiUid);
	mViewFollower->setView(this->getViewport_s());
	SliceAutoViewportCalculator::ReturnType result = mViewFollower->calculate();

//	CX_LOG_CHANNEL_DEBUG("CA") << "roi=" << roiUid;
//	CX_LOG_CHANNEL_DEBUG("CA") << this << " autozoom zoom=" << result.zoom << ", center=" << result.center_shift_s;
	this->changeZoom(result.zoom);
	Vector3D newcenter_r = mViewFollower->findCenter_r_fromShift_s(result.center_shift_s);
	mServices->patient()->setCenter(newcenter_r);
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

void ViewWrapper2D::showSlot()
{
	activeToolChangedSlot();
	viewportChanged();
}

void ViewWrapper2D::initializePlane(PLANE_TYPE plane)
{
	double viewHeight = mView->getViewport_s().range()[1];
	mSliceProxy->initializeFromPlane(plane, false, true, viewHeight, 0.25);
	mOrientationAnnotationRep->setSliceProxy(mSliceProxy);
}

/** get the orientation type directly from the slice proxy
 */
ORIENTATION_TYPE ViewWrapper2D::getOrientationType() const
{
	return mSliceProxy->getComputer().getOrientationType();
}

ViewPtr ViewWrapper2D::getView()
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

ImagePtr ViewWrapper2D::getImageToDisplay()
{
    if (!mGroupData)
        return ImagePtr();

	std::vector<ImagePtr> images = mGroupData->getImagesAndChangingImagesFromTrackedStreams(DataViewProperties::createSlice2D(), true);
    ImagePtr image;
    if (!images.empty())
        image = images.back();  // always show last in vector

    return image;
}

bool ViewWrapper2D::useGPU2DRendering()
{
		return settings()->value("View2D/useGPU2DRendering").toBool();
}

void ViewWrapper2D::createAndAddSliceRep()
{
    if (!mSliceRep)
    {
        mSliceRep = SliceRepSW::New("SliceRep_" + mView->getName());
        mSliceRep->setSliceProxy(mSliceProxy);
        mView->addRep(mSliceRep);
    }
}

QString ViewWrapper2D::getDataDescription()
{
	QString text;
	if (this->useGPU2DRendering())
	{
		text = this->getAllDataNames(DataViewProperties::createSlice2D()).join("\n");
	}
	else //software rendering
	{
		ImagePtr image = this->getImageToDisplay();
		if (!image)
			return "";
		// list all meshes and one image.
		QStringList textList;
		std::vector<MeshPtr> mesh = mGroupData->getMeshes(DataViewProperties::createSlice2D());
		for (unsigned i = 0; i < mesh.size(); ++i)
			textList << qstring_cast(mesh[i]->getName());
		if (image)
			textList << image->getName();
		text = textList.join("\n");
	}
	return text;
}

QString ViewWrapper2D::getViewDescription()
{
	return qstring_cast(mSliceProxy->getComputer().getPlaneType());
}

void ViewWrapper2D::updateItemsFromViewGroup()
{
	if (!mGroupData)
		return;

    ImagePtr image = this->getImageToDisplay();

    if (image)
    {
        Vector3D c = image->get_rMd().coord(image->boundingBox().center());
        mSliceProxy->setDefaultCenter(c);

        if (this->useGPU2DRendering())
        {
            this->recreateMultiSlicer();
        }
        else //software rendering
        {
            this->removeAndResetMultiSliceRep();
            this->createAndAddSliceRep();

            mSliceRep->setImage(image);
        }
    }
    else //no images to display in the view
    {
        this->removeAndResetSliceRep();
        this->removeAndResetMultiSliceRep();
    }
}

void ViewWrapper2D::updateView()
{
	if (!this->getView())
		return;
	this->updateItemsFromViewGroup();

	this->ViewWrapper::updateView();

    //UPDATE ORIENTATION ANNOTATION
	mOrientationAnnotationRep->setVisible(settings()->value("View/showOrientationAnnotation").value<bool>());

    //UPDATE DATA METRIC ANNOTATION
	mDataRepContainer->updateSettings();

	if (mToolRep2D)
	{
		bool isOblique = mSliceProxy->getComputer().getOrientationType() == otOBLIQUE;
		bool useCrosshair = settings()->value("View2D/showToolCrosshair", true).toBool();
		mToolRep2D->setUseCrosshair(!isOblique && useCrosshair);
		mToolRep2D->setCrosshairColor(settings()->value("View2D/toolCrossHairColor").value<QColor>());
		mToolRep2D->setTooltipLineColor(settings()->value("View2D/toolColor").value<QColor>());
		mToolRep2D->setTooltipPointColor(settings()->value("View/toolTipPointColor").value<QColor>());
		mToolRep2D->setToolOffsetPointColor(settings()->value("View/toolOffsetPointColor").value<QColor>());
		mToolRep2D->setToolOffsetLineColor(settings()->value("View/toolOffsetLineColor").value<QColor>());
	}

	this->applyViewFollower();
}

DoubleBoundingBox3D ViewWrapper2D::getViewport_s() const
{
	DoubleBoundingBox3D BB_vp = getViewport();
	Transform3D vpMs = mView->get_vpMs();
	DoubleBoundingBox3D BB_s = transform(vpMs.inv(), BB_vp);
	return BB_s;
}

void ViewWrapper2D::dataViewPropertiesChangedSlot(QString uid)
{
	DataPtr data = mServices->patient()->getData(uid);
	DataViewProperties properties = mGroupData->getProperties(uid);

	if (properties.hasSlice2D())
		this->dataAdded(data);
	else
		this->dataRemoved(uid);
}

void ViewWrapper2D::dataAdded(DataPtr data)
{
	if (boost::dynamic_pointer_cast<Image>(data))
	{
		this->imageAdded(boost::dynamic_pointer_cast<Image>(data));
	}
	else
	{
		mDataRepContainer->addData(data);
	}
	this->updateView();
}

void ViewWrapper2D::dataRemoved(const QString& uid)
{
	mDataRepContainer->removeData(uid);
	this->updateView();
}

void ViewWrapper2D::activeToolChangedSlot()
{
	ToolPtr activeTool = mServices->tracking()->getActiveTool();
	mSliceProxy->setTool(activeTool);
}

/**Part of the mouse interactor:
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mousePressSlot(int x, int y, Qt::MouseButtons buttons)
{
	if (buttons & Qt::LeftButton)
	{
		Vector3D clickPos_vp = qvp2vp(QPoint(x,y));
		moveManualTool(clickPos_vp, Vector3D(0,0,0));
		samplePoint(clickPos_vp);
	}
}

/**Part of the mouse interactor:
 * Move manual tool tip when mouse pressed
 *
 */
void ViewWrapper2D::mouseMoveSlot(int x, int y, Qt::MouseButtons buttons)
{
	if (buttons & Qt::LeftButton)
	{
		Vector3D clickPos_vp = qvp2vp(QPoint(x,y));
		moveManualTool(clickPos_vp, clickPos_vp - mLastClickPos_vp);
	}
}

void ViewWrapper2D::moveManualTool(Vector3D vp, Vector3D delta_vp)
{
	if (this->getOrientationType() == otORTHOGONAL)
		setAxisPos(vp);
	else
	{
		this->shiftAxisPos(delta_vp); // signal the maual tool that something is happening (important for playback tool)
		mLastClickPos_vp = vp;
	}
}

/**Part of the mouse interactor:
 * Interpret mouse wheel as a zoom operation.
 */
void ViewWrapper2D::mouseWheelSlot(int x, int y, int delta, int orientation, Qt::MouseButtons buttons)
{
	// scale zoom in log space
	double val = log10(mZoom2D->getFactor());
	val += delta / 120.0 / 20.0; // 120 is normal scroll resolution, x is zoom resolution
	double newZoom = pow(10.0, val);

	mZoom2D->setFactor(newZoom);

	Navigation(mServices).centerToTooltip(); // side effect: center on tool
}

/**Convert a position in Qt viewport space (pixels with origin in upper-left corner)
 * to vtk viewport space (pixels with origin in lower-left corner).
 */
Vector3D ViewWrapper2D::qvp2vp(QPoint pos_qvp)
{
	QSize size = mView->size();
	Vector3D pos_vp(pos_qvp.x(), size.height()-1 - pos_qvp.y(), 0.0); // convert from left-handed qt space to vtk space display/viewport
	return pos_vp;
}

/**Move the tool pos / axis pos to a new position given
 * by delta movement in vp space.
 */
void ViewWrapper2D::shiftAxisPos(Vector3D delta_vp)
{
	delta_vp = -delta_vp;
	ToolPtr tool = mServices->tracking()->getManualTool();

	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D prMt = tool->get_prMt();
	Transform3D vpMs = mView->get_vpMs();
	Vector3D delta_s = vpMs.inv().vector(delta_vp);

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
	ToolPtr tool = mServices->tracking()->getManualTool();

	Transform3D sMr = mSliceProxy->get_sMr();
	Transform3D rMpr = mServices->patient()->get_rMpr();
	Transform3D prMt = tool->get_prMt();

	// find tool position in s
	Vector3D tool_t(0, 0, tool->getTooltipOffset());
	Vector3D tool_s = (sMr * rMpr * prMt).coord(tool_t);

	// find click position in s.
	Transform3D vpMs = mView->get_vpMs();
	Vector3D click_s = vpMs.inv().coord(click_vp);

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

//	DoubleBoundingBox3D BB_vp = getViewport();
//	Transform3D vpMs = mView->get_vpMs();
//	mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, transform(vpMs.inv(), BB_vp));
	mSlicePlanes3DMarker->getProxy()->setViewportData(plane, mSliceProxy, this->getViewport_s());

	mView->addRep(mSlicePlanes3DMarker);
}

void ViewWrapper2D::videoSourcesChangedSlot()
{
	this->updateView();
}

void ViewWrapper2D::showManualToolSlot(bool visible)
{
	settings()->setValue("View2D/showManualTool", visible);
}

//------------------------------------------------------------------------------
}
