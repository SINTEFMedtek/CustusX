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
 * cxViewWrapper2D.h
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */
#ifndef CXVIEWWRAPPER2D_H_
#define CXVIEWWRAPPER2D_H_

#include <vector>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "sscDefinitions.h"
#include "cxViewWrapper.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
#include "sscConfig.h"

class QMouseEvent;
class QWheelEvent;

namespace ssc
{
typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
}


namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */

/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class ViewWrapper2D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper2D(ssc::ViewWidget* view);
	virtual ~ViewWrapper2D();
	virtual void initializePlane(ssc::PLANE_TYPE plane);
	virtual ssc::ViewWidget* getView();
	virtual void setZoom2D(SyncedValuePtr value);
	virtual void setOrientationMode(SyncedValuePtr value);
	virtual void setSlicePlanesProxy(ssc::SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);

	static bool overlayIsEnabled();
	virtual void updateView();

private slots:
	void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void viewportChanged();
	void showSlot();
	void mousePressSlot(QMouseEvent* event);
	void mouseMoveSlot(QMouseEvent* event);
	void mouseWheelSlot(QWheelEvent* event);
	void orientationActionSlot();
	void global2DZoomActionSlot();
	void orientationModeChanged();
	void settingsChangedSlot(QString key);
	void optionChangedSlot();

private:
	virtual void setZoomFactor2D(double zoomFactor);
	virtual double getZoomFactor2D() const;
	void moveManualTool(QPoint point);

	virtual void appendToContextMenu(QMenu& contextMenu);
	void addReps();
	ssc::DoubleBoundingBox3D getViewport() const;
	ssc::Transform3D get_vpMs() const;
	ssc::Vector3D qvp2vp(QPoint pos_qvp);
	void setAxisPos(ssc::Vector3D click_vp);
	void shiftAxisPos(ssc::Vector3D delta_vp);

	ssc::ORIENTATION_TYPE getOrientationType() const;
	void changeOrientationType(ssc::ORIENTATION_TYPE type);

	ssc::Vector3D displayToWorld(ssc::Vector3D p_d) const;
	ssc::Vector3D viewToDisplay(ssc::Vector3D p_v) const;

	virtual void imageAdded(ssc::ImagePtr image);
	virtual void meshAdded(ssc::MeshPtr mesh);
	virtual void imageRemoved(const QString& uid);
	virtual void meshRemoved(const QString& uid);
	virtual void pointMetricAdded(ssc::PointMetricPtr mesh);
	virtual void pointMetricRemoved(const QString& uid);

	virtual void dataAdded(ssc::DataPtr data);
	virtual void dataRemoved(const QString& uid);

	void resetMultiSlicer();
	ssc::Texture3DSlicerRepPtr mMultiSliceRep;

	ssc::GeometricRep2DPtr mPickerGlyphRep;
	ssc::SliceProxyPtr mSliceProxy;
	ssc::SliceRepSWPtr mSliceRep;
	std::map<QString, ssc::GeometricRep2DPtr> mGeometricRep;
	std::map<QString, ssc::PointMetricRep2DPtr> mPointMetricRep;
	ssc::ToolRep2DPtr mToolRep2D;
//  ssc::OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	ssc::OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	ssc::DisplayTextRepPtr mPlaneTypeText;
	ssc::DisplayTextRepPtr mDataNameText;
	ssc::SlicePlanes3DMarkerIn2DRepPtr mSlicePlanes3DMarker;
	QPointer<ssc::ViewWidget> mView;

	// sunchronized data
	SyncedValuePtr mZoom2D;
	SyncedValuePtr mOrientationMode;
	ssc::Vector3D mClickPos;

	QActionGroup* mOrientationActionGroup;

};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
