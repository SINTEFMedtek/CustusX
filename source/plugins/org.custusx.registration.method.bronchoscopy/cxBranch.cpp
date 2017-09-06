/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
