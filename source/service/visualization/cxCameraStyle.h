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
#include "sscEnumConverter.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;
#include "cxCameraStyleForView.h"

namespace cx
{

typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
using cx::Transform3D;
typedef boost::shared_ptr<class VisualizationServiceBackend> VisualizationServiceBackendPtr;

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
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class CameraStyle: public QObject
{
Q_OBJECT
public:
	explicit CameraStyle(VisualizationServiceBackendPtr backend);

	/** Select tool style. This replaces the vtkInteractor Style.
	  */
	void setCameraStyle(CAMERA_STYLE_TYPE style);
	QActionGroup* createInteractorStyleActionGroup();

private slots:
	void viewChangedSlot();
	void setInteractionStyleActionSlot();

private:
	void updateActionGroup();
	void addInteractorStyleAction(QString caption, QActionGroup* group, QString className, QIcon icon,
					QString helptext);

	CAMERA_STYLE_TYPE mCameraStyle; ///< the current camerastyle
	QActionGroup* mCameraStyleGroup;
	std::vector<CameraStyleForViewPtr> mViews;
	VisualizationServiceBackendPtr mBackend;
};

/**
 * @}
 */
} //namespace cx


#endif /* CXCAMERASTYLE_H_ */
