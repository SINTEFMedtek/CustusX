/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCameraStyleForViewFORVIEW_H
#define CXCameraStyleForViewFORVIEW_H

#include "org_custusx_core_view_Export.h"

#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxViewService.h"
#include "cxViewGroupData.h"
#include "cxRegionOfInterestMetric.h"
#include "cxDoubleRange.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;
class vtkInteractorStyle;

namespace cx
{
typedef boost::shared_ptr<class ViewportPreRenderListener> ViewportPreRenderListenerPtr;
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;

typedef boost::shared_ptr<class CameraStyleForView> CameraStyleForViewPtr;
using cx::Transform3D;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */

/**
 * Reduce jitter during zooming.
 *
 * Input a suggested zoom value, output the smoothed value.
 *  - Maintain a interval where the value can move inside without
 *    changing the smoothed value.
 *  - If the value moves outside interval, the interval will move
 *    along, and the smoothed value will be set to the input value.
 */
class JitterFilter
{
public:
	JitterFilter();
	double newValue(double value);
private:
	DoubleRange range;
	double currentValue;
};

struct CameraInfo
{
	CameraInfo() {}
	explicit CameraInfo(vtkCameraPtr camera);
	double distance() const { return (pos-focus).length(); }
	Vector3D vpn() const { return (pos-focus).normal(); }

	Vector3D pos;
	Vector3D focus;
	Vector3D vup;
	double viewAngle; // vtk View angle in DEGREES
};

bool similar(const CameraInfo& lhs, const CameraInfo& rhs, double tol=1.0E-6);

/**
 * \class CameraStyleForView
 *
 * Controls the current camera style of the 3d view.
 * Refactored from class View3D.
 * Refactored from class CameraStyle.
 *
 * \date Dec 9, 2008
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_view_EXPORT CameraStyleForView: public QObject
{
Q_OBJECT
public:
	explicit CameraStyleForView(CoreServicesPtr backend);
	void setView(ViewPtr widget);

	/** Select tool style. This replaces the vtkInteractor Style.
	  */
	void setCameraStyle(CameraStyleData style);
	CameraStyleData getCameraStyle();

private slots:
	void setModified();
	void activeToolChangedSlot();

private:
	ViewPtr getView() const;
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	ToolRep3DPtr getToolRep() const;
	bool isToolFollowingStyle() const;
	void onPreRender();
	void applyCameraStyle(); ///< receives transforms from the tool which the camera should follow

	void connectTool();
	void disconnectTool();
	void viewportChangedSlot();
	RegionOfInterest getROI(QString uid) const;
	void setInteractor(vtkSmartPointer<vtkInteractorStyle> style);

	CameraStyleData mStyle; ///< the current CameraStyleForView
	ToolPtr mFollowingTool; ///< the tool the camera is following
	ViewportListenerPtr mViewportListener;
	ViewportPreRenderListenerPtr mPreRenderListener;
	bool mBlockCameraUpdate; ///< for breaking a camera update loop
	ViewPtr mView;
	CoreServicesPtr mBackend;

//	Vector3D mPreviousZoomCameraPos;
	JitterFilter mZoomJitterFilter;

	Vector3D smoothZoomedCameraPosition(Vector3D pos);
	void handleLights();
	CameraInfo viewEntireAutoZoomROI(CameraInfo info);
	void updateCamera(CameraInfo info);
	Vector3D getToolTip_r();
	Transform3D get_rMto();
};

/**
 * @}
 */
} //namespace cx

#endif // CXCameraStyleForViewFORVIEW_H
