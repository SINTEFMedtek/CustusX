/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CENTERLINEREGISTRATION_H_
#define CENTERLINEREGISTRATION_H_

#include <vector>
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "org_custusx_registration_method_centerline_Export.h"
#include "vtkForwardDeclarations.h"
#include <map>
#include "cxTransform3D.h"
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkLandmarkTransform.h>


typedef std::vector< Eigen::Matrix4d > M4Vector;


namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;
typedef vtkSmartPointer<vtkDoubleArray>             vtkDoubleArrayPtr;
typedef vtkSmartPointer<vtkPoints>                  vtkPointsPtr;
typedef vtkSmartPointer<vtkPolyData>                vtkPolyDataPtr;

/** Rigid registration of tracked positions (moving) to a centerline (fixed).
 *
 * Minimizes the sum of squared distances from each transformed moving point to
 * its closest fixed point, with Levenberg-Marquardt over the Euler angles
 * (rotation Rz*Rx*Ry about the origin) and the translation. Each of the six
 * parameters can be locked with UpdateScales().
 */
class org_custusx_registration_method_centerline_EXPORT CenterlineRegistration
{

public:
    CenterlineRegistration();
    vtkPointsPtr smoothPositions(vtkPointsPtr centerline);
    void UpdateScales(bool xRot, bool yRot, bool zRot, bool xTrans, bool yTrans, bool zTrans);
    void SetFixedPoints(vtkPointsPtr points);
    void SetMovingPoints(vtkPointsPtr points);
    Transform3D FullRegisterMoving(Transform3D init_transform);
    vtkPointsPtr processCenterline(vtkPolyDataPtr centerline, Transform3D rMd);
    vtkPointsPtr ConvertTrackingDataToVTK(TimedTransformMap trackingData_prMt, Transform3D rMpr);
    Transform3D runCenterlineRegistration(vtkPolyDataPtr centerline, Transform3D rMd, TimedTransformMap trackingData_prMt, Transform3D old_rMpr );
    virtual ~CenterlineRegistration();

private:
    std::vector<Vector3D> mFixedPoints;
    std::vector<Vector3D> mMovingPoints;
    bool mFreeParameters[6];
    Transform3D mResultTransform;
    bool mRegistrationUpdated;

};

Eigen::Matrix4d registrationAlgorithm(M4Vector Tnavigation);
vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions);
}//namespace cx

#endif /* CENTERLINEREGISTRATION_H_ */
