/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef BRANCH_H_
#define BRANCH_H_

#include <vector>
#include "cxMesh.h"
#include "cxVector3D.h"
#include "org_custusx_registration_method_bronchoscopy_Export.h"

typedef std::vector<double> dVector;
typedef std::vector<dVector> dVectors;


namespace cx
{

class org_custusx_registration_method_bronchoscopy_EXPORT Branch;
typedef boost::shared_ptr<class Branch> BranchPtr;
typedef std::vector<BranchPtr> branchVector;

class Branch
{
	Eigen::MatrixXd mPositions;
	Eigen::MatrixXd mOrientations;
	branchVector mChildBranches;
	BranchPtr mParentBranch;
public:
	Branch();
	virtual ~Branch();
	void setPositions(Eigen::MatrixXd pos);
	Eigen::MatrixXd getPositions();
	void setOrientations(Eigen::MatrixXd orient);
	Eigen::MatrixXd getOrientations();
	void addChildBranch(BranchPtr child); //Note that this method doesn't set this branch as parent to the child. Inconsistent?
	void setChildBranches(branchVector children); //Note that this method doesn't set this branch as parent to the children. Inconsistent?
	void deleteChildBranches();
	branchVector getChildBranches();
	void setParentBranch(BranchPtr parent); //Note that this method doesn't set this branch as a child of the parent. Inconsistent?
    int findGenerationNumber();
    double findBranchRadius();
	BranchPtr getParentBranch();
	int findParentIndex(branchVector bv) const;
};


}//namespace cx

#endif /* BRANCH_H_ */



