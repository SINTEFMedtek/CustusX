/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewWrapper3D.h"

#include <vector>

#include "boost/bind.hpp"
#include "boost/function.hpp"

#include <QAction>
#include <QMenu>

#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>

#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxToolRep2D.h"
#include "cxDisplayTextRep.h"
#include "cxLogger.h"
#include "cxSlicePlanes3DRep.h"
#include "cxMesh.h"
#include "cxPickerRep.h"
#include "cxGeometricRep.h"
#include "cxToolRep3D.h"
#include "cxTypeConversions.h"
#include "cxVideoSource.h"
#include "cxVideoRep.h"
#include "cxToolTracer.h"
#include "cxOrientationAnnotation3DRep.h"
#include "cxSettings.h"
#include "cxTrackingService.h"
#include "cxRepManager.h"
#include "cxCameraControl.h"
#include "cxLandmarkRep.h"
#include "cxPointMetricRep.h"
#include "cxDistanceMetricRep.h"
#include "cxRegionOfInterestMetricRep.h"
#include "cxAngleMetricRep.h"
#include "cxPlaneMetricRep.h"
#include "cxFrameMetricRep.h"
#include "cxToolMetricRep.h"
#include "cxDataMetricRep.h"
#include "cxDataLocations.h"
#include "cxManualTool.h"
#include "cxImage2DRep3D.h"

#include "cxPatientModelService.h"
#include "cxRepContainer.h"


#include "cxData.h"
#include "cxAxesRep.h"
#include "cxViewGroup.h"

#include "cxAngleMetric.h"
#include "cxDistanceMetric.h"
#include "cxPointMetric.h"
#include "cxSphereMetric.h"
#include "cxShapedMetric.h"
#include "cxCustomMetric.h"
#include "cxSphereMetricRep.h"
#include "cxDonutMetricRep.h"
#include "cxCustomMetricRep.h"

#include "cxDepthPeeling.h"
#include "cxAxisConnector.h"
#include "cxMultiVolume3DRepProducer.h"
#include "cxMetricNamesRep.h"
#include "cxVisServices.h"
#include "cxNavigation.h"

#include "cxTrackedStream.h"
#include "cxStreamRep3D.h"
#include "cxStream2DRep3D.h"
#include "cxActiveData.h"

#include "cxSlices3DRep.h"

namespace cx
{

ViewWrapper3D::ViewWrapper3D(int startIndex, ViewPtr view, VisServicesPtr services):
	ViewWrapper(services)
{
	view->getRenderer()->GetActiveCamera()->SetClippingRange(0.1, 2000);
	if (!view->getRenderWindow()->GetStereoCapableWindow())
		view->getRenderWindow()->StereoCapableWindowOn(); // Just set all 3D views 3D capable

	mShowAxes = false;
	mView = view;
	this->connectContextMenu(mView);
	QString index = QString::number(startIndex);
	QColor background = settings()->value("backgroundColor").value<QColor>();
	mView->setBackgroundColor(background);

	view->getRenderer()->GetActiveCamera()->SetParallelProjection(false);
//	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(settingsChangedSlot(QString)));

	this->initializeMultiVolume3DRepProducer();

	mLandmarkRep = LandmarkRep::New(mServices->patient());
	mLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
	mLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());

	mPickerRep = PickerRep::New(mServices->patient());

	connect(mPickerRep.get(), SIGNAL(pointPicked(Vector3D)), this, SLOT(pickerRepPointPickedSlot(Vector3D)));
//	connect(mPickerRep.get(), SIGNAL(dataPicked(QString)), this, SLOT(pickerRepDataPickedSlot(QString)));
	mPickerRep->setSphereRadius(settings()->value("View3D/sphereRadius").toDouble());
	mPickerRep->setEnabled(false);
	mView->addRep(mPickerRep);
	connect(mServices->tracking().get(), SIGNAL(activeToolChanged(const QString&)), this, SLOT(activeToolChangedSlot()));
	this->activeToolChangedSlot();

	this->ViewWrapper::addReps();

	//data name text rep
	this->updateMetricNamesRep();

	connect(mServices->tracking().get(), &TrackingService::stateChanged, this, &ViewWrapper3D::toolsAvailableSlot);
	connect(mServices->patient()->getActiveData().get(), &ActiveData::activeImageChanged, this, &ViewWrapper3D::activeImageChangedSlot);
	this->toolsAvailableSlot();

