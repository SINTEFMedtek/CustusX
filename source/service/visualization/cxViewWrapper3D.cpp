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

#include "cxViewWrapper3D.h"

#include <vector>

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include <QAction>
#include <QMenu>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include "sscView.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscToolRep2D.h"
#include "sscDisplayTextRep.h"
#include "sscMessageManager.h"
#include "sscSlicePlanes3DRep.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscPickerRep.h"
#include "sscGeometricRep.h"
#include "sscToolRep3D.h"
#include "sscVolumetricRep.h"
#include "sscTypeConversions.h"
#include "sscVideoSource.h"
#include "sscVideoRep.h"
#include "sscToolTracer.h"
#include "sscOrientationAnnotation3DRep.h"
#include "cxSettings.h"
#include "cxToolManager.h"
#include "cxRepManager.h"
#include "cxCameraControl.h"
#include "cxLandmarkRep.h"
#include "sscPointMetricRep.h"
#include "sscDistanceMetricRep.h"
#include "sscAngleMetricRep.h"
#include "sscPlaneMetricRep.h"
#include "cxFrameMetricRep.h"
#include "cxToolMetricRep.h"
#include "sscDataMetricRep.h"
#include "cxDataLocations.h"
#include "sscTexture3DSlicerRep.h"
#include "sscSlices3DRep.h"
#include "sscEnumConverter.h"
#include "sscManualTool.h"
#include "sscImage2DRep3D.h"
#include "sscLogger.h"

#include "sscData.h"
#include "sscAxesRep.h"
#include "cxViewGroup.h"

#include "sscAngleMetric.h"
#include "sscDistanceMetric.h"
#include "sscPointMetric.h"

#include "cxDepthPeeling.h"
#include "cxAxisConnector.h"
#include "cxMultiVolume3DRepProducer.h"


namespace cx
{



ViewWrapper3D::ViewWrapper3D(int startIndex, ViewWidget* view)
{
	view->getRenderer()->GetActiveCamera()->SetClippingRange(1, 2000);
	if (!view->getRenderWindow()->GetStereoCapableWindow())
		view->getRenderWindow()->StereoCapableWindowOn(); // Just set all 3D views 3D capable

	mShowAxes = false;
	mView = view;
	this->connectContextMenu(mView);
	QString index = QString::number(startIndex);
	QColor background = settings()->value("backgroundColor").value<QColor>();
	mView->setBackgroundColor(background);

	view->getRenderer()->GetActiveCamera()->SetParallelProjection(false);
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	this->initializeMultiVolume3DRepProducer();

	mLandmarkRep = LandmarkRep::New("LandmarkRep_" + index);
	mLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
	mLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());

	mPickerRep = PickerRep::New("PickerRep_" + index, "PickerRep_" + index);

	connect(mPickerRep.get(), SIGNAL(pointPicked(Vector3D)), this, SLOT(PickerRepPointPickedSlot(Vector3D)));
	connect(mPickerRep.get(), SIGNAL(dataPicked(QString)), this, SLOT(PickerRepDataPickedSlot(QString)));
	mPickerRep->setSphereRadius(settings()->value("View3D/sphereRadius").toDouble());
	mPickerRep->setEnabled(false);
	mView->addRep(mPickerRep);
	connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	this->dominantToolChangedSlot();

	// plane type text rep
	mPlaneTypeText = DisplayTextRep::New("planeTypeRep_" + mView->getName(), "");
	mPlaneTypeText->addText(Vector3D(0, 1, 0), "3D", Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = DisplayTextRep::New("dataNameText_" + mView->getName(), "");
	mDataNameText->addText(Vector3D(0, 1, 0), "not initialized", Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);

	connect(toolManager(), SIGNAL(configured()), this, SLOT(toolsAvailableSlot()));
	connect(toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
	connect(dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChangedSlot()));
	this->toolsAvailableSlot();

	mAnnotationMarker = RepManager::getInstance()->getCachedRep<OrientationAnnotation3DRep>(
					"annotation_" + mView->getName());
//  mAnnotationMarker = OrientationAnnotation3DRep::New("annotation_"+mView->getName(), "");
	mAnnotationMarker->setMarkerFilename(
					DataLocations::getRootConfigPath() + "/models/"
									+ settings()->value("View3D/annotationModel").toString());
	mAnnotationMarker->setSize(settings()->value("View3D/annotationModelSize").toDouble());

	mView->addRep(mAnnotationMarker);
//  mAnnotationMarker->setVisible(settings()->value("View3D/showOrientationAnnotation").toBool());

//Stereo
//  mView->getRenderWindow()->StereoCapableWindowOn(); // Moved to cxView3D
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));
	//Init 3D stereo from settings
	this->setStereoType(settings()->value("View3D/stereoType").toInt());
	this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());

	//Only try to set depth peeling if View3D/depthPeeling == true
	if(settings()->value("View3D/depthPeeling").toBool())
		this->setTranslucentRenderingToDepthPeeling(settings()->value("View3D/depthPeeling").toBool());

	this->updateView();
}

