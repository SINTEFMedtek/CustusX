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
 * cxViewWrapper3D.h
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */
#ifndef CXVIEWWRAPPER3D_H_
#define CXVIEWWRAPPER3D_H_

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
 * \addtogroup cx_service_visualization
 * @{
 */

typedef boost::shared_ptr<class ToolAxisConnector> ToolAxisConnectorPtr;

enum STEREOTYPE
{
	stFRAME_SEQUENTIAL, stINTERLACED, stDRESDEN, stRED_BLUE
};


/** Wrapper for a View3D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper3D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper3D(int startIndex, ViewWidget* view, VisualizationServiceBackendPtr backend);
	virtual ~ViewWrapper3D();
	virtual ViewWidget* getView();
	virtual double getZoom2D() { return -1.0; }
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);
	void setStereoType(int type);

protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid);
private slots:
	void showSlices();
	void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void toolsAvailableSlot(); ///< add all tools when configured
	void showSlicePlanesActionSlot(bool checked);
	void fillSlicePlanesActionSlot(bool checked);
	void showAxesActionSlot(bool checked);
	void showManualToolSlot(bool visible);
	void resetCameraActionSlot();
	void activeImageChangedSlot();
	void showRefToolSlot(bool checked);
	void showToolPathSlot(bool checked);
	void PickerRepPointPickedSlot(Vector3D p_r);
	void centerImageActionSlot();
	void centerToolActionSlot();
	void optionChangedSlot();
	void showOrientationSlot(bool visible);
	void globalConfigurationFileChangedSlot(QString key);
	void setStereoEyeAngle(double angle);
	void settingsChangedSlot(QString key);
	void PickerRepDataPickedSlot(QString);
	void updateView();

private:
	virtual void appendToContextMenu(QMenu& contextMenu);
	void readDataRepSettings(RepPtr rep);
	void updateSlices();

	QAction* createSlicesAction(QString title, QWidget* parent);

	void createSlicesActions(QWidget *parent);
	QAction* createSlicesAction(PlaneTypeCollection planes, QWidget* parent);

	void showLandmarks(bool on);
	void showPointPickerProbe(bool on);
	void setOrientationAnnotation();

	RepPtr createDataRep3D(DataPtr data);
    DataMetricRepPtr createDataMetricRep3D(DataPtr data);
//    virtual void dataAdded(DataPtr data);
//	virtual void dataRemoved(const QString& uid);

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
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
//	DisplayTextRepPtr mMetricsText;
	MetricNamesRepPtr mMetricNames;
//	QString mShowSlicesMode;
	std::vector<AxisConnectorPtr> mAxis;

	bool mShowAxes; ///< show 3D axes reps for all tools and ref space
	Slices3DRepPtr mSlices3DRep;
	SlicePlanes3DRepPtr mSlicePlanes3DRep;
	OrientationAnnotation3DRepPtr mAnnotationMarker;

	QPointer<ViewWidget> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
