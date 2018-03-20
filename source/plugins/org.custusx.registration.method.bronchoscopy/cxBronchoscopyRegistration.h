/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef BRONCHOSCOPYREGISTRATION_H_
#define BRONCHOSCOPYREGISTRATION_H_

#include "org_custusx_registration_method_bronchoscopy_Export.h"
#include "cxBranchList.h"
#include <vector>
#include "vtkForwardDeclarations.h"

typedef std::vector< Eigen::Matrix4d > M4Vector;


namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;
typedef boost::shared_ptr<class BranchList> BranchListPtr;

class org_custusx_registration_method_bronchoscopy_EXPORT BronchoscopyRegistration
{
	BranchListPtr mBranchListPtr;
	bool mCenterlineProcessed;

public:
	BronchoscopyRegistration();
	vtkPolyDataPtr processCenterline(vtkPolyDataPtr centerline, Transform3D rMd, int numberOfGenerations = 0);
    BranchListPtr processCenterlineImage2Image(vtkPolyDataPtr centerline, int numberOfGenerations = 0);
	Eigen::Matrix4d runBronchoscopyRegistration(TimedTransformMap trackingData_prMt, Transform3D old_rMpr, double maxDistanceForLocalRegistration);
    Eigen::Matrix4d runBronchoscopyRegistrationImage2Image(vtkPolyDataPtr centerlineFixed, vtkPolyDataPtr centerlineMoving);
	bool isCenterlineProcessed();
	virtual ~BronchoscopyRegistration();
};

M4Vector excludeClosePositions();
Eigen::Matrix4d registrationAlgorithm(BranchListPtr branches, M4Vector Tnavigation);
Eigen::Matrix4d registrationAlgorithmImage2Image(BranchListPtr branchesFixed, BranchListPtr branchesMoving);
std::vector<Eigen::MatrixXd::Index> dsearch2n(Eigen::MatrixXd pos1, Eigen::MatrixXd pos2, Eigen::MatrixXd ori1, Eigen::MatrixXd ori2);
vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions);
Eigen::Matrix4d performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok);
std::pair<Eigen::MatrixXd , Eigen::MatrixXd> RemoveInvalidData(Eigen::MatrixXd positionData, Eigen::MatrixXd orientationData);
M4Vector RemoveInvalidData(M4Vector T_vector);
org_custusx_registration_method_bronchoscopy_EXPORT Eigen::MatrixXd makeTransformedMatrix(vtkPolyDataPtr linesPolyData, Transform3D rMd = Transform3D::Identity());

}//namespace cx

#endif /* BRONCHOSCOPYREGISTRATION_H_ */