ViewWrapper3D::~ViewWrapper3D()
{
	if (mView)
	{
		mView->removeReps();
		mMultiVolume3DRepProducer->removeRepsFromView();
	}
}

void ViewWrapper3D::initializeMultiVolume3DRepProducer()
{
	if (!mView)
		messageManager()->sendError("Missing View in initializeMultiVolume3DRepProducer");

	if (!mMultiVolume3DRepProducer)
	{
		mMultiVolume3DRepProducer.reset(new MultiVolume3DRepProducer());
		connect(mMultiVolume3DRepProducer.get(), SIGNAL(imagesChanged()), this, SLOT(updateView()));
		mMultiVolume3DRepProducer->setView(mView);
	}

	mMultiVolume3DRepProducer->setMaxRenderSize(settings()->value("View3D/maxRenderSize").toInt());
	mMultiVolume3DRepProducer->setVisualizerType(settings()->value("View3D/ImageRender3DVisualizer").toString());
}

void ViewWrapper3D::settingsChangedSlot(QString key)
{
	if (key == "backgroundColor")
	{
		QColor background = settings()->value("backgroundColor").value<QColor>();
		mView->setBackgroundColor(background);
	}
	if (( key=="View3D/ImageRender3DVisualizer" )||( key=="View3D/maxRenderSize" ))
	{
		this->initializeMultiVolume3DRepProducer();
	}
	if (key == "View/showDataText")
	{
		this->updateView();
	}
	if ((key == "View3D/annotationModelSize" )||( key == "View3D/annotationModel"))
	{
		mAnnotationMarker->setMarkerFilename(
						DataLocations::getRootConfigPath() + "/models/"
										+ settings()->value("View3D/annotationModel").toString());
		mAnnotationMarker->setSize(settings()->value("View3D/annotationModelSize").toDouble());
	}
	if (key == "showManualTool")
	{
		this->toolsAvailableSlot();
	}
	if ((key == "View3D/sphereRadius" )||( key == "View3D/labelSize" )||( key == "View/showLabels"))
	{
		for (RepMap::iterator iter = mDataReps.begin(); iter != mDataReps.end(); ++iter)
		{
			this->readDataRepSettings(iter->second);
		}

		this->toolsAvailableSlot();
		mLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
		mLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());
	}
	if (key == "View3D/depthPeeling")
		this->setTranslucentRenderingToDepthPeeling(settings()->value("View3D/depthPeeling").toBool());
}

