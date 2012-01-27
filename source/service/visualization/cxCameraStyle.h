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

class QWidget;
class QMenu;

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

/**
 * \class CameraStyle
 *
 * Controls the current camera style of the 3d view.
 * Refactored from class View3D.
 *
 * \date Dec 9, 2008
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Christian Askeland, SINTEF
 */
class CameraStyle: public QObject
{
Q_OBJECT
public:
	enum Style
	{
		DEFAULT_STYLE, TOOL_STYLE, ANGLED_TOOL_STYLE
	};

	CameraStyle();

	void setCameraStyle(Style style, int offset = -1); ///< lets the user select where the camera, offset only used by TOOL_STYLE

public slots:
	void setCameraOffsetSlot(int offset); ///< sets the camera offset

protected slots:
	void moveCameraToolStyleSlot(Transform3D prMt, double timestamp); ///< receives transforms from the tool which the camera should follow
	void dominantToolChangedSlot();
	void viewChangedSlot();

protected:
	View3D* getView() const;
	vtkRendererPtr getRenderer() const;
	vtkCameraPtr getCamera() const;
	ssc::ToolRep3DPtr getToolRep() const;

	void activateCameraDefaultStyle();
	void activateCameraToolStyle(int offset = 0);
	void activateCameraAngledToolStyle(int offset = 0);
	void connectTool();
	void disconnectTool();

	Style mCameraStyle; ///< the current camerastyle
	int mCameraOffset; ///< the distance between the camera and focalpoint
	ssc::ToolPtr mFollowingTool; ///< the tool the camera is following
};

/**
 * @}
 */
} //namespace cx

#endif /* CXCAMERASTYLE_H_ */
