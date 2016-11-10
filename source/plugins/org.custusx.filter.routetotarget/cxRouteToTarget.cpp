

#include "cxRouteToTarget.h"
#include <vtkPolyData.h>
//#include "cxDoubleDataAdapterXml.h"
#include "cxBranchList.h"
#include "cxBranch.h"
#include <vtkCellArray.h>
#include "vtkCardinalSpline.h"

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

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

/*
void RouteToTarget::setCenterline(vtkPolyDataPtr centerline)
{
	mCLpoints = this->getCenterlinePositions(centerline);
}
*/

Eigen::MatrixXd RouteToTarget::getCenterlinePositions(vtkPolyDataPtr centerline_r)
{

    int N = centerline_r->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);
	for(vtkIdType i = 0; i < N; i++)
		{
		double p[3];
        centerline_r->GetPoint(i,p);
		Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		CLpoints.block(0 , i , 3 , 1) = position;
		}
	return CLpoints;
}

void RouteToTarget::processCenterline(vtkPolyDataPtr centerline_r)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

    Eigen::MatrixXd CLpoints_r = getCenterlinePositions(centerline_r);

    mBranchListPtr->findBranchesInCenterline(CLpoints_r);

	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}


void RouteToTarget::findClosestPointInBranches(Vector3D targetCoordinate_r)
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
            double D = findDistance(positions.col(j), targetCoordinate_r);
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


vtkPolyDataPtr RouteToTarget::findRouteToTarget(Vector3D targetCoordinate_r)
{

    findClosestPointInBranches(targetCoordinate_r);
	findRoutePositions();

	smoothPositions();

    vtkPolyDataPtr retval = addVTKPoints(mRoutePositions);

	return retval;
}

vtkPolyDataPtr RouteToTarget::findExtendedRoute(Vector3D targetCoordinate_r)
{
    float extentionPointIncrement = 0.5; //mm
    mExtendedRoutePositions.clear();
    mExtendedRoutePositions = mRoutePositions;
    double extentionDistance = findDistance(mRoutePositions.front(),targetCoordinate_r);
    Eigen::Vector3d extentionVector = ( targetCoordinate_r - mRoutePositions.front() ) / extentionDistance;
    int numberOfextentionPoints = (int) extentionDistance * extentionPointIncrement;
    Eigen::Vector3d extentionPointIncrementVector = extentionVector / extentionPointIncrement;

    for (int i = 1; i<= numberOfextentionPoints; i++)
    {
        mExtendedRoutePositions.insert(mExtendedRoutePositions.begin(), mRoutePositions.front() + extentionPointIncrementVector*i);
        std::cout << mRoutePositions.front() + extentionPointIncrementVector*i << std::endl;
    }

    vtkPolyDataPtr retval = addVTKPoints(mExtendedRoutePositions);

    return retval;
}

vtkPolyDataPtr RouteToTarget::addVTKPoints(std::vector<Eigen::Vector3d> positions)
{
	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr lines = vtkCellArrayPtr::New();
    int numberOfPositions = positions.size();
	for (int j = numberOfPositions - 1; j >= 0; j--)
	{
        points->InsertNextPoint(positions[j](0),positions[j](1),positions[j](2));
	}
	for (int j = 0; j < numberOfPositions-1; j++)
	{
		vtkIdType connection[2] = {j, j+1};
		lines->InsertNextCell(2, connection);
	}
	retval->SetPoints(points);
	retval->SetLines(lines);
	return retval;
}

void RouteToTarget::smoothPositions()
{
	int numberOfInputPoints = mRoutePositions.size();
	int controlPointFactor = 10;
	int numberOfControlPoints = numberOfInputPoints / controlPointFactor;

	vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

	if (numberOfControlPoints >= 2)
	{
		//add control points to spline
		for(int j=0; j<numberOfControlPoints; j++)
		{
			int indexP = (j*numberOfInputPoints)/numberOfControlPoints;
			splineX->AddPoint(indexP,mRoutePositions[indexP](0));
			splineY->AddPoint(indexP,mRoutePositions[indexP](1));
			splineZ->AddPoint(indexP,mRoutePositions[indexP](2));
		}
		//Always add the last point to complete spline
		splineX->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](0));
		splineY->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](1));
		splineZ->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](2));


		//evaluate spline - get smoothed positions
		std::vector< Eigen::Vector3d > smoothingResult;
		for(int j=0; j<numberOfInputPoints; j++)
		{
			double splineParameter = j;
			Eigen::Vector3d tempPoint;
			tempPoint(0) = splineX->Evaluate(splineParameter);
			tempPoint(1) = splineY->Evaluate(splineParameter);
			tempPoint(2) = splineZ->Evaluate(splineParameter);
			smoothingResult.push_back(tempPoint);
		}
		mRoutePositions.clear();
		mRoutePositions = smoothingResult;
	}
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
