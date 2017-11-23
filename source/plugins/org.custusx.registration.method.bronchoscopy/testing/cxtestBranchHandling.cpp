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

#include "catch.hpp"

#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxBranchList.h"
#include "cxtestVtkPolyDataTree.h"
#include "cxBronchoscopyRegistration.h"


namespace cxtest
{

TEST_CASE("Test the find number of branches in the dummy centerline", "[unit][bronchoscopy]")
{
    vtkPolyDataPtr linesPolyData = makeDummyCenterLine();
	Eigen::MatrixXd CLpoints = cx::makeTransformedMatrix(linesPolyData);

	cx::BranchListPtr bl = cx::BranchListPtr(new cx::BranchList());
    bl->findBranchesInCenterline(CLpoints);

    CHECK(bl->getBranches().size() == 3);
	CHECK(linesPolyData->GetNumberOfPoints() == 300);
	CHECK(linesPolyData->GetNumberOfCells() == 300 - 1);
}

TEST_CASE("Test the connect branches to a single poly data object, original - i.e. branches are not fully connected", "[unit][bronchoscopy]")
{
	int n1 = 100;
	int n2 = 100;
	int n3 = 100;
	vtkPolyDataPtr linesPolyData = makeDummyCenterLine(n1,n2,n3);
	Eigen::MatrixXd CLpoints = cx::makeTransformedMatrix(linesPolyData);
	cx::BranchListPtr bl = cx::BranchListPtr(new cx::BranchList());
	bl->findBranchesInCenterline(CLpoints);

	vtkPolyDataPtr linesFromBranches = bl->createVtkPolyDataFromBranches();

	CHECK(linesFromBranches->GetNumberOfPoints() == n1+n2+n3);
	CHECK(linesFromBranches->GetNumberOfCells() == n1+n2+n3 - 3);
}

TEST_CASE("Test the connect branches to a single poly data object, fully connected", "[unit][bronchoscopy]")
{
	int n1 = 100;
	int n2 = 100;
	int n3 = 100;
	vtkPolyDataPtr linesPolyData = makeDummyCenterLine(n1,n2,n3);
	Eigen::MatrixXd CLpoints = cx::makeTransformedMatrix(linesPolyData);
	cx::BranchListPtr bl = cx::BranchListPtr(new cx::BranchList());
	bl->findBranchesInCenterline(CLpoints);

	vtkPolyDataPtr linesFromBranches = bl->createVtkPolyDataFromBranches(true);

	CHECK(linesFromBranches->GetNumberOfPoints() == n1+n2+n3);
	CHECK(linesFromBranches->GetNumberOfCells() == n1+n2+n3 - 1);
}

TEST_CASE("Test the connect branches to a single poly data object, fully connected and straight lines", "[unit][bronchoscopy]")
{
    /**
     * The branches are made first, hence 3 branches gives 6 points and 3 lines.
     * Then the two child branches are connected to the parent
     * with two additional lines, giving 5 lines in total.
     */
	vtkPolyDataPtr linesPolyData = makeDummyCenterLine();
	Eigen::MatrixXd CLpoints = cx::makeTransformedMatrix(linesPolyData);
	cx::BranchListPtr bl = cx::BranchListPtr(new cx::BranchList());
	bl->findBranchesInCenterline(CLpoints);

	vtkPolyDataPtr linesFromBranches = bl->createVtkPolyDataFromBranches(true, true);

    CHECK(linesFromBranches->GetNumberOfPoints() == 6);
    CHECK(linesFromBranches->GetNumberOfCells() == 5);
}

TEST_CASE("Test the findParentIndex method", "[unit][bronchoscopy]")
{
	cx::BranchPtr parent = cx::BranchPtr(new cx::Branch());
	cx::BranchPtr parent2 = cx::BranchPtr(new cx::Branch());
	cx::BranchPtr child = cx::BranchPtr(new cx::Branch());
	cx::BranchPtr strayBranch = cx::BranchPtr(new cx::Branch());

	cx::branchVector v;
	v.push_back(parent);
	v.push_back(parent2);

	//parent is index i
	child->setParentBranch(parent);
	CHECK(child->findParentIndex(v) == 0);

	//parent is index 1
	child->setParentBranch(parent2);
	CHECK(child->findParentIndex(v) == 1);

	//parent is not in vector
	child->setParentBranch(strayBranch);
	CHECK(child->findParentIndex(v) == -1);

}

} //namespace cxtest
