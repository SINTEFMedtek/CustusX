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
    void findBranchesInCenterline(Eigen::MatrixXd positions_r);
	void selectGenerations(int maxGeneration);
	void calculateOrientations();
	void smoothOrientations();
	void interpolateBranchPositions(int interpolationFactor);
    void smoothBranchPositions(int controlPointDistance);
	BranchListPtr removePositionsForLocalRegistration(Eigen::MatrixXd trackingPositions, double maxDistance);
	vtkPolyDataPtr createVtkPolyDataFromBranches(bool fullyConnected = false, bool straightBranches = false) const;
};

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed);
Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix);
Eigen::MatrixXd eraseCol(int removeIndex, Eigen::MatrixXd positions);
std::pair<Eigen::MatrixXd::Index, double> dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions);
std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

}//namespace cx

#endif /* BRANCHLIST_H_ */
