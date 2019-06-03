#ifndef CXCAMERASTYLEINTERACTOR_H
#define CXCAMERASTYLEINTERACTOR_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxResourceVisualizationExport.h"

#include <QPointer>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
class QIcon;
class QWidget;
class QMenu;
class QActionGroup;

namespace cx
{

typedef boost::shared_ptr<class CoreServices> CoreServicesPtr;

/**
 * \file
 * \addtogroup org_custusx_core_view
 * @{
 */


/** GUI interaction for the CameraStyle.
 *
 * Connect to one CameraStyle instance, then
 * connect the internal actions to that instance.
 * The actions can be used by calling createInteractorStyleActionGroup.
 *
 * \date Dec 9, 2008
 * \author Janne Beate Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 */
class cxResourceVisualization_EXPORT CameraStyleInteractor: public QObject
{
Q_OBJECT
public:
	explicit CameraStyleInteractor();
	QActionGroup* getInteractorStyleActionGroup();
	void connectCameraStyle(ViewGroupDataPtr vg);

private slots:
	void setInteractionStyleActionSlot();
	void updateActionGroup();
private:
	void addInteractorStyleAction(QString caption, QActionGroup* group, QString className, QIcon icon,
					QString helptext);
	QPointer<QActionGroup> mCameraStyleGroup;
	ViewGroupDataPtr mGroup;
	CoreServicesPtr mBackend;
};
typedef boost::shared_ptr<class CameraStyleInteractor> CameraStyleInteractorPtr;

/**
 * @}
 */
} //namespace cx


#endif // CXCAMERASTYLEINTERACTOR_H
