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
#include "org_custusx_virtualbronchoscopy_Export.h"

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
    bool                        mAutomaticRotation;

	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector< double > mCameraRotations;
    std::vector< double > mCameraRotationsSmoothed;

	void		updateManualToolPosition();
	void		generateSplineCurve(MeshPtr mesh);
	void		generateSplineCurve(std::vector< Eigen::Vector3d > routePositions);
    std::vector< double > smoothCameraRotations(std::vector< double > cameraRotations);

public:
	CXVBcameraPath(TrackingServicePtr tracker, PatientModelServicePtr patientModel,
				   ViewServicePtr visualization);

	void setRoutePositions(std::vector< Eigen::Vector3d > routePositions);
	void setCameraRotations(std::vector< double > cameraRotations);
    void setAutomaticRotation(bool automaticRotation);

signals:
    void		rotationChanged(int value);

public slots:
	void cameraRawPointsSlot(MeshPtr mesh);
	void cameraPathPositionSlot(int positionPercentage);
	void cameraViewAngleSlot(int angle);
	void cameraRotateAngleSlot(int angle);

};

    double org_custusx_virtualbronchoscopy_EXPORT positionPercentageAdjusted(double positionPercentage);

} /* namespace cx */

#endif // CXVBCAMERAPATH_H
