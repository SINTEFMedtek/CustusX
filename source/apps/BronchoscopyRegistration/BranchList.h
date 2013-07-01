/*
 * BranchList.h
 *
 *  Created on: Jun 20, 2013
 *      Author: ehofstad
 */

#ifndef BRANCHLIST_H_
#define BRANCHLIST_H_

#include "Branch.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"
#include <vtkPolyData.h>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;

class BranchList
{
	std::vector<Branch*> Branches;
public:
	BranchList();
	virtual ~BranchList();
	void addBranch(Branch* b);
	std::vector<Branch*> getBranches();
	void findBranchesInCenterline(Eigen::MatrixXd positions);
	void calculateOrientations();
	void smoothOrientations();

};

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed);
Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix);
Eigen::MatrixXd eraseCol(int removeIndex, Eigen::MatrixXd positions);
std::pair<Eigen::MatrixXd::Index, double> dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions);
std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

}//namespace cx

#endif /* BRANCHLIST_H_ */
