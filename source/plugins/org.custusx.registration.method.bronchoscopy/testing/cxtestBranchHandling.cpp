/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <algorithm>
#include <cmath>

#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxBranchList.h"
#include "cxBranch.h"
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

namespace
{

cx::BranchPtr createBranch(Eigen::Vector3d start, Eigen::Vector3d direction, int lengthInMm)
{
	Eigen::MatrixXd positions(3, lengthInMm + 1);
	for (int i = 0; i <= lengthInMm; ++i)
	{
		positions.col(i) = start + i * direction.normalized();
	}
	cx::BranchPtr retval(new cx::Branch());
	retval->setPositions(positions);
	return retval;
}

void connect(cx::BranchPtr parent, cx::BranchPtr child)
{
	parent->addChildBranch(child);
	child->setParentBranch(parent);
}

} // namespace

TEST_CASE("BranchList: a short end branch on the trachea is removed and the trachea merged", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr spur = createBranch(Eigen::Vector3d(1, 0, 60), Eigen::Vector3d(1, 0, 0), 4);
	cx::BranchPtr lowerTrachea = createBranch(Eigen::Vector3d(0, 0, 59), Eigen::Vector3d(0, 0, -1), 30);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 28), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 28), Eigen::Vector3d(1, 0, -1), 20);
	connect(trachea, spur);
	connect(trachea, lowerTrachea);
	connect(lowerTrachea, left);
	connect(lowerTrachea, right);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, spur, lowerTrachea, left, right})
	{
		branchList->addBranch(branch);
	}

	branchList->removeShortEndBranchesFromMainAirways(10);

	REQUIRE(branchList->getBranches().size() == 3);
	CHECK(branchList->getBranches()[0] == trachea);
	CHECK(trachea->getPositions().cols() == 41 + 31);
	REQUIRE(trachea->getChildBranches().size() == 2);
	CHECK(left->getParentBranch() == trachea);
	CHECK(right->getParentBranch() == trachea);
}

TEST_CASE("BranchList: long or branching children of the trachea are kept", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 15);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 6);
	cx::BranchPtr rightChild = createBranch(Eigen::Vector3d(6, 0, 53), Eigen::Vector3d(1, 0, -1), 20);
	connect(trachea, left);
	connect(trachea, right);
	connect(right, rightChild);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, left, right, rightChild})
	{
		branchList->addBranch(branch);
	}

	branchList->removeShortEndBranchesFromMainAirways(10);

	CHECK(branchList->getBranches().size() == 4);
	CHECK(trachea->getChildBranches().size() == 2);
}

TEST_CASE("BranchList: the trachea keeps its children if all of them are short end branches", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 5);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 5);
	connect(trachea, left);
	connect(trachea, right);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, left, right})
	{
		branchList->addBranch(branch);
	}

	branchList->removeShortEndBranchesFromMainAirways(10);

	CHECK(branchList->getBranches().size() == 3);
}

TEST_CASE("BranchList: a short end branch on a main bronchus is removed and the bronchus merged", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 10);
	cx::BranchPtr spur = createBranch(Eigen::Vector3d(11, 0, 49), Eigen::Vector3d(0, 1, 0), 7);
	cx::BranchPtr rightLower = createBranch(Eigen::Vector3d(12, 0, 48), Eigen::Vector3d(1, 0, -1), 15);
	cx::BranchPtr rightLowerA = createBranch(Eigen::Vector3d(27, 0, 33), Eigen::Vector3d(1, 0, 0), 12);
	cx::BranchPtr rightLowerB = createBranch(Eigen::Vector3d(27, 0, 33), Eigen::Vector3d(0, 0, -1), 12);
	connect(trachea, left);
	connect(trachea, right);
	connect(right, spur);
	connect(right, rightLower);
	connect(rightLower, rightLowerA);
	connect(rightLower, rightLowerB);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, left, right, spur, rightLower, rightLowerA, rightLowerB})
	{
		branchList->addBranch(branch);
	}

	branchList->removeShortEndBranchesFromMainAirways(10);

	CHECK(branchList->getBranches().size() == 5);
	CHECK(right->getPositions().cols() == 11 + 16);
	REQUIRE(right->getChildBranches().size() == 2);
	CHECK(rightLowerA->getParentBranch() == right);
}

TEST_CASE("BranchList: the root with the largest tree becomes the trachea, short loose roots are removed", "[unit][bronchoscopy]")
{
	cx::BranchPtr fragment = createBranch(Eigen::Vector3d(5, 0, 105), Eigen::Vector3d(0, 0, -1), 8);
	cx::BranchPtr longFragment = createBranch(Eigen::Vector3d(30, 0, 80), Eigen::Vector3d(0, 0, -1), 15);
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 20);
	connect(trachea, left);
	connect(trachea, right);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {fragment, longFragment, trachea, left, right})
	{
		branchList->addBranch(branch);
	}

	branchList->selectTracheaAsLargestTree(10);

	std::vector<cx::BranchPtr> branches = branchList->getBranches();
	REQUIRE(branches.size() == 4);
	CHECK(branches[0] == trachea);
	CHECK(std::find(branches.begin(), branches.end(), fragment) == branches.end());
	CHECK(std::find(branches.begin(), branches.end(), longFragment) != branches.end());
}

