/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxBranch.h"

namespace cx
{

Branch::Branch() :
	mParentBranch(BranchPtr())
{
	// TODO Auto-generated constructor stub

}

void Branch::setPositions(Eigen::MatrixXd pos)
{
	mPositions = pos;
	this->removeEqualPositions();
	this->calculateOrientations();
}

Eigen::MatrixXd Branch::getPositions()
{
	return mPositions;
}

void Branch::setOrientations(Eigen::MatrixXd orient)
{
	mOrientations = orient;
}

Eigen::MatrixXd Branch::getOrientations()
{
	return mOrientations;
}

void Branch::setRadius(Eigen::VectorXd r)
{
	mRadius = r;
}

Eigen::VectorXd Branch::getRadius()
{
	return mRadius;
}

double Branch::getAverageRadius()
{
	return mRadius.mean();
}

void Branch::addChildBranch(BranchPtr child)
{
	mChildBranches.push_back(child);
}

void Branch::setChildBranches(branchVector children)
{
	mChildBranches = children;
}

void Branch::deleteChildBranches()
{
	mChildBranches.clear();
}

branchVector Branch::getChildBranches()
{
	return mChildBranches;
}

void Branch::setParentBranch(BranchPtr parent)
{
	mParentBranch = parent;
}
BranchPtr Branch::getParentBranch()
{
	return mParentBranch;
}

void Branch::calculateOrientations()
{
	Eigen::MatrixXd positions = this->getPositions();
	Eigen::MatrixXd diff = positions.rightCols(positions.cols() - 1) - positions.leftCols(positions.cols() - 1);
	Eigen::MatrixXd orientations(positions.rows(),positions.cols());
	orientations.leftCols(orientations.cols() - 1) = diff / diff.norm();
	orientations.rightCols(1) = orientations.col(orientations.cols() - 1);
	this->setOrientations(orientations);
}

/**
 * @brief Branch::findParentIndex
 * Given a vector of branches, find this branch's parent branch in that vector.
 * Return the index in the vector of the parent branch.
 * @param bv
 * A vector of branches
 * @return
 * The index the parent branch has in the input vector.
 * Or -1 if the parent branch is not found in the vector.
 */
int Branch::findParentIndex(branchVector bv) const
{
	for(size_t i = 0; i < bv.size(); ++i)
	{
		if(bv[i] == mParentBranch)
			return static_cast<int>(i);
	}

	return -1;
}

int Branch::findGenerationNumber()
{
	int generationNumber = 1;

	BranchPtr parentBranchPtr = this->getParentBranch();
	while (parentBranchPtr)
	{
		if (parentBranchPtr->getChildBranches().size() > 1) // Do not count generation if it is not a real division
			generationNumber = generationNumber + 1;

		parentBranchPtr = parentBranchPtr->getParentBranch();

		if (generationNumber > 23) //maximum possible generations - avoiding infinite loop
			break;
	}

	return generationNumber;
}

double Branch::findBranchRadius()
{
	int generationNumber = this->findGenerationNumber();

	if (generationNumber == 1)
		return 8;
	if (generationNumber == 2)
		return 6;
	if (generationNumber == 3)
		return 4;
	if (generationNumber == 4)
		return 3;
	if (generationNumber == 5)
		return 2.5;
	else
		return 2;
}

void Branch::setBronchoscopeRotation(double rotation)
{
	mBronchoscopeRotation = rotation;
}

double Branch::getBronchoscopeRotation()
{
	return mBronchoscopeRotation;
}

void Branch::removeEqualPositions()
{
	Eigen::MatrixXd positions = this->getPositions();
	Eigen::MatrixXd orientations = this->getOrientations();
	bool resizeOrientations = false;
	if (positions.cols() == orientations.cols())
		resizeOrientations = true;

	for (int i = positions.cols() - 1; i > 0; i--)
	{
		if (similar( (positions.col(i)-positions.col(i-1)).cwiseAbs().sum(), 0))
		{
			positions.block(0 , i , positions.rows() , positions.cols() - i - 1) = positions.rightCols(positions.cols() - i - 1);
			positions.conservativeResize(Eigen::NoChange, positions.cols() - 1);
			if (resizeOrientations)
				orientations.conservativeResize(Eigen::NoChange, orientations.cols() - 1);
		}
	}

	mPositions = positions;
	if (resizeOrientations)
		mOrientations = orientations;
}


Branch::~Branch()
{

}

}//namespace cx
