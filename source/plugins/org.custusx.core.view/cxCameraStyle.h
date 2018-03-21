/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * cxCameraStyle.h
 *
 *  \date Mar 9, 2011
 *      \author dev
 */
#ifndef CXCAMERASTYLE_H_
#define CXCAMERASTYLE_H_

#include "org_custusx_core_view_Export.h"

#include <QPointer>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxEnumConverter.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;
#include "cxCameraStyleForView.h"

namespace cx
{

typedef boost::shared_ptr<class CameraStyle> CameraStylePtr;
using cx::Transform3D;
typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
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
class org_custusx_core_view_EXPORT CameraStyle: public QObject
{
Q_OBJECT
public:
	explicit CameraStyle(CoreServicesPtr backend, ViewGroupDataPtr viewGroupData);
	void setCameraStyle(CameraStyleData style); ///< Select tool style. This replaces the vtkInteractor Style.
	CameraStyleData getCameraStyle() const;

	void addView(ViewPtr view);
	void clearViews();

signals:
	void cameraStyleChanged();
private:
	CameraStyleData mCameraStyle; ///< the current camerastyle
	std::vector<CameraStyleForViewPtr> mViews;
	CoreServicesPtr mBackend;
	ViewGroupDataPtr mViewGroupData;
};


/**
 * @}
 */
} //namespace cx


#endif /* CXCAMERASTYLE_H_ */
