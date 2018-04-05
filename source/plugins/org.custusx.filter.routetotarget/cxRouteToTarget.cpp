

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
    //mBranchListPtr->smoothBranchPositions(40);

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

/*
    RouteToTarget::searchBranchUp is finding all positions from a given index on a branch and up
    the airway tree to the top of trachea. All positions are added to mRoutePositions, which stores
    all positions along the route-to-target.
    Before the positions are added they are smoothed by RouteToTarget::smoothBranch.
*/
void RouteToTarget::searchBranchUp(BranchPtr searchBranchPtr, int startIndex)
{
	if (!searchBranchPtr)
		return;

    std::vector< Eigen::Vector3d > positions = smoothBranch(searchBranchPtr, startIndex, searchBranchPtr->getPositions().col(startIndex));

	for (int i = 0; i<=startIndex && i<positions.size(); i++)
        mRoutePositions.push_back(positions[i]);

    BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();
    if (parentBranchPtr)
        searchBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1);
}


vtkPolyDataPtr RouteToTarget::findRouteToTarget(Vector3D targetCoordinate_r)
{

    findClosestPointInBranches(targetCoordinate_r);
	findRoutePositions();

    //smoothPositions();

    vtkPolyDataPtr retval = addVTKPoints(mRoutePositions);

	return retval;
}

vtkPolyDataPtr RouteToTarget::findExtendedRoute(Vector3D targetCoordinate_r)
{
    float extentionPointIncrement = 0.5; //mm
    mExtendedRoutePositions.clear();
    mExtendedRoutePositions = mRoutePositions;
	if(mRoutePositions.size() > 0)
	{
		double extentionDistance = findDistance(mRoutePositions.front(),targetCoordinate_r);
		Eigen::Vector3d extentionVector = ( targetCoordinate_r - mRoutePositions.front() ) / extentionDistance;
		int numberOfextentionPoints = (int) extentionDistance * extentionPointIncrement;
		Eigen::Vector3d extentionPointIncrementVector = extentionVector / extentionPointIncrement;

		for (int i = 1; i<= numberOfextentionPoints; i++)
		{
			mExtendedRoutePositions.insert(mExtendedRoutePositions.begin(), mRoutePositions.front() + extentionPointIncrementVector*i);
			//std::cout << mRoutePositions.front() + extentionPointIncrementVector*i << std::endl;
		}
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

/*
    RouteToTarget::smoothBranch is smoothing the positions of a centerline branch by using vtkCardinalSpline.
    The degree of smoothing is dependent on the branch radius and the shape of the branch.
    First, the method tests if a straight line from start to end of the branch is sufficient by the condition of
    all positions along the line being within the lumen of the airway (max distance from original centerline
    is set to branch radius).
    If this fails, one more control point is added to the spline at the time, until the condition is fulfilled.
    The control point added for each iteration is the position with the larges deviation from the original/unfiltered
    centerline.
*/
std::vector< Eigen::Vector3d > RouteToTarget::smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition)
{
    vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

    double branchRadius = branchPtr->findBranchRadius();

    //add control points to spline

    //add first position
    Eigen::MatrixXd positions = branchPtr->getPositions();
    splineX->AddPoint(0,startPosition(0));
    splineY->AddPoint(0,startPosition(1));
    splineZ->AddPoint(0,startPosition(2));


    // Add last position if no parent branch, else add parents position closest to current branch.
    // Branch positions are stored in order from head to feet (e.g. first position is top of trachea),
    // while route-to-target is generated from target to top of trachea.
    if(!branchPtr->getParentBranch())
    {
        splineX->AddPoint(startIndex,positions(0,0));
        splineY->AddPoint(startIndex,positions(1,0));
        splineZ->AddPoint(startIndex,positions(2,0));
    }
    else
    {
        Eigen::MatrixXd parentPositions = branchPtr->getParentBranch()->getPositions();
        splineX->AddPoint(startIndex,parentPositions(0,parentPositions.cols()-1));
        splineY->AddPoint(startIndex,parentPositions(1,parentPositions.cols()-1));
        splineZ->AddPoint(startIndex,parentPositions(2,parentPositions.cols()-1));

    }

    //Add points until all filtered/smoothed postions are within branch radius distance of the unfiltered branch centerline posiition.
    //This is to make sure the smoothed centerline is within the lumen of the airways.
    double maxDistanceToOriginalPosition = branchRadius;
    int maxDistanceIndex = -1;
    std::vector< Eigen::Vector3d > smoothingResult;

    //add positions to spline
    while (maxDistanceToOriginalPosition >= branchRadius && splineX->GetNumberOfPoints() < startIndex)
    {
        if(maxDistanceIndex > 0)
        {
            //add to spline the position with largest distance to original position
            splineX->AddPoint(maxDistanceIndex,positions(0,startIndex - maxDistanceIndex));
            splineY->AddPoint(maxDistanceIndex,positions(1,startIndex - maxDistanceIndex));
            splineZ->AddPoint(maxDistanceIndex,positions(2,startIndex - maxDistanceIndex));
        }

		//evaluate spline - get smoothed positions      
        maxDistanceToOriginalPosition = 0.0;
        smoothingResult.clear();
        for(int j=0; j<=startIndex; j++)
		{
			double splineParameter = j;
			Eigen::Vector3d tempPoint;
			tempPoint(0) = splineX->Evaluate(splineParameter);
			tempPoint(1) = splineY->Evaluate(splineParameter);
			tempPoint(2) = splineZ->Evaluate(splineParameter);
			smoothingResult.push_back(tempPoint);

            //calculate distance to original (non-filtered) position
            double distance = findDistanceToLine(tempPoint, positions);
            //finding the index with largest distance
            if (distance > maxDistanceToOriginalPosition)
            {
                maxDistanceToOriginalPosition = distance;
                maxDistanceIndex = j;
            }
        }
    }

    return smoothingResult;
}

double findDistanceToLine(Eigen::MatrixXd point, Eigen::MatrixXd line)
{
    double minDistance = findDistance(point, line.col(0));
    for (int i=1; i<line.cols(); i++)
        if (minDistance > findDistance(point, line.col(i)))
            minDistance = findDistance(point, line.col(i));

    return minDistance;
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
