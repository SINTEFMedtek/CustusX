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
        generationNumber = generationNumber + 1;
        parentBranchPtr = parentBranchPtr->getParentBranch();
    }

    return generationNumber;
}

double Branch::findBranchRadius()
{
    int generationNumber = this->findGenerationNumber();

    if (generationNumber == 1)
        return 6;
    if (generationNumber == 2)
        return 4;
    if (generationNumber == 3)
        return 3;
    if (generationNumber == 4)
        return 2.5;
    if (generationNumber == 5)
        return 2;
    else
        return 2;
}


Branch::~Branch()
{

}

}//namespace cx