	mAnnotationMarker = RepManager::getInstance()->getCachedRep<OrientationAnnotation3DRep>();
	this->settingsChangedSlot("View3D/annotationModel");
	this->settingsChangedSlot("View3D/annotationModelSize");
	mView->addRep(mAnnotationMarker);

//Stereo
//  mView->getRenderWindow()->StereoCapableWindowOn(); // Moved to cxView3D
//	connect(settings(), SIGNAL(valueChangedFor(QString)), this, SLOT(globalConfigurationFileChangedSlot(QString)));
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
		reportError("Missing View in initializeMultiVolume3DRepProducer");

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
	this->ViewWrapper::settingsChangedSlot(key);

	if (key == "View3D/stereoType")
	{
		this->setStereoType(settings()->value("View3D/stereoType").toInt());
	}
	if (key == "View3D/eyeAngle")
	{
		this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
	}
	if (key == "backgroundColor")
	{
		QColor background = settings()->value("backgroundColor").value<QColor>();
		mView->setBackgroundColor(background);
	}
	if (( key=="View3D/ImageRender3DVisualizer" )||( key=="View3D/maxRenderSize" ))
	{
		this->initializeMultiVolume3DRepProducer();
	}
	if ((key == "View/showDataText")
			|| (key == "View/showOrientationAnnotation"))
	{
		this->updateView();
	}
	if ((key == "View3D/annotationModelSize" )||( key == "View3D/annotationModel"))
	{
		QString annotationFile = settings()->value("View3D/annotationModel").toString();
		mAnnotationMarker->setMarkerFilename(DataLocations::findConfigFilePath(annotationFile, "/models"));
		mAnnotationMarker->setSize(settings()->value("View3D/annotationModelSize").toDouble());
	}
	if (key == "View3D/showManualTool")
	{
		this->toolsAvailableSlot();
	}
	if ((key == "View3D/sphereRadius" )
			||( key == "View3D/labelSize" )
			||( key == "View/showLabels")
			||( key == "View/showMetricNamesInCorner"))
	{
		for (RepMap::iterator iter = mDataReps.begin(); iter != mDataReps.end(); ++iter)
		{
			this->readDataRepSettings(iter->second);
		}

		this->updateMetricNamesRep();

		this->toolsAvailableSlot();
		mLandmarkRep->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
		mLandmarkRep->setLabelSize(settings()->value("View3D/labelSize").toDouble());
	}
	if (key == "View3D/depthPeeling")
		this->setTranslucentRenderingToDepthPeeling(settings()->value("View3D/depthPeeling").toBool());
}

void ViewWrapper3D::updateMetricNamesRep()
{
	bool enabled = settings()->value("View/showMetricNamesInCorner").value<bool>();

	if (enabled)
	{
		if (!mMetricNames)
		{
			mMetricNames = MetricNamesRep::New();
			mView->addRep(mMetricNames);
		}

		if (mGroupData)
			mMetricNames->setData(mGroupData->getData());
	}
	else
	{
		mView->removeRep(mMetricNames);
		mMetricNames.reset();
	}
}

void ViewWrapper3D::pickerRepPointPickedSlot(Vector3D p_r)
{
	NavigationPtr nav = this->getNavigation();
	nav->centerToPosition(p_r, Navigation::v2D);
}

void ViewWrapper3D::pickerRepDataPickedSlot(QString uid)
{
	//std::cout << "picked: " << uid << std::endl;
}

