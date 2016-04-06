/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXCameraStyleForViewFORVIEW_H
#define CXCameraStyleForViewFORVIEW_H

#include "org_custusx_core_view_Export.h"

#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxEnumConverter.h"
#include "cxViewService.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;

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
	void setCameraStyle(CAMERA_STYLE_TYPE style);

private slots:
	void setModified();
	void activeToolChangedSlot();

private:
	ViewPtr getView() const;
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

	ViewPtr mView;
	CoreServicesPtr mBackend;
};

/**
 * @}
 */
} //namespace cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, CAMERA_STYLE_TYPE);

#endif // CXCameraStyleForViewFORVIEW_H
