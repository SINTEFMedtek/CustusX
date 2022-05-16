/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef BRANCHLIST_H_
#define BRANCHLIST_H_

#include "cxBranch.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include "org_custusx_registration_method_bronchoscopy_Export.h"


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class BranchList> BranchListPtr;

#define MAX_ROTATION_TO_TILT_DOWN_DEGREES 30 //Threshold between rotation direction of scope to tilt up or down in a bifurcation.

class org_custusx_registration_method_bronchoscopy_EXPORT BranchList
{
	std::vector<BranchPtr> mBranches;
public:
	BranchList();
	virtual ~BranchList();
	void addBranch(BranchPtr b);
	void deleteBranch(BranchPtr b);
	void deleteAllBranches();
	std::vector<BranchPtr> getBranches();
	void findBranchesInCenterline(Eigen::MatrixXd positions_r, bool sortByZindex = true);
	void selectGenerations(int maxGeneration);
	void findBronchoscopeRotation();
	void calculateBronchoscopeRotation(BranchPtr branch);
	void smoothOrientations();
	void smoothRadius();
	BranchPtr findBranchWithLargestRadius();
    void interpolateBranchPositions(double resolution);
	void smoothBranchPositions(int controlPointDistance);
	void excludeClosePositionsInCTCenterline(double minPointDistance);
	BranchListPtr removePositionsForLocalRegistration(Eigen::MatrixXd trackingPositions, double maxDistance);
	vtkPolyDataPtr createVtkPolyDataFromBranches(bool fullyConnected = false, bool straightBranches = false) const;
};

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > org_custusx_registration_method_bronchoscopy_EXPORT findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed);
Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix);
Eigen::MatrixXd org_custusx_registration_method_bronchoscopy_EXPORT eraseCol(int removeIndex, Eigen::MatrixXd positions);
std::pair<Eigen::MatrixXd::Index, double> org_custusx_registration_method_bronchoscopy_EXPORT dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions);
std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > org_custusx_registration_method_bronchoscopy_EXPORT dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
std::vector<Eigen::Vector3d> org_custusx_registration_method_bronchoscopy_EXPORT smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition);
double bendingDirectionToBronchoscopeRotation(Vector3D bendingDirection, Vector3D parentBranchOrientation, double parentRotation);
Vector3D calculateBronchoscopeBendingDirection(Vector3D A, Vector3D B);

}//namespace cx

#endif /* BRANCHLIST_H_ */
