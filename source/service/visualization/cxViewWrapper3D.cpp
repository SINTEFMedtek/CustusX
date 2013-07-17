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
#include "sscDataMetricRep.h"
#include "cxDataLocations.h"
#include "sscTexture3DSlicerRep.h"
#include "sscSlices3DRep.h"
#include "sscEnumConverter.h"
#include "sscManualTool.h"
#include "sscImage2DRep3D.h"

#include "sscData.h"
#include "sscAxesRep.h"
#include "cxViewGroup.h"

#include "sscAngleMetric.h"
#include "sscDistanceMetric.h"
#include "sscPointMetric.h"

#include "cxDepthPeeling.h"

namespace cx
{

AxisConnector::AxisConnector(ssc::CoordinateSystem space)
{
	mListener.reset(new ssc::CoordinateSystemListener(space));
	connect(mListener.get(), SIGNAL(changed()), this, SLOT(changedSlot()));

	mRep = ssc::AxesRep::New(space.toString() + "_axis");
	mRep->setCaption(space.toString(), ssc::Vector3D(1, 0, 0));
	mRep->setShowAxesLabels(false);
	mRep->setFontSize(0.08);
	mRep->setAxisLength(0.03);
	this->changedSlot();
}

void AxisConnector::mergeWith(ssc::CoordinateSystemListenerPtr base)
{
	mBase = base;
	connect(mBase.get(), SIGNAL(changed()), this, SLOT(changedSlot()));
	this->changedSlot();
}

void AxisConnector::connectTo(ssc::ToolPtr tool)
{
	mTool = tool;
	connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(changedSlot()));
	this->changedSlot();
}

void AxisConnector::changedSlot()
{
	ssc::Transform3D  rMs = ssc::SpaceHelpers::get_toMfrom(mListener->getSpace(), ssc::CoordinateSystem(ssc::csREF));
	mRep->setTransform(rMs);

	mRep->setVisible(true);

	// if connected to tool: check visibility
	if (mTool)
		mRep->setVisible(mTool->getVisible());

	// Dont show if equal to base
	if (mBase)
	{
		ssc::Transform3D rMb = ssc::SpaceHelpers::get_toMfrom(mBase->getSpace(), ssc::CoordinateSystem(ssc::csREF));
		if (ssc::similar(rMb, rMs))
			mRep->setVisible(false);
	}

}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ViewWrapper3D::ViewWrapper3D(int startIndex, ssc::ViewWidget* view)
{
	mShowAxes = false;
	mView = view;
	this->connectContextMenu(mView);
	QString index = QString::number(startIndex);
	QColor background = settings()->value("backgroundColor").value<QColor>();
	mView->setBackgroundColor(background);

	view->getRenderer()->GetActiveCamera()->SetParallelProjection(false);
	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	mLandmarkRep = LandmarkRep::New("LandmarkRep_" + index);
	mLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
	mLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());

	mPickerRep = ssc::PickerRep::New("PickerRep_" + index, "PickerRep_" + index);

	connect(mPickerRep.get(), SIGNAL(pointPicked(ssc::Vector3D)), this, SLOT(PickerRepPointPickedSlot(ssc::Vector3D)));
	connect(mPickerRep.get(), SIGNAL(dataPicked(QString)), this, SLOT(PickerRepDataPickedSlot(QString)));
	mPickerRep->setSphereRadius(settings()->value("View3D/sphereRadius").toDouble());
	mPickerRep->setEnabled(false);
	mView->addRep(mPickerRep);
	connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
	this->dominantToolChangedSlot();

	// plane type text rep
	mPlaneTypeText = ssc::DisplayTextRep::New("planeTypeRep_" + mView->getName(), "");
	mPlaneTypeText->addText(ssc::Vector3D(0, 1, 0), "3D", ssc::Vector3D(0.98, 0.02, 0.0));
	mView->addRep(mPlaneTypeText);

	//data name text rep
	mDataNameText = ssc::DisplayTextRep::New("dataNameText_" + mView->getName(), "");
	mDataNameText->addText(ssc::Vector3D(0, 1, 0), "not initialized", ssc::Vector3D(0.02, 0.02, 0.0));
	mView->addRep(mDataNameText);

	connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(toolsAvailableSlot()));
	connect(ssc::toolManager(), SIGNAL(initialized()), this, SLOT(toolsAvailableSlot()));
	connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChangedSlot()));
	this->toolsAvailableSlot();

	mAnnotationMarker = RepManager::getInstance()->getCachedRep<ssc::OrientationAnnotation3DRep>(
					"annotation_" + mView->getName());
//  mAnnotationMarker = ssc::OrientationAnnotation3DRep::New("annotation_"+mView->getName(), "");
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

