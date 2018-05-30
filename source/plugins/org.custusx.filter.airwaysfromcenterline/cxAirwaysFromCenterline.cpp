

#include "cxAirwaysFromCenterline.h"
#include <vtkPolyData.h>
//#include "cxDoubleDataAdapterXml.h"
#include "cxBranchList.h"
#include "cxBranch.h"
#include <vtkCellArray.h>
#include "vtkCardinalSpline.h"
#include <vtkLine.h>
#include <vtkCellArray.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkImplicitModeller.h>
#include <vtkContourFilter.h>

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{

AirwaysFromCenterline::AirwaysFromCenterline():
    mBranchListPtr(new BranchList)
{
}


AirwaysFromCenterline::~AirwaysFromCenterline()
{
}


Eigen::MatrixXd AirwaysFromCenterline::getCenterlinePositions(vtkPolyDataPtr centerline_r)
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

void AirwaysFromCenterline::processCenterline(vtkPolyDataPtr centerline_r)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

    Eigen::MatrixXd CLpoints_r = getCenterlinePositions(centerline_r);

    mBranchListPtr->findBranchesInCenterline(CLpoints_r);

	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();
    mBranchListPtr->smoothBranchPositions(40);

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
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
std::vector< Eigen::Vector3d > AirwaysFromCenterline::smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition)
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

vtkPolyDataPtr AirwaysFromCenterline::generateTubes()
{

    vtkPolyDataPtr output  = vtkPolyDataPtr::New();
    vtkLineSourcePtr lineSourcePtr = vtkLineSourcePtr::New();
    std::vector<BranchPtr> branches = mBranchListPtr->getBranches();

    for (int i = 0; i < branches.size(); i++)
    {
        Eigen::MatrixXd positions = branches[i]->getPositions();
        vtkPointsPtr pointsPtr = vtkPointsPtr::New();;
        pointsPtr->SetNumberOfPoints(positions.cols());

        for (int j = 0; j < positions.cols(); j++)
        {
            pointsPtr->SetPoint(j, positions(0,j), positions(1,j), positions(2,j));
        }
        lineSourcePtr->SetPoints(pointsPtr);

        // Create a tube (cylinder) around the line
         vtkSmartPointer<vtkTubeFilter> tubeFilterPtr = vtkSmartPointer<vtkTubeFilter>::New();
         tubeFilterPtr->SetInputConnection(lineSourcePtr->GetOutputPort());
         tubeFilterPtr->SetRadius(branches[i]->findBranchRadius()); //default is .5
         tubeFilterPtr->SetNumberOfSides(50);
         tubeFilterPtr->Update();

         //Append the new mesh
         vtkAppendPolyDataPtr appendFilterPtr = vtkSmartPointer<vtkAppendPolyData>::New();
         appendFilterPtr->AddInputData(output);
         appendFilterPtr->AddInputData(tubeFilterPtr->GetOutput());
         appendFilterPtr->Update();

         // Remove any duplicate points.
         vtkSmartPointer<vtkCleanPolyData> cleanFilterPtr = vtkSmartPointer<vtkCleanPolyData>::New();
         cleanFilterPtr->SetInputConnection(appendFilterPtr->GetOutputPort());
         cleanFilterPtr->Update();

         output = cleanFilterPtr->GetOutput();
    }

    // Create the implicit modeller
    vtkSmartPointer<vtkImplicitModeller> blobbyLogoImp =vtkSmartPointer<vtkImplicitModeller>::New();
    blobbyLogoImp->SetInputData(output);
    blobbyLogoImp->SetMaximumDistance(0.1);
    blobbyLogoImp->SetSampleDimensions(256, 256, 256);
    blobbyLogoImp->SetAdjustDistance(0.1);

    // Extract an iso surface, i.e. the tube shell
    vtkContourFilterPtr blobbyLogoIso = vtkSmartPointer<vtkContourFilter>::New();
    blobbyLogoIso->SetInputConnection(blobbyLogoImp->GetOutputPort());
    blobbyLogoIso->SetValue(1, 1.5); //orig
    blobbyLogoIso->Update();

    output = blobbyLogoIso->GetOutput();

    return output;
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