TEST_CASE("BranchList: single children are merged anywhere in the tree", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 10);
	cx::BranchPtr rightContinued = createBranch(Eigen::Vector3d(12, 0, 48), Eigen::Vector3d(1, 0, -1), 10);
	cx::BranchPtr rightA = createBranch(Eigen::Vector3d(23, 0, 37), Eigen::Vector3d(1, 0, 0), 12);
	cx::BranchPtr rightB = createBranch(Eigen::Vector3d(23, 0, 37), Eigen::Vector3d(0, 0, -1), 12);
	connect(trachea, left);
	connect(trachea, right);
	connect(right, rightContinued);
	connect(rightContinued, rightA);
	connect(rightContinued, rightB);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, left, right, rightContinued, rightA, rightB})
	{
		branchList->addBranch(branch);
	}

	branchList->mergeAllSingleChildren();

	CHECK(branchList->getBranches().size() == 5);
	CHECK(right->getPositions().cols() == 11 + 11);
	CHECK(right->getChildBranches().size() == 2);
	CHECK(rightA->getParentBranch() == right);
}

TEST_CASE("BranchList: end branches on the trachea at least as long as the limit are kept", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr tracheaLower = createBranch(Eigen::Vector3d(0, 0, 59), Eigen::Vector3d(0, 0, -1), 20);
	cx::BranchPtr sideBranch = createBranch(Eigen::Vector3d(1, 0, 60), Eigen::Vector3d(1, 0, 0), 10);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 38), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 38), Eigen::Vector3d(1, 0, -1), 20);
	connect(trachea, tracheaLower);
	connect(trachea, sideBranch);
	connect(tracheaLower, left);
	connect(tracheaLower, right);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, tracheaLower, sideBranch, left, right})
	{
		branchList->addBranch(branch);
	}

	branchList->removeShortEndBranchesFromMainAirways(10);

	CHECK(branchList->getBranches().size() == 5);
	CHECK(trachea->getChildBranches().size() == 2);
}

namespace
{

void addLine(std::vector<Eigen::Vector3d>& points, Eigen::Vector3d from, Eigen::Vector3d to)
{
	const int steps = static_cast<int>(std::ceil((to - from).norm()));
	for (int i = 1; i <= steps; ++i)
	{
		points.push_back(from + (to - from) * i / steps);
	}
}

/** Centerline points of a trachea with a short side spur, two main bronchi and four lobar bronchi. */
Eigen::MatrixXd airwayTreeWithTracheaSpur()
{
	std::vector<Eigen::Vector3d> points(1, Eigen::Vector3d(0, 0, 150));
	addLine(points, Eigen::Vector3d(0, 0, 150), Eigen::Vector3d(0, 0, 100));
	addLine(points, Eigen::Vector3d(0, 0, 126), Eigen::Vector3d(6, 0, 128));
	for (int side : {-1, 1})
	{
		addLine(points, Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(side*30, 0, 80));
		addLine(points, Eigen::Vector3d(side*30, 0, 80), Eigen::Vector3d(side*50, 0, 65));
		addLine(points, Eigen::Vector3d(side*30, 0, 80), Eigen::Vector3d(side*32, 0, 55));
	}
	Eigen::MatrixXd retval(3, points.size());
	for (size_t i = 0; i < points.size(); ++i)
	{
		retval.col(i) = points[i];
	}
	return retval;
}

} // namespace

TEST_CASE("BranchList: findBranchesInCenterline cleans up a spur on the trachea", "[unit][bronchoscopy]")
{
	cx::BranchListPtr cleaned(new cx::BranchList());
	cleaned->findBranchesInCenterline(airwayTreeWithTracheaSpur());
	cx::BranchListPtr raw(new cx::BranchList());
	raw->findBranchesInCenterline(airwayTreeWithTracheaSpur(), true, true, false);

	REQUIRE(raw->getBranches().size() == 9);
	bool rawTracheaHasEndBranch = false;
	for (cx::BranchPtr child : raw->getBranches()[0]->getChildBranches())
	{
		rawTracheaHasEndBranch = rawTracheaHasEndBranch || child->getChildBranches().empty();
	}
	CHECK(rawTracheaHasEndBranch);

	REQUIRE(cleaned->getBranches().size() == 7);
	cx::BranchPtr trachea = cleaned->getBranches()[0];
	REQUIRE(trachea->getChildBranches().size() == 2);
	for (cx::BranchPtr mainBronchus : trachea->getChildBranches())
	{
		CHECK(mainBronchus->getChildBranches().size() == 2);
	}
}


TEST_CASE("BranchList: deleteBranch removes only that branch from its parent", "[unit][bronchoscopy]")
{
	cx::BranchPtr trachea = createBranch(Eigen::Vector3d(0, 0, 100), Eigen::Vector3d(0, 0, -1), 40);
	cx::BranchPtr left = createBranch(Eigen::Vector3d(-1, 0, 59), Eigen::Vector3d(-1, 0, -1), 20);
	cx::BranchPtr right = createBranch(Eigen::Vector3d(1, 0, 59), Eigen::Vector3d(1, 0, -1), 20);
	connect(trachea, left);
	connect(trachea, right);
	cx::BranchListPtr branchList(new cx::BranchList());
	for (cx::BranchPtr branch : {trachea, left, right})
	{
		branchList->addBranch(branch);
	}

	branchList->deleteBranch(left);

	CHECK(branchList->getBranches().size() == 2);
	REQUIRE(trachea->getChildBranches().size() == 1);
	CHECK(trachea->getChildBranches()[0] == right);
}

} //namespace cxtest
