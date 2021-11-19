/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxViewWrapper2D.h
 *
 *  \date Mar 24, 2010
 *      \author christiana
 */
#ifndef CXVIEWWRAPPER2D_H_
#define CXVIEWWRAPPER2D_H_

#include "org_custusx_core_view_Export.h"

#include <vector>
#include <QPointer>
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"
#include "cxViewWrapper.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "sscConfig.h"

class QMouseEvent;
class QWheelEvent;

namespace cx
{
typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
typedef boost::shared_ptr<class ViewFollower> ViewFollowerPtr;
typedef boost::shared_ptr<class Zoom2DHandler> Zoom2DHandlerPtr;
typedef boost::shared_ptr<class DataRepContainer> DataRepContainerPtr;
}


namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/** Wrapper for a View2D.
 *  Handles the connections between specific reps and the view.
 *
 */
class org_custusx_core_view_EXPORT ViewWrapper2D: public ViewWrapper
{
Q_OBJECT
public:
	ViewWrapper2D(ViewPtr view, VisServicesPtr backend);
	virtual ~ViewWrapper2D();
	virtual void initializePlane(PLANE_TYPE plane);
	virtual ViewPtr getView();
	virtual void setSlicePlanesProxy(SlicePlanesProxyPtr proxy);
	virtual void setViewGroup(ViewGroupDataPtr group);

	virtual void updateView();

	ImagePtr getImageToDisplay();

signals:
	void pointSampled(Vector3D p_r);

protected:
	virtual QString getDataDescription();
	virtual QString getViewDescription();

protected slots:
	virtual void dataViewPropertiesChangedSlot(QString uid);
	virtual void videoSourcesChangedSlot();
	virtual void settingsChangedSlot(QString key);

private slots:
	void activeToolChangedSlot(); ///< makes sure the reps are connected to the right tool
	void viewportChanged();
	void showSlot();
	void mousePressSlot(int x, int y, Qt::MouseButtons buttons);
	void mouseMoveSlot(int x, int y, Qt::MouseButtons buttons);
	void mouseWheelSlot(int x, int y, int delta, int orientation, Qt::MouseButtons buttons);
	void optionChangedSlot();
	void showManualToolSlot(bool visible);
	void toggleShowManualTool();

protected slots:
	void samplePoint(Vector3D click_vp);
private:
	void moveManualTool(Vector3D vp, Vector3D delta_vp);
	virtual void appendToContextMenu(QMenu& contextMenu);
	void addReps();
	DoubleBoundingBox3D getViewport() const;
	Vector3D qvp2vp(QPoint pos_qvp);
	void setAxisPos(Vector3D click_vp);
	void shiftAxisPos(Vector3D delta_vp);

	ORIENTATION_TYPE getOrientationType() const;

	virtual void imageAdded(ImagePtr image);
	//virtual void imageRemoved(const QString& uid);

	virtual void dataAdded(DataPtr data);
	virtual void dataRemoved(const QString& uid);

	void recreateMultiSlicer();
	void updateItemsFromViewGroup();

	void createAndAddSliceRep();
	void removeAndResetSliceRep();

	bool useGPU2DRendering();
	bool createAndAddMultiSliceRep();
	void removeAndResetMultiSliceRep();

	std::vector<ImagePtr> getImagesToView();
	bool isAnyplane();

	Texture3DSlicerRepPtr mMultiSliceRep;

	DataRepContainerPtr mDataRepContainer;

	GeometricRep2DPtr mPickerGlyphRep;
	SliceProxyPtr mSliceProxy;
	SliceRepSWPtr mSliceRep;
	ToolRep2DPtr mToolRep2D;
	OrientationAnnotationSmartRepPtr mOrientationAnnotationRep;
	SlicePlanes3DMarkerIn2DRepPtr mSlicePlanes3DMarker;
	ViewPtr mView;
	ViewFollowerPtr mViewFollower;

	// synchronized data
	Zoom2DHandlerPtr mZoom2D;

	Vector3D mLastClickPos_vp;

	QActionGroup* mOrientationActionGroup;
	//	void sendROItoFollower();
	void changeZoom(double delta);
	void applyViewFollower();
	DoubleBoundingBox3D getViewport_s() const;
};
typedef boost::shared_ptr<ViewWrapper2D> ViewWrapper2DPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXVIEWWRAPPER2D_H_ */
