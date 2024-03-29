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
#include "cxForwardDeclarations.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include "org_custusx_registration_method_bronchoscopy_Export.h"

typedef std::vector<double> dVector;
typedef std::vector<dVector> dVectors;


namespace cx
{

class org_custusx_registration_method_bronchoscopy_EXPORT Branch;
typedef std::vector<BranchPtr> branchVector;

class Branch
{
	Eigen::MatrixXd mPositions;
	Eigen::MatrixXd mOrientations;
	Eigen::VectorXd mRadius;
	branchVector mChildBranches;
	BranchPtr mParentBranch;
	Vector3D mBendingDirection;
	double mBronchoscopeRotation = 0;
	QString mLobe;

public:
	Branch();
	virtual ~Branch();
	void setPositions(Eigen::MatrixXd pos);
	Eigen::MatrixXd getPositions();
	void setOrientations(Eigen::MatrixXd orient);
	Eigen::MatrixXd getOrientations();
	void setRadius(Eigen::VectorXd r);
	Eigen::VectorXd getRadius();
	double getAverageRadius();
	void addChildBranch(BranchPtr child); //Note that this method doesn't set this branch as parent to the child. Inconsistent?
	void setChildBranches(branchVector children); //Note that this method doesn't set this branch as parent to the children. Inconsistent?
	void deleteChildBranches();
	branchVector getChildBranches();
	void setParentBranch(BranchPtr parent); //Note that this method doesn't set this branch as a child of the parent. Inconsistent?
	int findGenerationNumber();
	double findBranchRadius();
	BranchPtr getParentBranch();
	void calculateOrientations();
	int findParentIndex(branchVector bv) const;
	void setBronchoscopeBendingDirection(Vector3D bendingDirection);
	Vector3D getBronchoscopeBendingDirection();
	void setBronchoscopeRotation(double rotation);
	double getBronchoscopeRotation();
	void removeEqualPositions();
	void setLap(QString lap);
	QString getLap();

};


}//namespace cx

#endif /* BRANCH_H_ */



