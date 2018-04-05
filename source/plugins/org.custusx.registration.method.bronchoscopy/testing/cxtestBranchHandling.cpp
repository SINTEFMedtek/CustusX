/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
