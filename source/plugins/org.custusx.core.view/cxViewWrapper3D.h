/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxViewWrapper3D.h
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */
#ifndef CXVIEWWRAPPER3D_H_
#define CXVIEWWRAPPER3D_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QPointer>
#include <QObject>
#include "cxDefinitions.h"
#include "cxViewWrapper.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxCoordinateSystemHelpers.h"

class QAction;
typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;

namespace cx
{
typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
typedef boost::shared_ptr<class DataMetricRep> DataMetricRepPtr;
typedef boost::shared_ptr<class MetricNamesRep> MetricNamesRepPtr;

}

namespace cx
{
typedef boost::shared_ptr<class ImageLandmarkRep> ImageLandmarkRepPtr;
typedef boost::shared_ptr<class PatientLandmarkRep> PatientLandmarkRepPtr;
typedef boost::shared_ptr<class MultiVolume3DRepProducer> MultiVolume3DRepProducerPtr;
typedef boost::shared_ptr<class AxisConnector> AxisConnectorPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

typedef boost::shared_ptr<class ToolAxisConnector> ToolAxisConnectorPtr;


/** Wrapper for a View3D.
 *  Handles the connections between specific reps and the view.
 *
 */
class org_custusx_core_view_EXPORT ViewWrapper3D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper3D(int startIndex, ViewPtr view, VisServicesPtr services);
	virtual ~ViewWrapper3D();
	virtual ViewPtr getView();
	virtual double getZoom2D() { return -1.0; }
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);
	void setStereoType(int type);

protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid);
	virtual void settingsChangedSlot(QString key);
private slots:
	void showSlices();
	void activeToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void toolsAvailableSlot(); ///< add all tools when configured
	void showSlicePlanesActionSlot(bool checked);
	void fillSlicePlanesActionSlot(bool checked);
	void showAxesActionSlot(bool checked);
	void showManualToolSlot(bool visible);
	void resetCameraActionSlot();
	void activeImageChangedSlot(QString uid);
	void showRefToolSlot(bool checked);
	void showToolPathSlot(bool checked);
	void pickerRepPointPickedSlot(Vector3D p_r);
	void centerImageActionSlot();
	void centerToolActionSlot();
	void optionChangedSlot();
	void showOrientationSlot(bool visible);
	void setStereoEyeAngle(double angle);
	void pickerRepDataPickedSlot(QString);
	void updateView();

protected:
	virtual QString getDataDescription();
	virtual QString getViewDescription();

private:
	virtual void appendToContextMenu(QMenu& contextMenu);
	void readDataRepSettings(RepPtr rep);
	void updateSlices();
	NavigationPtr getNavigation();

	QAction* createSlicesAction(QString title, QWidget* parent);

	void createSlicesActions(QWidget *parent);
	QAction* createSlicesAction(PlaneTypeCollection planes, QWidget* parent);

	void showLandmarks(bool on);
	void showPointPickerProbe(bool on);
	void setOrientationAnnotation();

	RepPtr createDataRep3D(DataPtr data);
    DataMetricRepPtr createDataMetricRep3D(DataPtr data);

	void addVolumeDataRep(DataPtr data);
	void removeVolumeDataRep(QString uid);

	void setTranslucentRenderingToDepthPeeling(bool setDepthPeeling);
	void initializeMultiVolume3DRepProducer();
	void updateMetricNamesRep();

	MultiVolume3DRepProducerPtr mMultiVolume3DRepProducer;
	typedef std::map<QString, RepPtr> RepMap;
	RepMap mDataReps;
	LandmarkRepPtr mLandmarkRep;
	PickerRepPtr mPickerRep;
	MetricNamesRepPtr mMetricNames;
	std::vector<AxisConnectorPtr> mAxis;

	bool mShowAxes; ///< show 3D axes reps for all tools and ref space
	Slices3DRepPtr mSlices3DRep;
	SlicePlanes3DRepPtr mSlicePlanes3DRep;
	OrientationAnnotation3DRepPtr mAnnotationMarker;

	ViewPtr mView;
	RepPtr createTrackedStreamRep(TrackedStreamPtr trackedStream);
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
