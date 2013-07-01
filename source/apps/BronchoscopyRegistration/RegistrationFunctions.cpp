/*
 * RegistrationFunctions.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: ehofstad
 */

#include "RegistrationFunctions.h"

namespace cx
{

Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix)
{
	for (int i = 0; i < matrix.cols() - 1; i++)  {
	   for (int j = i + 1; j < matrix.cols(); j++) {
		  if (matrix(i,rowNumber) > matrix(j,rowNumber))
			  matrix.col(i).swap(matrix.col(j));
	   }
	}
return matrix;
}

Eigen::MatrixXd eraseColumn(int removeIndex, Eigen::MatrixXd positions)
{
	positions.block(0 , removeIndex , positions.rows() , positions.cols() - removeIndex) = positions.rightCols(positions.cols() - 1 - removeIndex);
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
	Eigen::VectorXd d(1);
	Eigen::VectorXd D(1);
	for (int i = 0; i > p1.cols(); i++)
	{
		// find nearest neighbour
		(p2.colwise() - p1.col(i)).colwise().squaredNorm().minCoeff(&index);
		d(1) = (p2.col(index) - p1.col(i)).norm();
		indexVector.push_back(index);
		//add distance
		Eigen::VectorXd newD(D.size() + 1);
		newD.head(D.size()) = D;
		newD.tail(1) = d;
		D.swap(newD);
	}
	return std::make_pair(indexVector , D);
}

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed)
{
	Eigen::MatrixXd branchPositions(1,3);
	Eigen::MatrixXd thisPosition(1,3);
	thisPosition = positionsNotUsed.col(startIndex);
	branchPositions = thisPosition; //add first position to branch
	positionsNotUsed = eraseColumn(startIndex,positionsNotUsed);; //remove first position from list of remaining points
	while (positionsNotUsed.size() > 0)
	{
		std::pair<Eigen::MatrixXd::Index, double > minDistance;
		Eigen::MatrixXd::Index index = minDistance.first;
		double d = minDistance.second;
		if (d > 2)
			break;

		thisPosition = positionsNotUsed.col(index);
		positionsNotUsed = eraseColumn(index,positionsNotUsed);
		//add position to branch
		Eigen::MatrixXd newBranchPositions(branchPositions.rows(), branchPositions.cols() + 1);
		newBranchPositions.rightCols<1>() = thisPosition;
		newBranchPositions.topLeftCorner(branchPositions.rows(),branchPositions.cols()) = branchPositions;
		branchPositions.swap(newBranchPositions);
	}
	return std::make_pair(branchPositions, positionsNotUsed);
}


} /* namespace cx */