void ViewWrapper3D::appendToContextMenu(QMenu& contextMenu)
{	
	QAction* slicePlanesAction = NULL;
	QAction* fillSlicePlanesAction = NULL;
	if (mSlicePlanes3DRep)
	{
		slicePlanesAction = new QAction("Show Slice Planes", &contextMenu);
		slicePlanesAction->setCheckable(true);
		slicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getVisible());
		slicePlanesAction->setToolTip("Visialize the outline of the 2D views in 3D");
		connect(slicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(showSlicePlanesActionSlot(bool)));

		fillSlicePlanesAction = new QAction("Fill Slice Planes", &contextMenu);
		fillSlicePlanesAction->setCheckable(true);
		fillSlicePlanesAction->setEnabled(mSlicePlanes3DRep->getProxy()->getVisible());
		fillSlicePlanesAction->setChecked(mSlicePlanes3DRep->getProxy()->getDrawPlanes());
		slicePlanesAction->setToolTip("Fill the visualized 2D views with color");
		connect(fillSlicePlanesAction, SIGNAL(triggered(bool)), this, SLOT(fillSlicePlanesActionSlot(bool)));
	}

	QAction* resetCameraAction = new QAction("Reset Camera (r)", &contextMenu);
	resetCameraAction->setToolTip("Zoom out, and show all objects in the view");
	connect(resetCameraAction, SIGNAL(triggered()), this, SLOT(resetCameraActionSlot()));

	QAction* centerImageAction = new QAction("Center to image", &contextMenu);
	centerImageAction->setToolTip("Move view to show center of active image in all views (no zoom)");
	connect(centerImageAction, SIGNAL(triggered()), this, SLOT(centerImageActionSlot()));

	QAction* centerToolAction = new QAction("Center to tool", &contextMenu);
	centerToolAction->setToolTip("Move view to show active tool in all views (no zoom)");
	connect(centerToolAction, SIGNAL(triggered()), this, SLOT(centerToolActionSlot()));

	QAction* showAxesAction = new QAction("Show Coordinate Axes", &contextMenu);
	showAxesAction->setCheckable(true);
	showAxesAction->setChecked(mShowAxes);
	showAxesAction->setToolTip("Show coordinate axes for all objects in 3D scene.\n"
							   "Axes are placed in obejct origin.\n"
							   "Red = X, Green = Y, Blue = Z");
	connect(showAxesAction, SIGNAL(triggered(bool)), this, SLOT(showAxesActionSlot(bool)));

	QAction* showManualTool = new QAction("Show Manual Tool 3D", &contextMenu);
	showManualTool->setCheckable(true);
	showManualTool->setChecked(settings()->value("View3D/showManualTool").toBool());
	showManualTool->setToolTip("Turn on/off visualization of the 3D vire cross");
	connect(showManualTool, SIGNAL(triggered(bool)), this, SLOT(showManualToolSlot(bool)));

	QAction* showOrientation = new QAction("Show Orientation", &contextMenu);
	showOrientation->setCheckable(true);
	showOrientation->setChecked(mAnnotationMarker->getVisible());
	showOrientation->setToolTip("Turn on/off visualization of the figure in the upper left corner in 3D,\n"
								"and the orientation letters on the sides in 2D");
	connect(showOrientation, SIGNAL(triggered(bool)), this, SLOT(showOrientationSlot(bool)));

	QAction* showToolPath = new QAction("Show Tool Path", &contextMenu);
	showToolPath->setCheckable(true);
	showToolPath->setChecked(settings()->value("showToolPath").toBool());
	showToolPath->setToolTip("Paint a line in 3D where the tool have been, as connected dots.\n"
							 "Turn off to reset");
	connect(showToolPath, SIGNAL(triggered(bool)), this, SLOT(showToolPathSlot(bool)));

	QMenu* show3DSlicesMenu = new QMenu("Show 3D slices");
	show3DSlicesMenu->setToolTip("Visualize the 2D views in 3D for the selected image");
	mShow3DSlicesInteractor->addDataActionsOfType<Image>(show3DSlicesMenu);

	QMenu* showSlicesMenu = new QMenu("Slice Type", &contextMenu);
	showSlicesMenu->setToolTip("Specify which 2D slices to show in 3D,\n"
							   "when 3D sclices is turned on");
	this->createSlicesActions(showSlicesMenu);

	QAction* showRefTool = new QAction("Show Reference Tool", &contextMenu);
	showRefTool->setDisabled(true);
	showRefTool->setCheckable(true);
	showRefTool->setToolTip("Visualize the tool set as reference in 3D");
	ToolPtr refTool = mServices->tracking()->getReferenceTool();
	if (refTool)
	{
		showRefTool->setText("Show " + refTool->getName());
		showRefTool->setEnabled(true);
		showRefTool->setChecked(RepContainer(mView->getReps()).findFirst<ToolRep3D>(refTool) ? true : false);
		connect(showRefTool, SIGNAL(toggled(bool)), this, SLOT(showRefToolSlot(bool)));
	}

	contextMenu.addSeparator();
	contextMenu.addMenu(show3DSlicesMenu);
	contextMenu.addMenu(showSlicesMenu);
	contextMenu.addSeparator();
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
	if (slicePlanesAction)
		contextMenu.addAction(slicePlanesAction);
	if (fillSlicePlanesAction)
		contextMenu.addAction(fillSlicePlanesAction);
}

