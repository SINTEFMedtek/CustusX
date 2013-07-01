/*
 * BranchList.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: ehofstad
 */

#include "BranchList.h"
#include "RegistrationFunctions.h"
#include "Branch.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"
#include <vtkPolyData.h>



namespace cx
{

BranchList::BranchList()
{

}


BranchList::~BranchList()
{
	for (int i = 0; i < Branches.size(); i++)
		Branches[i]->~Branch();
}

void BranchList::addBranch(Branch* b)
{
	Branches.push_back(b);
}

std::vector<Branch*> BranchList::getBranches()
{
	return Branches;
}


void BranchList::calculateOrientations()
{
	for (int i = 0; i < Branches.size(); i++)
	{
		Eigen::MatrixXd positions = Branches[i]->getPositions();
		Eigen::MatrixXd diff = positions.rightCols(positions.cols() - 1) - positions.leftCols(positions.cols() - 1);
		Eigen::MatrixXd orientations(positions.rows(),positions.cols());
		orientations.leftCols(orientations.cols() - 1) = diff / diff.norm();
		orientations.rightCols(1) = orientations.col(orientations.cols() - 1);
		Branches[i]->setOrientations(orientations);
	}
}

void BranchList::smoothOrientations()
{
	for (int i = 0; i < Branches.size(); i++)
	{
		Eigen::MatrixXd orientations = Branches[i]->getOrientations();
		Eigen::MatrixXd newOrientations(orientations.rows(),orientations.cols());
		int numberOfColumns = orientations.cols();
		for (int j = 0; j < numberOfColumns; j++)
		{
			newOrientations.col(j) = orientations.block(0,std::max(j-2,0),orientations.rows(),std::min(5,numberOfColumns-j)).rowwise().mean(); //smoothing
			newOrientations.col(j) = newOrientations.col(j) / newOrientations.col(j).norm(); // normalizing
		}
		Branches[i]->setOrientations(newOrientations);
	}
}




void BranchList::findBranchesInCenterline(Eigen::MatrixXd positions)
{
	positions = sortMatrix(2,positions);
	Eigen::MatrixXd positionsNotUsed = positions;
	int minIndex;
	int index;
	int splitIndex;
	Eigen::MatrixXd::Index startIndex;
	Branch* branchToSplit;
	while (positionsNotUsed.size() > 0)
	{
		if (Branches.size() > 0)
		{
			double minDistance = 1000;
			for (int i = 0; i < Branches.size(); i++)
			{
				std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd> distances;
				distances = dsearchn(positionsNotUsed, Branches[i]->getPositions());
				double d = distances.second.minCoeff(&index);
				if (d < minDistance)
				{
					minDistance = d;
					branchToSplit = Branches[i];
					startIndex = index;
					if (minDistance < 2)
						break;
				}
			}
			std::pair<Eigen::MatrixXd::Index, double> dsearchResult = dsearch(positionsNotUsed.col(startIndex) , branchToSplit->getPositions());
			splitIndex = dsearchResult.first;
		}
		else //if this is the first branch. Select the top position (Trachea).
			startIndex = positionsNotUsed.cols() - 1;

		std::pair<Eigen::MatrixXd,Eigen::MatrixXd > connectedPointsResult = findConnectedPointsInCT(startIndex , positionsNotUsed);
		Eigen::MatrixXd branchPositions = connectedPointsResult.first;
		positionsNotUsed = connectedPointsResult.second;

		if (branchPositions.cols() >= 5) //only include brances of length >= 5 points
		{
			Branch* newBranch = new Branch();
			newBranch->setPositions(branchPositions);
			Branches.push_back(newBranch);

			if (Branches.size() > 1) // do not try to split another branch when the first branch is processed
			{
				if (splitIndex + 1 >= 5 and branchToSplit->getPositions().cols() - splitIndex - 1 >= 5)
					//do not split branch if the new branch is close to the edge of the branch
					//if the new branch is not close to one of the edges of the
					//connected existing branch: Split the existing branch
				{
					Branch* newBranchFromSplit = new Branch();
					Eigen::MatrixXd branchToSplitPositions = branchToSplit->getPositions();
					newBranchFromSplit->setPositions(branchToSplitPositions.rightCols(branchToSplitPositions.cols() - splitIndex - 1));
					branchToSplit->setPositions(branchToSplitPositions.leftCols(splitIndex + 1));
					Branches.push_back(newBranchFromSplit);
					newBranchFromSplit->setParentBranch(branchToSplit);
					newBranch->setParentBranch(branchToSplit);
					newBranchFromSplit->setChildBranches(branchToSplit->getChildBranches());
					branchVector branchToSplitChildren = branchToSplit->getChildBranches();
					for (int i = 0; i < branchToSplitChildren.size(); i++)
						branchToSplitChildren[i]->setParentBranch(newBranchFromSplit);
					branchToSplit->deleteChildBranches();
					branchToSplit->addChildBranch(newBranchFromSplit);
					branchToSplit->addChildBranch(newBranch);
				}
				else if (splitIndex + 1 < 5)
					 // If the new branch is close to the start of the existing
					 // branch: Connect it to the same position start as the
					 // existing branch
				{
					newBranch->setParentBranch(branchToSplit->getParentBranch());
					branchToSplit->getParentBranch()->addChildBranch(newBranch);
				}
				else if (branchToSplit->getPositions().cols() - splitIndex - 1 < 5)
					// If the new branch is close to the end of the existing
					// branch: Connect it to the end of the existing branch
				{
					newBranch->setParentBranch(branchToSplit);
					branchToSplit->addChildBranch(newBranch);
				}

			}

		}
	}
}


Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix)
{
	for (int i = 0; i < matrix.cols() - 1; i++)  {
	   for (int j = i + 1; j < matrix.cols(); j++) {
		  if (matrix(rowNumber,i) > matrix(rowNumber,j)){
			  matrix.col(i).swap(matrix.col(j));
		  }
	   }
	}
return matrix;
}



