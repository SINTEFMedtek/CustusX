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
 * cxCameraStyle.h
 *
 *  \date Mar 9, 2011
 *      \author dev
 */
#ifndef CXCAMERASTYLE_H_
#define CXCAMERASTYLE_H_

#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscViewportListener.h"
#include "sscEnumConverter.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;

namespace cx
{

class View3D;

typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
using ssc::Transform3D;

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
 * \class CameraStyle
 *
 * Controls the current camera style of the 3d view.
 * Refactored from class View3D.
 *
 * \date Dec 9, 2008
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class CameraStyle: public QObject
{
Q_OBJECT
public:
	CameraStyle();

	/** Select tool style. This replaces the vtkInteractor Style.
	  *
	  */
	void setCameraStyle(CAMERA_STYLE_TYPE style);
	QActionGroup* createInteractorStyleActionGroup();

private slots:
	void moveCameraToolStyleSlot(Transform3D prMt, double timestamp); ///< receives transforms from the tool which the camera should follow
	void dominantToolChangedSlot();
	void viewChangedSlot();
	void setInteractionStyleActionSlot();

private:
	View3D* getView() const;
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	ssc::ToolRep3DPtr getToolRep() const;
	bool isToolFollowingStyle(CAMERA_STYLE_TYPE style) const;

	void connectTool();
	void disconnectTool();
	void viewportChangedSlot();
	void updateCamera();
	void updateActionGroup();

	void addInteractorStyleAction(QString caption, QActionGroup* group, QString className, QIcon icon,
					QString helptext);

	CAMERA_STYLE_TYPE mCameraStyle; ///< the current camerastyle
	ssc::ToolPtr mFollowingTool; ///< the tool the camera is following
	ssc::ViewportListenerPtr mViewportListener;
	bool mBlockCameraUpdate; ///< for breaking a camera update loop
	QActionGroup* mCameraStyleGroup;
};

/**
 * @}
 */
} //namespace cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cx, CAMERA_STYLE_TYPE);

#endif /* CXCAMERASTYLE_H_ */
