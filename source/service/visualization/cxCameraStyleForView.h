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
#ifndef CXCameraStyleForViewFORVIEW_H
#define CXCameraStyleForViewFORVIEW_H

#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"
#include "sscEnumConverter.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;

namespace cx
{
typedef boost::shared_ptr<class ViewportPreRenderListener> ViewportPreRenderListenerPtr;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;

typedef boost::shared_ptr<class CameraStyleForView> CameraStyleForViewPtr;
using cx::Transform3D;

/**
 * \file
 * \addtogroup cxServiceVisualization
 * @{
 */


enum CAMERA_STYLE_TYPE
{
	cstDEFAULT_STYLE, cstTOOL_STYLE, cstANGLED_TOOL_STYLE, cstUNICAM_STYLE, cstCOUNT
};


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
class CameraStyleForView: public QObject
{
Q_OBJECT
public:
	explicit CameraStyleForView(VisualizationServiceBackendPtr backend);
	void setView(ViewWidgetQPtr widget);

	/** Select tool style. This replaces the vtkInteractor Style.
	  */
	void setCameraStyle(CAMERA_STYLE_TYPE style);

private slots:
	void setModified();
	void dominantToolChangedSlot();

private:
	ViewWidgetQPtr getView() const;
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	ToolRep3DPtr getToolRep() const;
	bool isToolFollowingStyle(CAMERA_STYLE_TYPE style) const;
	void onPreRender();
	void moveCameraToolStyleSlot(Transform3D prMt, double timestamp); ///< receives transforms from the tool which the camera should follow

	void connectTool();
	void disconnectTool();
	void viewportChangedSlot();
	void updateCamera();

	CAMERA_STYLE_TYPE mCameraStyleForView; ///< the current CameraStyleForView
	ToolPtr mFollowingTool; ///< the tool the camera is following
	ViewportListenerPtr mViewportListener;
	ViewportPreRenderListenerPtr mPreRenderListener;
	bool mBlockCameraUpdate; ///< for breaking a camera update loop

	ViewWidgetQPtr mView;
	VisualizationServiceBackendPtr mBackend;
};

/**
 * @}
 */
} //namespace cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, CAMERA_STYLE_TYPE);

#endif // CXCameraStyleForViewFORVIEW_H