Eigen::MatrixXd eraseCol(int removeIndex, Eigen::MatrixXd positions)
{
	positions.block(0 , removeIndex , positions.rows() , positions.cols() - removeIndex - 1) = positions.rightCols(positions.cols() - removeIndex - 1);
	positions.conservativeResize(Eigen::NoChange, positions.cols() - 1);
	return positions;
}

std::pair<Eigen::MatrixXd::Index, double> dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions)
{
	Eigen::MatrixXd::Index index;
	// find nearest neighbour
	(positions.colwise() - p).colwise().squaredNorm().minCoeff(&index);
	double d = (positions.col(index) - p).norm();

	return std::make_pair(index , d);
}

std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2)
{
	Eigen::MatrixXd::Index index;
	std::vector<Eigen::MatrixXd::Index> indexVector;
	Eigen::VectorXd D(p1.cols());
	for (int i = 0; i < p1.cols(); i++)
	{
		// find nearest neighbour
		(p2.colwise() - p1.col(i)).colwise().squaredNorm().minCoeff(&index);
		D(i) = (p2.col(index) - p1.col(i)).norm();
		indexVector.push_back(index);
	}
	return std::make_pair(indexVector , D);
}

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed)
{
	Eigen::MatrixXd branchPositions(3,1);
	Eigen::MatrixXd thisPosition(3,1);
	thisPosition = positionsNotUsed.col(startIndex);
	branchPositions = thisPosition; //add first position to branch
	positionsNotUsed = eraseCol(startIndex,positionsNotUsed);; //remove first position from list of remaining points

	while (positionsNotUsed.size() > 0)
	{
		std::pair<Eigen::MatrixXd::Index, double > minDistance = dsearch(thisPosition, positionsNotUsed);
		Eigen::MatrixXd::Index index = minDistance.first;
		double d = minDistance.second;
		if (d > 2)
			break;

		thisPosition = positionsNotUsed.col(index);
		positionsNotUsed = eraseCol(index,positionsNotUsed);
		//add position to branch
		Eigen::MatrixXd newBranchPositions(branchPositions.rows(), branchPositions.cols() + 1);
		newBranchPositions.rightCols(1) = thisPosition;
		newBranchPositions.leftCols(branchPositions.cols()) = branchPositions;
		branchPositions.swap(newBranchPositions);
	}
	return std::make_pair(branchPositions, positionsNotUsed);
}


}//namespace cx
