/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVBCAMERAPATH_H
#define CXVBCAMERAPATH_H

#include <QObject>

#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;
typedef vtkSmartPointer<class vtkParametricSpline> vtkParametricSplinePtr;

namespace cx {

/**
 * Class for storing and processing the virtual
 * endoscope camera path when performing
 * virtual endoscopy
 *
 * \ingroup org_custusx_virtualbronchoscopy
 *
 * \date Aug 27, 2015
 * \author Geir Arne Tangen, SINTEF
 */

class CXVBcameraPath : public QObject
{
	Q_OBJECT

private:
    vtkParametricSplinePtr      mSpline;
	TrackingServicePtr			mTrackingService;
	PatientModelServicePtr		mPatientModelService;
	ViewServicePtr				mViewService;
	ToolPtr						mManualTool;

	int							mNumberOfInputPoints;
	int							mNumberOfControlPoints;
	Vector3D					mLastCameraPos_r;
	Vector3D					mLastCameraFocus_r;
    Vector3D                    mLastStoredViewVector;
	double						mLastCameraViewAngle;
	double						mLastCameraRotAngle;

	void		updateManualToolPosition();
	void		generateSplineCurve(MeshPtr mesh);

public:
	CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel,
				   ViewServicePtr visualization);

public slots:
	void cameraRawPointsSlot(MeshPtr mesh);
	void cameraPathPositionSlot(int pos);
	void cameraViewAngleSlot(int angle);
	void cameraRotateAngleSlot(int angle);

};

} /* namespace cx */

#endif // CXVBCAMERAPATH_H