void ViewWrapper3D::createSlicesActions(QWidget* parent)
{
	this->createSlicesAction(PlaneTypeCollection(ptAXIAL, ptCORONAL, ptSAGITTAL), parent);
	this->createSlicesAction(PlaneTypeCollection(ptAXIAL), parent);
	this->createSlicesAction(PlaneTypeCollection(ptCORONAL), parent);
	this->createSlicesAction(PlaneTypeCollection(ptSAGITTAL), parent);
	this->createSlicesAction(PlaneTypeCollection(ptANYPLANE), parent);
	this->createSlicesAction(PlaneTypeCollection(ptINVERSEANYPLANE), parent);
	this->createSlicesAction(PlaneTypeCollection(ptRADIALPLANE), parent);
	this->createSlicesAction(PlaneTypeCollection(ptSIDEPLANE), parent);
	this->createSlicesAction(PlaneTypeCollection(ptTOOLSIDEPLANE), parent);
}

QAction* ViewWrapper3D::createSlicesAction(PlaneTypeCollection planes, QWidget* parent)
{
	QString title = planes.toString();
	QString active = mGroupData->getSliceDefinitions().toString();

	QAction* action = new QAction(title, parent);
	connect(action, SIGNAL(triggered()), this, SLOT(showSlices()));
	action->setData(title);
	action->setCheckable(true);
	action->setChecked(active == title);

	parent->addAction(action);
	return action;
}

void ViewWrapper3D::showSlices()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if (!action)
		return;

	PlaneTypeCollection planes = PlaneTypeCollection::fromString(action->data().toString());

	if (!action->isChecked())
		mGroupData->setSliceDefinitions(PlaneTypeCollection());
	else
		mGroupData->setSliceDefinitions(planes);
}

void ViewWrapper3D::setViewGroup(ViewGroupDataPtr group)
{
	ViewWrapper::setViewGroup(group);

	connect(group.get(), &ViewGroupData::initialized, this, &ViewWrapper3D::resetCameraActionSlot);
	connect(group.get(), &ViewGroupData::optionsChanged, this, &ViewWrapper3D::optionChangedSlot);
	mView->getRenderer()->SetActiveCamera(mGroupData->getCamera3D()->getCamera());

	// Set eye angle after camera change. Maybe create a cameraChangedSlot instead
	this->setStereoEyeAngle(settings()->value("View3D/eyeAngle").toDouble());
	this->optionChangedSlot();

}