void ViewWrapper3D::PickerRepPointPickedSlot(Vector3D p_r)
{
	Transform3D rMpr = *toolManager()->get_rMpr();
	Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ManualToolPtr tool = cxToolManager::getInstance()->getManualTool();
	Vector3D offset = tool->get_prMt().vector(Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	dataManager()->setCenter(p_r);
	Vector3D p0_pr = tool->get_prMt().coord(Vector3D(0, 0, 0));
	tool->set_prMt(createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
}

void ViewWrapper3D::PickerRepDataPickedSlot(QString uid)
{
	//std::cout << "picked: " << uid << std::endl;
}

void ViewWrapper3D::appendToContextMenu(QMenu& contextMenu)
{
	QAction* slicePlanesAction = new QAction("Show Slice Planes", &contextMenu);
	slicePlanesAction->setCheckable(true);
	slicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
	connect(slicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(showSlicePlanesActionSlot(bool)));

	QAction* fillSlicePlanesAction = new QAction("Fill Slice Planes", &contextMenu);
	fillSlicePlanesAction->setCheckable(true);
	fillSlicePlanesAction->setEnabled(mSlicePlanes3DRep->getProxy()->getVisible());
	fillSlicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getDrawPlanes());
	connect(fillSlicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(fillSlicePlanesActionSlot(bool)));

	QAction* resetCameraAction = new QAction("Reset Camera (r)", &contextMenu);
	connect(resetCameraAction, SIGNAL(triggered()), this, SLOT(resetCameraActionSlot()));

	QAction* centerImageAction = new QAction("Center to image", &contextMenu);
	connect(centerImageAction, SIGNAL(triggered()), this, SLOT(centerImageActionSlot()));

	QAction* centerToolAction = new QAction("Center to tool", &contextMenu);
	connect(centerToolAction, SIGNAL(triggered()), this, SLOT(centerToolActionSlot()));

	QAction* showAxesAction = new QAction("Show Coordinate Axes", &contextMenu);
	showAxesAction->setCheckable(true);
	showAxesAction->setChecked(mShowAxes);
	connect(showAxesAction, SIGNAL(triggered(bool)), this, SLOT(showAxesActionSlot(bool)));

	QAction* showManualTool = new QAction("Show Manual Tool", &contextMenu);
	showManualTool->setCheckable(true);
	showManualTool->setChecked(settings()->value("showManualTool").toBool());
	connect(showManualTool, SIGNAL(triggered(bool)), this, SLOT(showManualToolSlot(bool)));

	QAction* showOrientation = new QAction("Show Orientation", &contextMenu);
	showOrientation->setCheckable(true);
	showOrientation->setChecked(mAnnotationMarker->getVisible());
	connect(showOrientation, SIGNAL(triggered(bool)), this, SLOT(showOrientationSlot(bool)));

	QAction* showToolPath = new QAction("Show Tool Path", &contextMenu);
	showToolPath->setCheckable(true);
	showToolPath->setChecked(settings()->value("showToolPath").toBool());
	connect(showToolPath, SIGNAL(triggered(bool)), this, SLOT(showToolPathSlot(bool)));

#ifdef USE_GLX_SHARED_CONTEXT
	QMenu* showSlicesMenu = new QMenu("Show Slices", &contextMenu);
	showSlicesMenu->addAction(this->createSlicesAction("ACS", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Axial", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Sagittal", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Coronal", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Any", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Dual", &contextMenu));
	showSlicesMenu->addAction(this->createSlicesAction("Radial", &contextMenu));
#endif // USE_GLX_SHARED_CONTEXT
	QAction* showRefTool = new QAction("Show Reference Tool", &contextMenu);
	showRefTool->setDisabled(true);
	showRefTool->setCheckable(true);
	ToolPtr refTool = cxToolManager::getInstance()->getReferenceTool();
	if (refTool)
	{
		showRefTool->setText("Show " + refTool->getName());
		showRefTool->setEnabled(true);
		showRefTool->setChecked(RepManager::findFirstRep<ToolRep3D>(mView->getReps(), refTool));
		connect(showRefTool, SIGNAL(toggled(bool)), this, SLOT(showRefToolSlot(bool)));
	}

	contextMenu.addSeparator();
#ifdef USE_GLX_SHARED_CONTEXT
	contextMenu.addMenu(showSlicesMenu);
#endif //USE_GLX_SHARED_CONTEXT
	contextMenu.addAction(resetCameraAction);
	contextMenu.addAction(centerImageAction);
	contextMenu.addAction(centerToolAction);
	contextMenu.addAction(showAxesAction);
	contextMenu.addAction(showOrientation);
	contextMenu.addSeparator();
	contextMenu.addAction(showManualTool);
	contextMenu.addAction(showRefTool);
	if (showToolPath)
		contextMenu.addAction(showToolPath);
	contextMenu.addSeparator();
	contextMenu.addAction(slicePlanesAction);
	contextMenu.addAction(fillSlicePlanesAction);
}

QAction* ViewWrapper3D::createSlicesAction(QString title, QWidget* parent)
{
	QAction* action = new QAction(title, parent);
	connect(action, SIGNAL(triggered()), this, SLOT(showSlices()));
	action->setData(title);
	action->setCheckable(true);
	action->setChecked(mShowSlicesMode == title);
	return action;
}

void ViewWrapper3D::showSlices()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if (!action)
		return;

	if (!action->isChecked())
		mShowSlicesMode = "";
	else
		mShowSlicesMode = action->data().toString();
	//	std::cout << "show " << mShowSlicesMode << std::endl;
	this->updateSlices();
}

void ViewWrapper3D::setViewGroup(ViewGroupDataPtr group)
{
	ViewWrapper::setViewGroup(group);

	connect(group.get(), SIGNAL(initialized()), this, SLOT(resetCameraActionSlot()));
	connect(group.get(), SIGNAL(optionsChanged()), this, SLOT(optionChangedSlot()));
	mView->getRenderer()->SetActiveCamera(mGroupData->getCamera3D()->getCamera());

	// Set eye angle after camera change. Maybe create a cameraChangedSlot instead
	this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
	this->optionChangedSlot();

}

void ViewWrapper3D::showToolPathSlot(bool checked)
{
	ToolRep3DPtr activeRep3D = RepManager::findFirstRep<ToolRep3D>(mView->getReps(),
					toolManager()->getDominantTool());
	if (activeRep3D)
	{
		if (activeRep3D->getTracer()->isRunning())
		{
			activeRep3D->getTracer()->stop();
			activeRep3D->getTracer()->clear();
		}
		else
		{
			activeRep3D->getTracer()->start();
		}
	}

	settings()->setValue("showToolPath", checked);
}

void ViewWrapper3D::showAxesActionSlot(bool checked)
{
	if (mShowAxes == checked)
		return;

	mShowAxes = checked;

	// clear all
	for (unsigned i=0; i<mAxis.size(); ++i)
		mView->removeRep(mAxis[i]->mRep);
	mAxis.clear();

	// show all
	if (mShowAxes)
	{
		AxisConnectorPtr axis;

		// reference space
		axis.reset(new AxisConnector(CoordinateSystem(csREF)));
		axis->mRep->setAxisLength(0.12);
		axis->mRep->setShowAxesLabels(true);
		axis->mRep->setCaption("ref", Vector3D(1, 0, 0));
		axis->mRep->setFontSize(0.03);
		mAxis.push_back(axis);

		// data spaces
		std::vector<DataPtr> data = mGroupData->getData();
		for (unsigned i = 0; i < data.size(); ++i)
		{
			axis.reset(new AxisConnector(CoordinateSystem(csDATA, data[i]->getUid())));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption(data[i]->getName(), Vector3D(1, 0, 0));
			axis->mRep->setFontSize(0.03);
			mAxis.push_back(axis);
		}

		// tool spaces
		ToolManager::ToolMapPtr tools = toolManager()->getTools();
		ToolManager::ToolMapPtr::element_type::iterator iter;
		for (iter = tools->begin(); iter != tools->end(); ++iter)
		{
			ToolPtr tool = iter->second;

			axis.reset(new AxisConnector(CoordinateSystem(csTOOL, tool->getUid())));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption("t", Vector3D(0.7, 1, 0.7));
			axis->mRep->setFontSize(0.03);
			axis->connectTo(tool);
			CoordinateSystemListenerPtr mToolListener = axis->mListener;

			mAxis.push_back(axis);

			axis.reset(new AxisConnector(CoordinateSystem(csSENSOR, tool->getUid())));
			axis->mRep->setAxisLength(0.05);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption("s", Vector3D(1, 1, 0));
			axis->mRep->setFontSize(0.03);
			axis->connectTo(tool);
			axis->mergeWith(mToolListener);
			mAxis.push_back(axis);
		}

		for (unsigned i=0; i<mAxis.size(); ++i)
			mView->addRep(mAxis[i]->mRep);
	}
}

void ViewWrapper3D::showManualToolSlot(bool visible)
{
	settings()->setValue("showManualTool", visible);
//  ToolManager::getInstance()->getManualTool()->setVisible(visible);
}

void ViewWrapper3D::showOrientationSlot(bool visible)
{
	settings()->setValue("View/showOrientationAnnotation", visible);
	this->updateView();
}

void ViewWrapper3D::resetCameraActionSlot()
{
	mView->getRenderer()->ResetCamera();
	//Update eye angle after camera is reset
	this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
}

void ViewWrapper3D::centerImageActionSlot()
{
	if (dataManager()->getActiveImage())
		Navigation().centerToData(dataManager()->getActiveImage());
	else
		Navigation().centerToView(mGroupData->getData());
}

void ViewWrapper3D::centerToolActionSlot()
{
	Navigation().centerToTooltip();
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
	mSlicePlanes3DRep->getProxy()->setVisible(checked);
	settings()->setValue("showSlicePlanes", checked);
}
void ViewWrapper3D::fillSlicePlanesActionSlot(bool checked)
{
	mSlicePlanes3DRep->getProxy()->setDrawPlanes(checked);
}

void ViewWrapper3D::dataAdded(DataPtr data)
{
	if (!data)
		return;

	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if (image)
	{
		mMultiVolume3DRepProducer->addImage(image);
	}
	else
	{
		if (!mDataReps.count(data->getUid()))
		{
			RepPtr rep = this->createDataRep3D(data);
			if (!rep)
				return;
			mDataReps[data->getUid()] = rep;
			mView->addRep(rep);

	//			ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	//			if (image)
	//			{
	//				connect(image.get(), SIGNAL(clipPlanesChanged()), this, SLOT(updateView()));
	//				connect(image.get(), SIGNAL(cropBoxChanged()), this, SLOT(updateView()));
	//			}
		}
	}


	this->activeImageChangedSlot();
	this->updateView();
}

void ViewWrapper3D::dataRemoved(const QString& uid)
{
	mMultiVolume3DRepProducer->removeImage(uid);
	if (mDataReps.count(uid))
	{
		mView->removeRep(mDataReps[uid]);
		mDataReps.erase(uid);
	}

	this->activeImageChangedSlot();
	this->updateView();
}

/**Construct a 3D standard rep for a given data.
 *
 */
RepPtr ViewWrapper3D::createDataRep3D(DataPtr data)
{
	if (boost::dynamic_pointer_cast<Mesh>(data))
	{
		GeometricRepPtr rep = GeometricRep::New(data->getUid() + "_geom3D_rep");
		rep->setMesh(boost::dynamic_pointer_cast<Mesh>(data));
		return rep;
	}
    else
    {
        DataMetricRepPtr rep = this->createDataMetricRep3D(data);
        if (rep)
            return rep;
    }

    return RepPtr();
}

DataMetricRepPtr ViewWrapper3D::createDataMetricRep3D(DataPtr data)
{
    DataMetricRepPtr rep;

    if (boost::dynamic_pointer_cast<PointMetric>(data))
        rep = PointMetricRep::New(data->getUid() + "_3D_rep");
    else if (boost::dynamic_pointer_cast<FrameMetric>(data))
        rep = FrameMetricRep::New(data->getUid() + "_3D_rep");
	else if (boost::dynamic_pointer_cast<ToolMetric>(data))
		rep = ToolMetricRep::New(data->getUid() + "_3D_rep");
	else if (boost::dynamic_pointer_cast<DistanceMetric>(data))
        rep = DistanceMetricRep::New(data->getUid() + "_3D_rep");
    else if (boost::dynamic_pointer_cast<AngleMetric>(data))
        rep = AngleMetricRep::New(data->getUid() + "_3D_rep");
    else if (boost::dynamic_pointer_cast<PlaneMetric>(data))
        rep = PlaneMetricRep::New(data->getUid() + "_3D_rep");

    if (rep)
    {
        this->readDataRepSettings(rep);
        rep->setDataMetric(boost::dynamic_pointer_cast<DataMetric>(data));
    }
    return rep;
}

/**helper. Read settings common for all data metric reps.
 *
 */
void ViewWrapper3D::readDataRepSettings(RepPtr rep)
{
	DataMetricRepPtr val = boost::dynamic_pointer_cast<DataMetricRep>(rep);
	if (!val)
		return;

	val->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
	val->setShowLabel(settings()->value("View/showLabels").toBool());
	val->setLabelSize(settings()->value("View3D/labelSize").toDouble());
}

void ViewWrapper3D::updateView()
{
	QString text;
	bool show = settings()->value("View/showDataText").value<bool>();

	if (show)
		text = this->getAllDataNames().join("\n");
	mDataNameText->setText(0, text);
	mDataNameText->setFontSize(std::max(12, 22 - 2 * text.size()));

	mAnnotationMarker->setVisible(settings()->value("View/showOrientationAnnotation").value<bool>());
}

void ViewWrapper3D::activeImageChangedSlot()
{
	if(!mGroupData)
		return;
	ImagePtr image = dataManager()->getActiveImage();

	// only show landmarks belonging to image visible in this view:
	std::vector<ImagePtr> images = mGroupData->getImages();
	if (!std::count(images.begin(), images.end(), image))
		image.reset();
}

void ViewWrapper3D::showRefToolSlot(bool checked)
{
	ToolPtr refTool = toolManager()->getReferenceTool();
	if (!refTool)
		return;
	ToolRep3DPtr refRep = RepManager::findFirstRep<ToolRep3D>(mView->getReps(), refTool);
	if (!refRep)
	{
		refRep = ToolRep3D::New(refTool->getUid() + "_rep3d_" + this->mView->getUid());
		refRep->setTool(refTool);
	}

	if (checked) //should show
		mView->addRep(refRep);
	else
		//should not show
		mView->removeRep(refRep);
}


void ViewWrapper3D::updateSlices()
{
//#ifdef USE_GLX_SHARED_CONTEXT
	if (mSlices3DRep)
		mView->removeRep(mSlices3DRep);
	//Simple bug fix of #746: Don't create slices if no volumes exist in 3D scene
	if (!mGroupData || mGroupData->getImages().empty())
	{
		messageManager()->sendWarning("Need volumes in the 3D scene to create 2D slices");
		return;
	}

	mSlices3DRep = Slices3DRep::New("MultiSliceRep_" + mView->getName());

	PLANE_TYPE type = string2enum<PLANE_TYPE>(mShowSlicesMode);
	if (type != ptCOUNT)
	{
		mSlices3DRep->addPlane(type);
	}
	else if (mShowSlicesMode == "ACS")
	{
		mSlices3DRep->addPlane(ptAXIAL);
		mSlices3DRep->addPlane(ptSAGITTAL);
		mSlices3DRep->addPlane(ptCORONAL);
	}
	else
	{
		mSlices3DRep.reset();
		return;
	}

	mSlices3DRep->setShaderPath(DataLocations::getShaderPath());
	if (mGroupData && !mGroupData->getImages().empty())
		mSlices3DRep->setImages(mGroupData->getImages());
	mSlices3DRep->setTool(toolManager()->getDominantTool());
	mView->addRep(mSlices3DRep);
//#endif // USE_GLX_SHARED_CONTEXT
}

ViewWidget* ViewWrapper3D::getView()
{
	return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
	ToolPtr dominantTool = toolManager()->getDominantTool();
	mPickerRep->setTool(dominantTool);
	if (mSlices3DRep)
		mSlices3DRep->setTool(dominantTool);
}

void ViewWrapper3D::toolsAvailableSlot()
{
	ToolManager::ToolMapPtr tools = toolManager()->getTools();
	ToolManager::ToolMapPtr::element_type::iterator iter;
	for (iter = tools->begin(); iter != tools->end(); ++iter)
	{
		ToolPtr tool = iter->second;
		if (tool->hasType(Tool::TOOL_REFERENCE))
			continue;

		ToolRep3DPtr toolRep = RepManager::findFirstRep<ToolRep3D>(mView->getReps(), tool);

		if (tool->hasType(Tool::TOOL_MANUAL) && !settings()->value("showManualTool").toBool())
		{
			if (toolRep)
				mView->removeRep(toolRep);
			continue;
		}
		//    mManualTool->setVisible(settings()->value("showManualTool").toBool());

		if (!toolRep)
		{
			toolRep = ToolRep3D::New(tool->getUid() + "_rep3d_" + this->mView->getUid());
			if (settings()->value("showToolPath").toBool())
				toolRep->getTracer()->start();
		}

		toolRep->setSphereRadius(settings()->value("View3D/sphereRadius").toDouble()); // use fraction of set size
		toolRep->setSphereRadiusInNormalizedViewport(true);

		toolRep->setTool(tool);
		toolRep->setOffsetPointVisibleAtZeroOffset(true);
		mView->addRep(toolRep);
	}
}

void ViewWrapper3D::optionChangedSlot()
{
	ViewGroupData::Options options = mGroupData->getOptions();

	this->showLandmarks(options.mShowLandmarks);
	this->showPointPickerProbe(options.mShowPointPickerProbe);
	mPickerRep->setGlyph(options.mPickerGlyph);
}

void ViewWrapper3D::showLandmarks(bool on)
{
	if (mLandmarkRep->isConnectedToView(mView) == on)
		return;

	if (on)
	{
		mView->addRep(mLandmarkRep);
	}
	else
	{
		mView->removeRep(mLandmarkRep);
	}
}

void ViewWrapper3D::showPointPickerProbe(bool on)
{
	mPickerRep->setEnabled(on);
}

void ViewWrapper3D::setSlicePlanesProxy(SlicePlanesProxyPtr proxy)
{
	mSlicePlanes3DRep = SlicePlanes3DRep::New("uid");
	mSlicePlanes3DRep->setProxy(proxy);
	mSlicePlanes3DRep->setDynamicLabelSize(true);
	bool show = settings()->value("showSlicePlanes").toBool();
	mSlicePlanes3DRep->getProxy()->setVisible(show); // init with default value

	mView->addRep(mSlicePlanes3DRep);
}

void ViewWrapper3D::setStereoType(int /*STEREOTYPE*/type)
{
	switch (type)
	//STEREOTYPE
	{
	case stFRAME_SEQUENTIAL:
		mView->getRenderWindow()->SetStereoTypeToCrystalEyes();
		break;
	case stINTERLACED:
		mView->getRenderWindow()->SetStereoTypeToInterlaced();
		break;
	case stDRESDEN:
		mView->getRenderWindow()->SetStereoTypeToDresden();
		break;
	case stRED_BLUE:
		mView->getRenderWindow()->SetStereoTypeToRedBlue();
		break;
	}
}

void ViewWrapper3D::globalConfigurationFileChangedSlot(QString key)
{
	if (key == "View3D/stereoType")
	{
		this->setStereoType(settings()->value("View3D/stereoType").toInt());
	}
	else if (key == "View3D/eyeAngle")
	{
		this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
	}
}

void ViewWrapper3D::setStereoEyeAngle(double angle)
{
	mView->getRenderer()->GetActiveCamera()->SetEyeAngle(angle);
}

void ViewWrapper3D::setTranslucentRenderingToDepthPeeling(bool setDepthPeeling)
{
	bool success = true;
	if(setDepthPeeling)
	{
		vtkSmartPointer<vtkAppendPolyData> translucentGeometry = GenerateOverlappingBunchOfSpheres(100, 100);
	  // generate a basic Mapper and Actor
	  vtkSmartPointer<vtkPolyDataMapper> mapper =
	    vtkSmartPointer<vtkPolyDataMapper>::New();
	  mapper->SetInputConnection(translucentGeometry->GetOutputPort());
	  vtkSmartPointer<vtkActor> actor =
	    vtkSmartPointer<vtkActor>::New();
	  actor->SetMapper(mapper);
	  actor->GetProperty()->SetOpacity(0.5); // translucent !!!
	  actor->GetProperty()->SetColor(1, 0, 0);
	  actor->RotateX(-72); // put the objects in a position where it is easy to see
	                       // different overlapping regions

	  mView->getRenderer()->AddActor(actor); //Test add to 3D view

		/*if (!IsDepthPeelingSupported(mView->getRenderWindow(), mView->getRenderer(), true))
		{
			messageManager()->sendWarning("GPU do not support depth peeling. Rendering of translucent surfaces is not supported");
			success = false;
		}
		else*/ if (!SetupEnvironmentForDepthPeeling(mView->getRenderWindow(), mView->getRenderer(), 100, 0.1))
		{
			messageManager()->sendWarning("Error setting depth peeling");
			success = false;
		}
		else
		{
			messageManager()->sendInfo("Set GPU depth peeling");
		}
		if(!success)
		  settings()->setValue("View3D/depthPeeling", false);
	} else
	{
		if (TurnOffDepthPeeling(mView->getRenderWindow(), mView->getRenderer()))
			messageManager()->sendInfo("Depth peeling turned off");
	}
}

//------------------------------------------------------------------------------
}
