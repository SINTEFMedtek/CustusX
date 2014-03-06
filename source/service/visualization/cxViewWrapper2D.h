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

namespace cx
{
typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
typedef boost::shared_ptr<class ViewFollower> ViewFollowerPtr;
typedef boost::shared_ptr<class Zoom2DHandler> Zoom2DHandlerPtr;
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
	ViewWrapper2D(ViewWidget* view, VisualizationServiceBackendPtr backend);
	virtual ~ViewWrapper2D();
	virtual void initializePlane(PLANE_TYPE plane);
	virtual ViewWidget* getView();
//	virtual void setZoom2D(SyncedValuePtr value);
	virtual void setOrientationMode(SyncedValuePtr value);
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);

	static bool overlayIsEnabled();
	virtual void updateView();

protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid);
private slots:
	void dominantToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void viewportChanged();
	void showSlot();
	void mousePressSlot(QMouseEvent* event);
	void mouseMoveSlot(QMouseEvent* event);
	void mouseWheelSlot(QWheelEvent* event);
	void orientationActionSlot();
//	void global2DZoomActionSlot();
	void orientationModeChanged();
	void settingsChangedSlot(QString key);
	void optionChangedSlot();
//	void zoom2DActionSlot();

private:
	void moveManualTool(QPoint point);
//	virtual void setZoomFactor2D(double zoomFactor);
//	virtual double getZoomFactor2D() const;
//	void set2DZoomConnectivityFromType(QString type);
//	QString get2DZoomConnectivityType();
//	void add2DZoomConnectivityAction(QString type, QString text, QMenu &contextMenu);

	virtual void appendToContextMenu(QMenu& contextMenu);
	void addReps();
	DoubleBoundingBox3D getViewport() const;
	Transform3D get_vpMs() const;
	Vector3D qvp2vp(QPoint pos_qvp);
	void setAxisPos(Vector3D click_vp);
	void shiftAxisPos(Vector3D delta_vp);

	ORIENTATION_TYPE getOrientationType() const;
	void changeOrientationType(ORIENTATION_TYPE type);

	Vector3D displayToWorld(Vector3D p_d) const;
	Vector3D viewToDisplay(Vector3D p_v) const;

	virtual void imageAdded(ImagePtr image);
	virtual void meshAdded(MeshPtr mesh);
	virtual void imageRemoved(const QString& uid);
	virtual void meshRemoved(const QString& uid);
	virtual void pointMetricAdded(PointMetricPtr mesh);
	virtual void pointMetricRemoved(const QString& uid);

	virtual void dataAdded(DataPtr data);
	virtual void dataRemoved(const QString& uid);

	void resetMultiSlicer();
	Texture3DSlicerRepPtr mMultiSliceRep;

	GeometricRep2DPtr mPickerGlyphRep;
	SliceProxyPtr mSliceProxy;
	SliceRepSWPtr mSliceRep;
	std::map<QString, GeometricRep2DPtr> mGeometricRep;
	std::map<QString, PointMetricRep2DPtr> mPointMetricRep;
	ToolRep2DPtr mToolRep2D;
//  OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	DisplayTextRepPtr mPlaneTypeText;
	DisplayTextRepPtr mDataNameText;
	SlicePlanes3DMarkerIn2DRepPtr mSlicePlanes3DMarker;
	QPointer<ViewWidget> mView;
	ViewFollowerPtr mViewFollower;

	// synchronized data
//	SyncedValuePtr mZoom2D;
	Zoom2DHandlerPtr mZoom2D;

	SyncedValuePtr mOrientationMode;
	Vector3D mClickPos;

	QActionGroup* mOrientationActionGroup;
//	QActionGroup* m2DZoomConnectivityActionGroup;

};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
