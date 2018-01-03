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