//	connect(viewManager()->getClipper().get(), SIGNAL(changed()), this, SLOT(updateView()));
	this->updateView();
}

ViewWrapper3D::~ViewWrapper3D()
{
//	disconnect(viewManager()->getClipper().get(), SIGNAL(changed()), this, SLOT(updateView()));
	if (mView)
	{
		mView->removeReps();
	}
}

void ViewWrapper3D::settingsChangedSlot(QString key)
{
	if (key == "backgroundColor")
	{
		QColor background = settings()->value("backgroundColor").value<QColor>();
		mView->setBackgroundColor(background);
	}
	if (( key=="useGPUVolumeRayCastMapper" )||( key=="maxRenderSize" ))
	{
		// reload volumes from cache
		std::vector<ssc::ImagePtr> images = mViewGroup->getImages();
		for (unsigned i = 0; i < images.size(); ++i)
		{
			this->dataRemoved(images[i]->getUid());
			this->dataAdded(images[i]);
		}
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
		//    mPatientLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
		//    mPatientLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());
	}
	if (key == "View3D/depthPeeling")
		this->setTranslucentRenderingToDepthPeeling(settings()->value("View3D/depthPeeling").toBool());
}

void ViewWrapper3D::PickerRepPointPickedSlot(ssc::Vector3D p_r)
{
	ssc::Transform3D rMpr = *ssc::toolManager()->get_rMpr();
	ssc::Vector3D p_pr = rMpr.inv().coord(p_r);

	// set the picked point as offset tip
	ssc::ManualToolPtr tool = ToolManager::getInstance()->getManualTool();
	ssc::Vector3D offset = tool->get_prMt().vector(ssc::Vector3D(0, 0, tool->getTooltipOffset()));
	p_pr -= offset;
	p_r = rMpr.coord(p_pr);

	// TODO set center here will not do: must handle
	ssc::dataManager()->setCenter(p_r);
	ssc::Vector3D p0_pr = tool->get_prMt().coord(ssc::Vector3D(0, 0, 0));
	tool->set_prMt(ssc::createTransformTranslate(p_pr - p0_pr) * tool->get_prMt());
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

	QAction* showOrientation = new QAction("Show Orientation (i)", &contextMenu);
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
	ssc::ToolPtr refTool = ToolManager::getInstance()->getReferenceTool();
	if (refTool)
	{
		showRefTool->setText("Show " + refTool->getName());
		showRefTool->setEnabled(true);
		showRefTool->setChecked(RepManager::findFirstRep<ssc::ToolRep3D>(mView->getReps(), refTool));
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
	mView->getRenderer()->SetActiveCamera(mViewGroup->getCamera3D()->getCamera());

	// Set eye angle after camera change. Maybe create a cameraChangedSlot instead
	this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
	this->optionChangedSlot();

}

void ViewWrapper3D::showToolPathSlot(bool checked)
{
	ssc::ToolRep3DPtr activeRep3D = RepManager::findFirstRep<ssc::ToolRep3D>(mView->getReps(),
					ssc::toolManager()->getDominantTool());
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
		axis.reset(new AxisConnector(ssc::CoordinateSystem(ssc::csREF)));
		axis->mRep->setAxisLength(0.12);
		axis->mRep->setShowAxesLabels(true);
		axis->mRep->setCaption("ref", ssc::Vector3D(1, 0, 0));
		axis->mRep->setFontSize(0.03);
		mAxis.push_back(axis);

		// data spaces
		std::vector<ssc::DataPtr> data = mViewGroup->getData();
		for (unsigned i = 0; i < data.size(); ++i)
		{
			axis.reset(new AxisConnector(ssc::CoordinateSystem(ssc::csDATA, data[i]->getUid())));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption(data[i]->getName(), ssc::Vector3D(1, 0, 0));
			axis->mRep->setFontSize(0.03);
			mAxis.push_back(axis);
		}

		// tool spaces
		ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
		ssc::ToolManager::ToolMapPtr::element_type::iterator iter;
		for (iter = tools->begin(); iter != tools->end(); ++iter)
		{
			ssc::ToolPtr tool = iter->second;

			axis.reset(new AxisConnector(ssc::CoordinateSystem(ssc::csTOOL, tool->getUid())));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption("t", ssc::Vector3D(0.7, 1, 0.7));
			axis->mRep->setFontSize(0.03);
			axis->connectTo(tool);
			ssc::CoordinateSystemListenerPtr mToolListener = axis->mListener;

			mAxis.push_back(axis);

			axis.reset(new AxisConnector(ssc::CoordinateSystem(ssc::csSENSOR, tool->getUid())));
			axis->mRep->setAxisLength(0.05);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption("s", ssc::Vector3D(1, 1, 0));
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
	if (ssc::dataManager()->getActiveImage())
		Navigation().centerToData(ssc::dataManager()->getActiveImage());
	else
		Navigation().centerToView(mViewGroup->getData());
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

void ViewWrapper3D::dataAdded(ssc::DataPtr data)
{
	if (!data)
		return;

	if (!mDataReps.count(data->getUid()))
	{
		ssc::RepPtr rep = this->createDataRep3D(data);
		if (!rep)
			return;
		mDataReps[data->getUid()] = rep;
		mView->addRep(rep);

		ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(data);
		if (image)
		{
			connect(image.get(), SIGNAL(clipPlanesChanged()), this, SLOT(updateView()));
			connect(image.get(), SIGNAL(cropBoxChanged()), this, SLOT(updateView()));
		}
	}

	this->activeImageChangedSlot();
	this->updateView();
}

void ViewWrapper3D::dataRemoved(const QString& uid)
{
	if (!mDataReps.count(uid))
		return;

	ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
	if (image)
	{
		disconnect(image.get(), SIGNAL(clipPlanesChanged()), this, SLOT(updateView()));
		disconnect(image.get(), SIGNAL(cropBoxChanged()), this, SLOT(updateView()));
	}

	mView->removeRep(mDataReps[uid]);
	mDataReps.erase(uid);

	this->activeImageChangedSlot();
	this->updateView();
}

/**Construct a 3D standard rep for a given data.
 *
 */
ssc::RepPtr ViewWrapper3D::createDataRep3D(ssc::DataPtr data)
{
	if (boost::dynamic_pointer_cast<ssc::Image>(data))
	{
		ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(data);
		if (image->getBaseVtkImageData()->GetDimensions()[2]==1)
		{
			cx::Image2DRep3DPtr rep = cx::Image2DRep3D::New(data->getUid()+"image2DRep");
			rep->setImage(image);
			return rep;
		}
		else
		{
			ssc::VolumetricBaseRepPtr rep = RepManager::getInstance()->getVolumetricRep(image);
			return rep;
		}
	}
	else if (boost::dynamic_pointer_cast<ssc::Mesh>(data))
	{
		ssc::GeometricRepPtr rep = ssc::GeometricRep::New(data->getUid() + "_geom3D_rep");
		rep->setMesh(boost::dynamic_pointer_cast<ssc::Mesh>(data));
		return rep;
	}
	else if (boost::dynamic_pointer_cast<ssc::PointMetric>(data))
	{
		ssc::PointMetricRepPtr rep = ssc::PointMetricRep::New(data->getUid() + "_3D_rep");
		this->readDataRepSettings(rep);
		rep->setPointMetric(boost::dynamic_pointer_cast<ssc::PointMetric>(data));
		return rep;
	}
	else if (boost::dynamic_pointer_cast<ssc::DistanceMetric>(data))
	{
		ssc::DistanceMetricRepPtr rep = ssc::DistanceMetricRep::New(data->getUid() + "_3D_rep");
		this->readDataRepSettings(rep);
		rep->setDistanceMetric(boost::dynamic_pointer_cast<ssc::DistanceMetric>(data));
		return rep;
	}
	else if (boost::dynamic_pointer_cast<ssc::AngleMetric>(data))
	{
		ssc::AngleMetricRepPtr rep = ssc::AngleMetricRep::New(data->getUid() + "_3D_rep");
		this->readDataRepSettings(rep);
		rep->setMetric(boost::dynamic_pointer_cast<ssc::AngleMetric>(data));
		return rep;
	}
	else if (boost::dynamic_pointer_cast<ssc::PlaneMetric>(data))
	{
		ssc::PlaneMetricRepPtr rep = ssc::PlaneMetricRep::New(data->getUid() + "_3D_rep");
		this->readDataRepSettings(rep);
		rep->setMetric(boost::dynamic_pointer_cast<ssc::PlaneMetric>(data));
		return rep;
	}

	return ssc::RepPtr();
}

/**helper. Read settings common for all data metric reps.
 *
 */
void ViewWrapper3D::readDataRepSettings(ssc::RepPtr rep)
{
	ssc::DataMetricRepPtr val = boost::dynamic_pointer_cast<ssc::DataMetricRep>(rep);
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
	if(!mViewGroup)
		return;
	ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

	// only show landmarks belonging to image visible in this view:
	std::vector<ssc::ImagePtr> images = mViewGroup->getImages();
	if (!std::count(images.begin(), images.end(), image))
		image.reset();
}

void ViewWrapper3D::showRefToolSlot(bool checked)
{
	ssc::ToolPtr refTool = ssc::toolManager()->getReferenceTool();
	if (!refTool)
		return;
	ssc::ToolRep3DPtr refRep = RepManager::findFirstRep<ssc::ToolRep3D>(mView->getReps(), refTool);
	if (!refRep)
	{
		refRep = ssc::ToolRep3D::New(refTool->getUid() + "_rep3d_" + this->mView->getUid());
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
#ifdef USE_GLX_SHARED_CONTEXT
	if (mSlices3DRep)
		mView->removeRep(mSlices3DRep);
	//Simple bug fix of #746: Don't create slices if no volumes exist in 3D scene
	if (!mViewGroup || mViewGroup->getImages().empty())
	{
		ssc::messageManager()->sendWarning("Need volumes in the 3D scene to create 2D slices");
		return;
	}

	mSlices3DRep = ssc::Slices3DRep::New("MultiSliceRep_" + mView->getName());

	ssc::PLANE_TYPE type = string2enum<ssc::PLANE_TYPE>(mShowSlicesMode);
	if (type != ssc::ptCOUNT)
	{
		mSlices3DRep->addPlane(type);
	}
	else if (mShowSlicesMode == "ACS")
	{
		mSlices3DRep->addPlane(ssc::ptAXIAL);
		mSlices3DRep->addPlane(ssc::ptSAGITTAL);
		mSlices3DRep->addPlane(ssc::ptCORONAL);
	}
	else
	{
		mSlices3DRep.reset();
		return;
	}

	mSlices3DRep->setShaderFile(DataLocations::getShaderPath() + "/Texture3DOverlay.frag");
	if (mViewGroup && !mViewGroup->getImages().empty())
		mSlices3DRep->setImages(mViewGroup->getImages());
	mSlices3DRep->setTool(ssc::toolManager()->getDominantTool());
//	return mSlices3DRep;
	mView->addRep(mSlices3DRep);
#endif // USE_GLX_SHARED_CONTEXT
}

ssc::ViewWidget* ViewWrapper3D::getView()
{
	return mView;
}

void ViewWrapper3D::dominantToolChangedSlot()
{
	ssc::ToolPtr dominantTool = ssc::toolManager()->getDominantTool();
	mPickerRep->setTool(dominantTool);
	if (mSlices3DRep)
		mSlices3DRep->setTool(dominantTool);
}

void ViewWrapper3D::toolsAvailableSlot()
{
	ssc::ToolManager::ToolMapPtr tools = ssc::toolManager()->getTools();
	ssc::ToolManager::ToolMapPtr::element_type::iterator iter;
	for (iter = tools->begin(); iter != tools->end(); ++iter)
	{
		ssc::ToolPtr tool = iter->second;
		if (tool->hasType(ssc::Tool::TOOL_REFERENCE))
			continue;

		ssc::ToolRep3DPtr toolRep = RepManager::findFirstRep<ssc::ToolRep3D>(mView->getReps(), tool);

		if (tool->hasType(ssc::Tool::TOOL_MANUAL) && !settings()->value("showManualTool").toBool())
		{
			if (toolRep)
				mView->removeRep(toolRep);
			continue;
		}
		//    mManualTool->setVisible(settings()->value("showManualTool").toBool());

		if (!toolRep)
		{
			toolRep = ssc::ToolRep3D::New(tool->getUid() + "_rep3d_" + this->mView->getUid());
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
	ViewGroupData::Options options = mViewGroup->getOptions();

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
		//mView->addRep(mPatientLandmarkRep);
		mView->addRep(mLandmarkRep);
	}
	else
	{
		//mView->removeRep(mPatientLandmarkRep);
		mView->removeRep(mLandmarkRep);
	}
}

void ViewWrapper3D::showPointPickerProbe(bool on)
{
	mPickerRep->setEnabled(on);
}

void ViewWrapper3D::setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy)
{
	mSlicePlanes3DRep = ssc::SlicePlanes3DRep::New("uid");
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
			ssc::messageManager()->sendWarning("GPU do not support depth peeling. Rendering of translucent surfaces is not supported");
			success = false;
		}
		else*/ if (!SetupEnvironmentForDepthPeeling(mView->getRenderWindow(), mView->getRenderer(), 100, 0.1))
		{
			ssc::messageManager()->sendWarning("Error setting depth peeling");
			success = false;
		}
		else
		{
			ssc::messageManager()->sendInfo("Set GPU depth peeling");
		}
		if(!success)
		  settings()->setValue("View3D/depthPeeling", false);
	} else
	{
		if (TurnOffDepthPeeling(mView->getRenderWindow(), mView->getRenderer()))
			ssc::messageManager()->sendInfo("Depth peeling turned off");
	}
}

//------------------------------------------------------------------------------
}
