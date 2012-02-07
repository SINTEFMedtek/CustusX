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
//#include "sscData.h"
#include "sscDefinitions.h"
//#include "sscAxesRep.h"
//#include "cxViewGroup.h"
#include "cxViewWrapper.h"
#include "cxForwardDeclarations.h"
#include "sscVector3D.h"
class QAction;
typedef vtkSmartPointer<class vtkAnnotatedCubeActor> vtkAnnotatedCubeActorPtr;
typedef vtkSmartPointer<class vtkOrientationMarkerWidget> vtkOrientationMarkerWidgetPtr;

namespace ssc
{
typedef boost::shared_ptr<class Slices3DRep> Slices3DRepPtr;
}

namespace cx
{
typedef boost::shared_ptr<class ImageLandmarkRep> ImageLandmarkRepPtr;
typedef boost::shared_ptr<class PatientLandmarkRep> PatientLandmarkRepPtr;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

typedef boost::shared_ptr<class ToolAxisConnector> ToolAxisConnectorPtr;

enum STEREOTYPE
{
	stFRAME_SEQUENTIAL, stINTERLACED, stDRESDEN, stRED_BLUE
};

class ToolAxisConnector: public QObject
{
Q_OBJECT
public:
	explicit ToolAxisConnector(ssc::ToolPtr tool);
	ssc::AxesRepPtr getAxis_t();
	ssc::AxesRepPtr getAxis_s();

private slots:
	void transformChangedSlot();
	void visibleSlot();
private:
	ssc::ToolPtr mTool;

	ssc::AxesRepPtr mAxis_t; ///< axis of the tool space
	ssc::AxesRepPtr mAxis_s; /// axis of the tool sensor space
};

/** Wrapper for a View3D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper3D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper3D(int startIndex, ssc::View* view);
	virtual ~ViewWrapper3D();
	virtual ssc::View* getView();
	virtual double getZoom2D() { return -1.0; }
	virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);
	void setStereoType(int type);

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
	void PickerRepPointPickedSlot(ssc::Vector3D p_r);
	void centerImageActionSlot();
	void centerToolActionSlot();
	void optionChangedSlot();
	void showOrientationSlot(bool visible);
	void globalConfigurationFileChangedSlot(QString key);
	void setStereoEyeAngle(double angle);
	void settingsChangedSlot(QString key);

private:
	virtual void appendToContextMenu(QMenu& contextMenu);
	void updateView();
	void readDataRepSettings(ssc::RepPtr rep);
	void updateSlices();

	QAction* createSlicesAction(QString title, QWidget* parent);

	void showLandmarks(bool on);
	void showPointPickerProbe(bool on);
	void setOrientationAnnotation();

	ssc::RepPtr createDataRep3D(ssc::DataPtr data);
	virtual void dataAdded(ssc::DataPtr data);
	virtual void dataRemoved(const QString& uid);

	typedef std::map<QString, ssc::RepPtr> RepMap;
	RepMap mDataReps;
	LandmarkRepPtr mLandmarkRep;
	ssc::PickerRepPtr mPickerRep;
	ssc::DisplayTextRepPtr mPlaneTypeText;
	ssc::DisplayTextRepPtr mDataNameText;
	std::map<QString, ToolAxisConnectorPtr> mToolAxis;
	ssc::AxesRepPtr mRefSpaceAxisRep;
	std::map<QString, ssc::AxesRepPtr> mDataSpaceAxisRep;
	QString mShowSlicesMode;

	bool mShowAxes; ///< show 3D axes reps for all tools and ref space
//  ssc::SliceProxyPtr mSliceProxy;
	ssc::Slices3DRepPtr mSlices3DRep;
	ssc::SlicePlanes3DRepPtr mSlicePlanes3DRep;
	ssc::OrientationAnnotation3DRepPtr mAnnotationMarker;

	QPointer<ssc::View> mView;
};
typedef boost::shared_ptr<ViewWrapper3D> ViewWrapper3DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER3D_H_ */