void ViewWrapper3D::showToolPathSlot(bool checked)
{
	ToolPtr tool = mServices->tracking()->getActiveTool();
	ToolRep3DPtr activeRep3D = RepContainer(mView->getReps()).findFirst<ToolRep3D>(tool);
	if (activeRep3D)
	{
		if(checked)
		{
			activeRep3D->getTracer()->start();
		}
		else if(!checked)
		{
			activeRep3D->getTracer()->stop();
			activeRep3D->getTracer()->clear();
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
		axis.reset(new AxisConnector(CoordinateSystem(csREF), mServices->spaceProvider()));
		axis->mRep->setAxisLength(0.12);
		axis->mRep->setShowAxesLabels(true);
		axis->mRep->setCaption("ref", Vector3D(1, 0, 0));
		axis->mRep->setFontSize(0.03);
		mAxis.push_back(axis);

		// data spaces
		std::vector<DataPtr> data = mGroupData->getData();
		for (unsigned i = 0; i < data.size(); ++i)
		{
			axis.reset(new AxisConnector(CoordinateSystem(csDATA, data[i]->getUid()), mServices->spaceProvider()));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption(data[i]->getName(), Vector3D(1, 0, 0));
			axis->mRep->setFontSize(0.03);
			mAxis.push_back(axis);
		}

		// tool spaces
		TrackingService::ToolMap tools = mServices->tracking()->getTools();
		TrackingService::ToolMap::iterator iter;
		for (iter = tools.begin(); iter != tools.end(); ++iter)
		{
			ToolPtr tool = iter->second;

			axis.reset(new AxisConnector(CoordinateSystem(csTOOL, tool->getUid()), mServices->spaceProvider()));
			axis->mRep->setAxisLength(0.08);
			axis->mRep->setShowAxesLabels(false);
			axis->mRep->setCaption("t", Vector3D(0.7, 1, 0.7));
			axis->mRep->setFontSize(0.03);
			axis->connectTo(tool);
			SpaceListenerPtr mToolListener = axis->mListener;

			mAxis.push_back(axis);

			axis.reset(new AxisConnector(CoordinateSystem(csSENSOR, tool->getUid()), mServices->spaceProvider()));
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
	settings()->setValue("View3D/showManualTool", visible);
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

NavigationPtr ViewWrapper3D::getNavigation()
{
	CameraControlPtr camera3D(new CameraControl());
	camera3D->setView(mView);

	return NavigationPtr(new Navigation(mServices, camera3D));
}

void ViewWrapper3D::centerImageActionSlot()
{
	NavigationPtr nav = this->getNavigation();
	nav->centerToDataInViewGroup(mGroupData, DataViewProperties::create3D());
}

void ViewWrapper3D::centerToolActionSlot()
{
	NavigationPtr nav = this->getNavigation();
	nav->centerToTooltip();
}

void ViewWrapper3D::showSlicePlanesActionSlot(bool checked)
{
	if (!mSlicePlanes3DRep)
		return;
	mSlicePlanes3DRep->getProxy()->setVisible(checked);
	settings()->setValue("showSlicePlanes", checked);
}

void ViewWrapper3D::fillSlicePlanesActionSlot(bool checked)
{
	if (!mSlicePlanes3DRep)
		return;
	mSlicePlanes3DRep->getProxy()->setDrawPlanes(checked);
}

void ViewWrapper3D::dataViewPropertiesChangedSlot(QString uid)
{
	DataPtr data = mServices->patient()->getData(uid);
	DataViewProperties properties = mGroupData->getProperties(uid);

	if (properties.hasVolume3D())
		this->addVolumeDataRep(data);
	else
		this->removeVolumeDataRep(uid);

	this->updateSlices();

	this->updateView();
}

void ViewWrapper3D::addVolumeDataRep(DataPtr data)
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
			if (rep)
			{
				mDataReps[data->getUid()] = rep;
				mView->addRep(rep);
			}
		}
	}
}

void ViewWrapper3D::removeVolumeDataRep(QString uid)
{
	mMultiVolume3DRepProducer->removeImage(uid);
	if (mDataReps.count(uid))
	{
		mView->removeRep(mDataReps[uid]);
		mDataReps.erase(uid);
	}
}

/**Construct a 3D standard rep for a given data.
 *
 */
RepPtr ViewWrapper3D::createDataRep3D(DataPtr data)
{
	if (boost::dynamic_pointer_cast<Mesh>(data))
	{
		GeometricRepPtr rep = GeometricRep::New();
		rep->setMesh(boost::dynamic_pointer_cast<Mesh>(data));
		return rep;
	}
	else if (boost::dynamic_pointer_cast<TrackedStream>(data))
	{
		TrackedStreamPtr trackedStream = boost::dynamic_pointer_cast<TrackedStream>(data);
		return this->createTrackedStreamRep(trackedStream);
	}
	else
	{
		DataMetricRepPtr rep = this->createDataMetricRep3D(data);
		if (rep)
			return rep;
	}

	return RepPtr();
}

RepPtr ViewWrapper3D::createTrackedStreamRep(TrackedStreamPtr trackedStream)
{
	if(!trackedStream->hasVideo())
	{
		connect(trackedStream.get(), &TrackedStream::streamChanged, this, &ViewWrapper3D::dataViewPropertiesChangedSlot);
		return RepPtr();
	}
	else
		disconnect(trackedStream.get(), &TrackedStream::streamChanged, this, &ViewWrapper3D::dataViewPropertiesChangedSlot);
	if(trackedStream->is3D())
	{
		StreamRep3DPtr rep = StreamRep3D::New(mServices->spaceProvider(), mServices->patient());
		rep->setTrackedStream(trackedStream);
		return rep;
	}
	else if (trackedStream->is2D())
	{
		Stream2DRep3DPtr rep = Stream2DRep3D::New(mServices->spaceProvider());
		rep->setTrackedStream(trackedStream);
		return rep;
	}
	else
	{
		reportWarning("ViewWrapper3D::createDataRep3D. TrackedStream is not 2D or 3D");
		return RepPtr();
	}
}

DataMetricRepPtr ViewWrapper3D::createDataMetricRep3D(DataPtr data)
{
	DataMetricRepPtr rep;

	if (boost::dynamic_pointer_cast<PointMetric>(data))
		rep = PointMetricRep::New();
	else if (boost::dynamic_pointer_cast<FrameMetric>(data))
		rep = FrameMetricRep::New();
	else if (boost::dynamic_pointer_cast<ToolMetric>(data))
		rep = ToolMetricRep::New();
	else if (boost::dynamic_pointer_cast<DistanceMetric>(data))
		rep = DistanceMetricRep::New();
	else if (boost::dynamic_pointer_cast<AngleMetric>(data))
		rep = AngleMetricRep::New();
	else if (boost::dynamic_pointer_cast<PlaneMetric>(data))
		rep = PlaneMetricRep::New();
	else if (boost::dynamic_pointer_cast<DonutMetric>(data))
		rep = DonutMetricRep::New();
	else if (boost::dynamic_pointer_cast<CustomMetric>(data))
		rep = CustomMetricRep::New();
	else if (boost::dynamic_pointer_cast<SphereMetric>(data))
		rep = SphereMetricRep::New();
	else if (boost::dynamic_pointer_cast<RegionOfInterestMetric>(data))
		rep = RegionOfInterestMetricRep::New();

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
	val->setShowAnnotation(!settings()->value("View/showMetricNamesInCorner").toBool());
}

QString ViewWrapper3D::getDataDescription()
{
	return this->getAllDataNames(DataViewProperties::create3D()).join("\n");
}

QString ViewWrapper3D::getViewDescription()
{
	return "3D";
}

void ViewWrapper3D::updateView()
{
	this->ViewWrapper::updateView();

	if (!this->getView())
		return;

	this->updateMetricNamesRep();

	mAnnotationMarker->setVisible(settings()->value("View/showOrientationAnnotation").value<bool>());

	ToolRep3DPtr manualToolRep = RepContainer(mView->getReps()).findManualToolRep<ToolRep3D>();
	if (manualToolRep)
	{
		manualToolRep->setTooltipPointColor(settings()->value("View/toolTipPointColor").value<QColor>());
		manualToolRep->setToolOffsetPointColor(settings()->value("View/toolOffsetPointColor").value<QColor>());
		manualToolRep->setToolOffsetLineColor(settings()->value("View/toolOffsetLineColor").value<QColor>());
	}
}

void ViewWrapper3D::activeImageChangedSlot(QString uid)
{
	if(!mGroupData)
		return;
	ImagePtr image = mServices->patient()->getData<Image>(uid);

	// only show landmarks belonging to image visible in this view:
	std::vector<ImagePtr> images = mGroupData->getImages(DataViewProperties::create3D());
	if (!std::count(images.begin(), images.end(), image))
		image.reset();
}

void ViewWrapper3D::showRefToolSlot(bool checked)
{
	ToolPtr refTool = mServices->tracking()->getReferenceTool();
	if (!refTool)
		return;
	ToolRep3DPtr refRep = RepContainer(mView->getReps()).findFirst<ToolRep3D>(refTool);
	if (!refRep)
	{
		refRep = ToolRep3D::New(mServices->spaceProvider(), refTool->getUid() + "_rep3d_" + this->mView->getUid());
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
	if (mSlices3DRep)
		mView->removeRep(mSlices3DRep);

	if (!mGroupData)
		return;

	std::vector<ImagePtr> images = mGroupData->getImages(DataViewProperties::createSlice3D());
//	std::vector<ImagePtr> images = mGroupData->get3DSliceImages();
	if (images.empty())
		return;

	std::vector<PLANE_TYPE> planes = mGroupData->getSliceDefinitions().get();
	if (planes.empty())
		return;
	mSlices3DRep = Slices3DRep::New(mSharedOpenGLContext, "MultiSliceRep_" + mView->getName());
	for (unsigned i=0; i<planes.size(); ++i)
		mSlices3DRep->addPlane(planes[i], mServices->patient());
	mSlices3DRep->setShaderPath(DataLocations::findConfigFolder("/shaders"));
	mSlices3DRep->setImages(images);
	mSlices3DRep->setTool(mServices->tracking()->getActiveTool());
	mView->addRep(mSlices3DRep);
}

ViewPtr ViewWrapper3D::getView()
{
	return mView;
}

void ViewWrapper3D::activeToolChangedSlot()
{
	ToolPtr controllingTool = this->getControllingTool();
	//CX_LOG_DEBUG() << "ViewWrapper3D::activeToolChangedSlot - controllingTool: " << controllingTool->getName();

	mPickerRep->setTool(controllingTool);
	if (mSlices3DRep)
		mSlices3DRep->setTool(controllingTool);
}

void ViewWrapper3D::toolsAvailableSlot()
{
	std::vector<ToolRep3DPtr> reps = RepContainer::findReps<ToolRep3D>(mView->getReps());

	TrackingService::ToolMap tools = mServices->tracking()->getTools();
	TrackingService::ToolMap::iterator iter;
	for (iter = tools.begin(); iter != tools.end(); ++iter)
	{
		ToolPtr tool = iter->second;
		if (tool->hasType(Tool::TOOL_REFERENCE))
			continue;

		ToolRep3DPtr toolRep = RepContainer(mView->getReps()).findFirst<ToolRep3D>(tool);

		std::vector<ToolRep3DPtr>::iterator oldRep = std::find(reps.begin(), reps.end(), toolRep);
		if (oldRep!=reps.end())
			reps.erase(oldRep);

		if (tool->hasType(Tool::TOOL_MANUAL) && !settings()->value("View3D/showManualTool").toBool())
		{
			if (toolRep)
				mView->removeRep(toolRep);
			continue;
		}

		if (!toolRep)
		{
			toolRep = ToolRep3D::New(mServices->spaceProvider(), tool->getUid() + "_rep3d_" + this->mView->getUid());
			if (settings()->value("showToolPath").toBool())
				toolRep->getTracer()->start();
		}

		toolRep->setSphereRadius(settings()->value("View3D/sphereRadius").toDouble()); // use fraction of set size
		toolRep->setSphereRadiusInNormalizedViewport(true);

		toolRep->setTool(tool);
		toolRep->setOffsetPointVisibleAtZeroOffset(true);
		mView->addRep(toolRep);
	}

	// remove reps for tools no longer present
	for (unsigned i=0; i<reps.size(); ++i)
	{
		mView->removeRep(reps[i]);
	}
}

void ViewWrapper3D::optionChangedSlot()
{
	ViewGroupData::Options options = mGroupData->getOptions();

	this->showLandmarks(options.mShowLandmarks);
	this->showPointPickerProbe(options.mShowPointPickerProbe);
	mPickerRep->setGlyph(options.mPickerGlyph);

	this->updateSlices();
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

void ViewWrapper3D::setStereoEyeAngle(double angle)
{
	mView->getRenderer()->GetActiveCamera()->SetEyeAngle(angle);
}

void ViewWrapper3D::setTranslucentRenderingToDepthPeeling(bool setDepthPeeling)
{
	if(setDepthPeeling)
	{
		bool isDPSupported = true;

		//IsDepthPeelingSupported function don't seem to work on OSX (error messages or seg. fault)
#ifndef __APPLE__
		if (!IsDepthPeelingSupported(mView->getRenderWindow(), mView->getRenderer(), true))
		{
			reportWarning("GPU do not support depth peeling. Rendering of translucent surfaces is not supported");
			isDPSupported = false;
		}
#endif

		if (isDPSupported && SetupEnvironmentForDepthPeeling(mView->getRenderWindow(), mView->getRenderer(), 100, 0.1))
		{
			report("Set GPU depth peeling");
		}
		else
		{
			reportWarning("Error setting depth peeling. The GPU or operating system might not support it.");
			settings()->setValue("View3D/depthPeeling", false);
		}
	}
	else
	{
		TurnOffDepthPeeling(mView->getRenderWindow(), mView->getRenderer());
	}
}

//------------------------------------------------------------------------------
}
