

#include "cxRouteToTarget.h"
#include <vtkPolyData.h>
//#include "cxDoubleDataAdapterXml.h"
#include "cxBranchList.h"
#include "cxBranch.h"
#include <vtkCellArray.h>

namespace cx
{

RouteToTarget::RouteToTarget():
	mBranchListPtr(new BranchList),
	mProjectedIndex(0)
{
}


RouteToTarget::~RouteToTarget()
{
}

void RouteToTarget::setCenterline(vtkPolyDataPtr centerline)
{
	mCLpoints = this->getCenterlinePositions(centerline);
}

Eigen::MatrixXd RouteToTarget::getCenterlinePositions(vtkPolyDataPtr centerline)
{

	int N = centerline->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);
	for(vtkIdType i = 0; i < N; i++)
		{
		double p[3];
		centerline->GetPoint(i,p);
		Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		CLpoints.block(0 , i , 3 , 1) = position;
		}
	return CLpoints;
}

void RouteToTarget::processCenterline(vtkPolyDataPtr centerline)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

	Eigen::MatrixXd CLpoints = getCenterlinePositions(centerline);

	mBranchListPtr->findBranchesInCenterline(CLpoints);
	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}


void RouteToTarget::findClosestPointInBranches(Vector3D targetCoordinate)
{

	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	std::vector<BranchPtr> branches = mBranchListPtr->getBranches();
	for (int i = 0; i < branches.size(); i++)
	{
		Eigen::MatrixXd positions = branches[i]->getPositions();
		for (int j = 0; j < positions.cols(); j++)
		{
			double D = findDistance(positions.col(j), targetCoordinate);
			if (D < minDistance)
			{
				minDistance = D;
				minDistanceBranch = branches[i];
				minDistancePositionIndex = j;
			}
		}
	}

		mProjectedBranchPtr = minDistanceBranch;
		mProjectedIndex = minDistancePositionIndex;
}


void RouteToTarget::findRoutePositions()
{
	mRoutePositions.clear();

	searchBranchUp(mProjectedBranchPtr, mProjectedIndex);
}

void RouteToTarget::searchBranchUp(BranchPtr searchBranchPtr, int startIndex)
{
	Eigen::MatrixXd positions = searchBranchPtr->getPositions();

	for (int i = startIndex; i>=0; i--)
		mRoutePositions.push_back(positions.col(i));

	BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();
	if (parentBranchPtr)
		searchBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1);
}


vtkPolyDataPtr RouteToTarget::findRouteToTarget(Vector3D targetCoordinate)
{

	findClosestPointInBranches(targetCoordinate);
	findRoutePositions();

	vtkPolyDataPtr retval = addVTKPoints();

	return retval;
}

vtkPolyDataPtr RouteToTarget::addVTKPoints()
{
	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr lines = vtkCellArrayPtr::New();
	for (int j = mRoutePositions.size() - 1; j >= 0; j--)
	{
		vtkIdType cells[1] = { points->GetNumberOfPoints() };
		points->InsertNextPoint(mRoutePositions[j](0),mRoutePositions[j](1),mRoutePositions[j](2));
		lines->InsertNextCell(1, cells);
	}
	retval->SetPoints(points);
	retval->SetVerts(lines);
	return retval;
}

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2)
{
	double d0 = p1(0) - p2(0);
	double d1 = p1(1) - p2(1);
	double d2 = p1(2) - p2(2);

	double D = sqrt( d0*d0 + d1*d1 + d2*d2 );

	return D;
}


} /* namespace cx */
