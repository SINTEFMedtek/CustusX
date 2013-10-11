/*
 * Branch.h
 *
 *  Created on: May 30, 2013
 *      Author: ehofstad
 */

#ifndef BRANCH_H_
#define BRANCH_H_

#include <vector>
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"

typedef std::vector<double> dVector;
typedef std::vector<dVector> dVectors;


namespace cx
{

class Branch;
typedef std::vector<Branch*> branchVector;

class Branch
{
	Eigen::MatrixXd positions;
	Eigen::MatrixXd orientations;
	branchVector childBranches;
	Branch* parentBranch;
public:
	Branch();
	virtual ~Branch();
	void setPositions(Eigen::MatrixXd pos);
	Eigen::MatrixXd getPositions();
	void setOrientations(Eigen::MatrixXd orient);
	Eigen::MatrixXd getOrientations();
	void addChildBranch(Branch* child);
	void setChildBranches(branchVector children);
	void deleteChildBranches();
	branchVector getChildBranches();
	void setParentBranch(Branch* parent);
	Branch* getParentBranch();

};


}//namespace cx

#endif /* BRANCH_H_ */



