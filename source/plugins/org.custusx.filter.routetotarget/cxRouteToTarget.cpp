

#include "cxRouteToTarget.h"
#include <vtkPolyData.h>
#include "cxBranchList.h"
#include "cxBranch.h"
#include "cxAirwaysFromCenterline.h"
#include "cxPointMetric.h"
#include <vtkCellArray.h>
#include "vtkCardinalSpline.h"
#include "vtkImageData.h"
#include <boost/math/special_functions/round.hpp>
#include "cxLogger.h"
#include <QDir>
#include "cxTime.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>

#define PI 3.1415926535897

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{

RouteToTarget::RouteToTarget():
	mBranchListPtr(new BranchList),
	mProjectedIndex(0),
	mBloodVesselBranchListPtr(new BranchList),
	mProjectedBloodVesselIndex(0)
{
}


RouteToTarget::~RouteToTarget()
{
}

void RouteToTarget::setBloodVesselVolume(ImagePtr bloodVesselVolume)
{
	mBloodVesselVolume = bloodVesselVolume;
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

void RouteToTarget::processCenterline(MeshPtr mesh)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

	vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());

	mCLpoints = getCenterlinePositions(centerline_r);

    mBranchListPtr->findBranchesInCenterline(mCLpoints);

	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();
    //mBranchListPtr->smoothBranchPositions(40);

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}

void RouteToTarget::processBloodVesselCenterline(Eigen::MatrixXd positions)
{
	if (mBloodVesselBranchListPtr)
		mBloodVesselBranchListPtr->deleteAllBranches();

	mBloodVesselBranchListPtr->findBranchesInCenterline(positions, false);

	mBloodVesselBranchListPtr->calculateOrientations();
	mBloodVesselBranchListPtr->smoothOrientations();
	mBloodVesselBranchListPtr->smoothBranchPositions(40);
	setBloodVesselRadius();

	std::cout << "Number of branches in CT blood vessel centerline: " << mBloodVesselBranchListPtr->getBranches().size() << std::endl;
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

void RouteToTarget::findClosestPointInBloodVesselBranches(Vector3D targetCoordinate_r)
{
	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	std::vector<BranchPtr> branches = mBloodVesselBranchListPtr->getBranches();
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

		mProjectedBloodVesselBranchPtr = minDistanceBranch;
		mProjectedBloodVesselIndex = minDistancePositionIndex;
}


void RouteToTarget::findRoutePositions()
{
	mRoutePositions.clear();

	searchBranchUp(mProjectedBranchPtr, mProjectedIndex);
}

void RouteToTarget::findRoutePositionsInBloodVessels()
{
	mBloodVesselRoutePositions.clear();

	searchBloodVesselBranchUp(mProjectedBloodVesselBranchPtr, mProjectedBloodVesselIndex);
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

	//std::vector< Eigen::Vector3d > positions = smoothBranch(searchBranchPtr, startIndex, searchBranchPtr->getPositions().col(startIndex));
	std::vector< Eigen::Vector3d > positions = getBranchPositions(searchBranchPtr, startIndex);

	for (int i = 0; i<=startIndex && i<positions.size(); i++)
		mRoutePositions.push_back(positions[i]);

	mBranchingIndex.push_back(mRoutePositions.size()-1);

	BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();
	if (parentBranchPtr)
		searchBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1);
}

void RouteToTarget::searchBloodVesselBranchUp(BranchPtr searchBranchPtr, int startIndex)
{
	if (!searchBranchPtr)
		return;

	//std::vector< Eigen::Vector3d > positions = smoothBranch(searchBranchPtr, startIndex, searchBranchPtr->getPositions().col(startIndex));
	std::vector< Eigen::Vector3d > positions = getBranchPositions(searchBranchPtr, startIndex);

	for (int i = 0; i<=startIndex && i<positions.size(); i++)
		mBloodVesselRoutePositions.push_back(positions[i]);

	//mBranchingIndex.push_back(mRoutePositions.size()-1);

	BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();
	if (parentBranchPtr)
		searchBloodVesselBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1);
}


vtkPolyDataPtr RouteToTarget::findRouteToTarget(PointMetricPtr targetPoint)
{
	mTargetPosition = targetPoint->getCoordinate();

	findClosestPointInBranches(mTargetPosition);
	findRoutePositions();

	vtkPolyDataPtr retval = addVTKPoints(mRoutePositions);

	return retval;
}


vtkPolyDataPtr RouteToTarget::findExtendedRoute(PointMetricPtr targetPoint)
{
	mTargetPosition = targetPoint->getCoordinate();
    double extentionPointIncrement = 0.25; //mm
    mExtendedRoutePositions.clear();
    mExtendedRoutePositions = mRoutePositions;
	if(mRoutePositions.size() > 0)
	{
		double extentionDistance = findDistance(mRoutePositions.front(),mTargetPosition);
		Eigen::Vector3d extentionVectorNormalized = ( mTargetPosition - mRoutePositions.front() ) / extentionDistance;
        int numberOfextentionPoints = int(extentionDistance / extentionPointIncrement);
        Eigen::Vector3d extentionPointIncrementVector = extentionVectorNormalized * extentionDistance / numberOfextentionPoints;

		for (int i = 1; i<= numberOfextentionPoints; i++)
		{
			mExtendedRoutePositions.insert(mExtendedRoutePositions.begin(), mRoutePositions.front() + extentionPointIncrementVector*i);
		}
	}

    vtkPolyDataPtr retval = addVTKPoints(mExtendedRoutePositions);
    return retval;
}


vtkPolyDataPtr RouteToTarget::findRouteToTargetAlongBloodVesselCenterlines(MeshPtr bloodVesselCenterlineMesh, PointMetricPtr targetPoint)
{
	vtkPolyDataPtr retval;
	//mBloodVesselBranchListPtr->deleteAllBranches();

//	if ( !checkIfRouteToTargetEndsAtEndOfLastBranch() )
//	{
//		CX_LOG_INFO() << "No improved route to target found along blood vessels";
//		return retval;
//	}

	vtkPolyDataPtr BVcenterline_r = bloodVesselCenterlineMesh->getTransformedPolyDataCopy(bloodVesselCenterlineMesh->get_rMd());
	Eigen::MatrixXd BVCLpoints_r = getCenterlinePositions(BVcenterline_r);
	mConnectedPointsInBVCL = findClosestBloodVesselSegments(BVCLpoints_r , mCLpoints, targetPoint->getCoordinate());

	//Eigen::MatrixXd::Index closestBVCLIndex = dsearch(targetPoint->getCoordinate(), BVCLpointsFiltered_r).first;
	//Eigen::MatrixXd::Index closestBVCLIndex = dsearch(mRoutePositions[0], BVCLpointsFiltered_r).first;

	//mConnectedPointsInBVCL = findLocalPointsInCT(closestBVCLIndex , BVCLpointsFiltered_r).first;
	std::cout << "Number of connected points: " << mConnectedPointsInBVCL.cols() << std::endl; //debug

	if (mRoutePositions.empty())
		return retval;

	//Eigen::MatrixXd::Index closestPositionToEndOfAirwayRTTIndex = dsearch(mRoutePositions[0], mConnectedPointsInBVCL).first;
	Eigen::MatrixXd::Index closestPositionToEndOfAirwayRTTIndex = dsearch(mRoutePositions[0], mConnectedPointsInBVCL).first;

	//setting position closest to RTT from airways in first index, where RTT should  continue
	mConnectedPointsInBVCL.col(mConnectedPointsInBVCL.cols()-1).swap(mConnectedPointsInBVCL.col(closestPositionToEndOfAirwayRTTIndex));

	//mBloodVesselBranchListPtr->findBranchesInCenterline(mConnectedPointsInBVCL, false);
	processBloodVesselCenterline(mConnectedPointsInBVCL);

	findClosestPointInBloodVesselBranches(mTargetPosition);
	findRoutePositionsInBloodVessels();
	mPathToBloodVesselsFound = makeConnectedAirwayAndBloodVesselRoute();

	retval = addVTKPoints(mBloodVesselRoutePositions);

	return retval;
}

vtkPolyDataPtr RouteToTarget::generateAirwaysFromBloodVesselCenterlines()
{
	vtkPolyDataPtr airwayMesh;
	if (mConnectedPointsInBVCL.cols() == 0 | !mPathToBloodVesselsFound)
		return airwayMesh;

	AirwaysFromCenterlinePtr airwaysFromBVCenterlinePtr = AirwaysFromCenterlinePtr(new AirwaysFromCenterline());
	airwaysFromBVCenterlinePtr->setTypeToBloodVessel(true);
	//airwaysFromBVCenterlinePtr->processCenterline(mConnectedPointsInBVCL);
	mBloodVesselBranchListPtr->interpolateBranchPositions(5);
	airwaysFromBVCenterlinePtr->setBranches(mBloodVesselBranchListPtr);

	airwayMesh = airwaysFromBVCenterlinePtr->generateTubes(2);

	return airwayMesh;
}

bool RouteToTarget::makeConnectedAirwayAndBloodVesselRoute()
{
	vtkPolyDataPtr mergedRoute;

	if ( mRoutePositions.empty() )
			return false;

	if ( mBloodVesselRoutePositions.empty() )
	{
		mMergedAirwayAndBloodVesselRoutePositions = mRoutePositions;
		return false;
	}

	if ( findDistance(mRoutePositions.front(),mTargetPosition) < findDistance(mBloodVesselRoutePositions.front(),mTargetPosition) )
	{
		CX_LOG_INFO() << "No improved route to target found along blood vessels";
		mMergedAirwayAndBloodVesselRoutePositions = mRoutePositions; // do not add blood vessel route if that is not leading closer to target than airway route alone
		return false;
	}

	std::vector<BranchPtr> branches = mBranchListPtr->getBranches();
	double minDistance = findDistanceToLine(mRoutePositions[0], mBloodVesselRoutePositions);
	bool closerAirwayFound = false;
	Vector3D closestPosition;
	for (int i = 0; i<branches.size(); i++) //find closest airway branch to blood vessel route
	{
		if ( branches[i]->findGenerationNumber() > 2 )//getChildBranches().empty()) //Needs to be end branch (no children)
		{
			Eigen::MatrixXd branchPositinos = branches[i]->getPositions();
			for (int j = 0; j<branchPositinos.cols(); j++)
			{
				double distance = findDistanceToLine(branchPositinos.col(j), mBloodVesselRoutePositions);
				if (minDistance > distance)
				{
					minDistance = distance;
					closestPosition = branchPositinos.col(j);
					closerAirwayFound = true;
					mProjectedBranchPtr = branches[i];
					mProjectedIndex = j;
				}
			}
		}
	}

	if (closerAirwayFound) //calculating new route
	{
		Eigen::MatrixXd::Index closestPositionToEndOfAirwayRTTIndex = dsearch(closestPosition, mConnectedPointsInBVCL).first;
		mConnectedPointsInBVCL.col(mConnectedPointsInBVCL.cols()-1).swap(mConnectedPointsInBVCL.col(closestPositionToEndOfAirwayRTTIndex));
		processBloodVesselCenterline(mConnectedPointsInBVCL);
		findClosestPointInBloodVesselBranches(mTargetPosition);
		findRoutePositionsInBloodVessels();

		findClosestPointInBranches(closestPosition);
		findRoutePositions();
	}

	//Checking for wrong direction of blood vessel route compared to airway route - to get correct connection
	Eigen::MatrixXd directionLastRTTBranch = mProjectedBranchPtr->getOrientations().rowwise().mean();
	BranchListPtr BVRTTbranchList = BranchListPtr(new BranchList());
	BranchPtr BVRTTbranch = BranchPtr(new Branch());
	BVRTTbranch->setPositions(convertToEigenMatrix(mBloodVesselRoutePositions));
	BVRTTbranchList->addBranch(BVRTTbranch);
	BVRTTbranchList->calculateOrientations();
	BVRTTbranchList->smoothOrientations();
	Eigen::MatrixXd BRRTTorientations = BVRTTbranch->getOrientations();
	bool wrongDirectionOfBloodVesselFound = false;
	int indexOfWrongDirection;
	double maxAngle = 110;
	std::cout << "directionLastRTTBranch: " << directionLastRTTBranch << std::endl; //debug
	std::cout << "cos(maxAngle*PI/180.0): " << cos(maxAngle*PI/180.0) << std::endl; //debug
	for (int i = mBloodVesselRoutePositions.size()-1; i > 0; i--)
	{
		double orientationDiffNorm = (BRRTTorientations.col(i)*directionLastRTTBranch).sum() / (BRRTTorientations.col(i).norm()*directionLastRTTBranch.norm());
		//double orientationDiffNorm = orientationDiff.sum()
		if ( orientationDiffNorm < cos(maxAngle*PI/180.0) )
		{
			wrongDirectionOfBloodVesselFound = true;
			indexOfWrongDirection = i;
			std::cout << "indexOfWrongDirection: " << i << " - orientation: " << BRRTTorientations.col(i) << " - orientationDiffNorm: " << orientationDiffNorm << std::endl; //debug
		}
		else
			break;
	}

	if (wrongDirectionOfBloodVesselFound)
	{
		mBloodVesselRoutePositions.erase(mBloodVesselRoutePositions.begin() + indexOfWrongDirection, mBloodVesselRoutePositions.end());
		findClosestPointInBranches(mBloodVesselRoutePositions.back());
		findRoutePositions();
	}

	std::vector< Eigen::Vector3d > mergedPositions = mBloodVesselRoutePositions;

	mergedPositions.insert( mergedPositions.end(), mRoutePositions.begin(), mRoutePositions.end() );

	double extentionPointIncrement = 0.25; //mm
	double extentionDistance = findDistance(mergedPositions.front(),mTargetPosition);
	Eigen::Vector3d extentionVectorNormalized = ( mTargetPosition - mergedPositions.front() ) / extentionDistance;
	int numberOfextentionPoints = int(extentionDistance / extentionPointIncrement);
	Eigen::Vector3d extentionPointIncrementVector = extentionVectorNormalized * extentionDistance / numberOfextentionPoints;

	for (int i = 1; i<= numberOfextentionPoints; i++)
	{
		mergedPositions.insert(mergedPositions.begin(), mBloodVesselRoutePositions.front() + extentionPointIncrementVector*i);
	}

	mMergedAirwayAndBloodVesselRoutePositions = mergedPositions;

	return true;
}

vtkPolyDataPtr RouteToTarget::getConnectedAirwayAndBloodVesselRoute()
{
	return addVTKPoints(mMergedAirwayAndBloodVesselRoutePositions);
}


bool RouteToTarget::checkIfRouteToTargetEndsAtEndOfLastBranch() // remove if not in use?
{
	if (!mProjectedBranchPtr)
		return false;

	if (!mProjectedIndex)
		return false;

	if (mProjectedBranchPtr->getChildBranches().empty())
		if (mProjectedBranchPtr->getPositions().cols()-1 == mProjectedIndex)
			return true;

	return false;
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
    RouteToTarget::getBranchPositions is used to get positions of a branch without smoothing.
    Equivalent to RouteToTarget::smoothBranch without smoothing.
*/
std::vector< Eigen::Vector3d > RouteToTarget::getBranchPositions(BranchPtr branchPtr, int startIndex)
{
    Eigen::MatrixXd branchPositions = branchPtr->getPositions();
    std::vector< Eigen::Vector3d > positions;

    for (int i = startIndex; i >=0; i--)
        positions.push_back(branchPositions.col(i));

    return positions;
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

    //Add points until all filtered/smoothed positions are minimum 1 mm inside the airway wall, (within r - 1 mm).
    //This is to make sure the smoothed centerline is within the lumen of the airways.
    double maxAcceptedDistanceToOriginalPosition = std::max(branchRadius - 1, 1.0);
    double maxDistanceToOriginalPosition = maxAcceptedDistanceToOriginalPosition + 1;
    int maxDistanceIndex = -1;
    std::vector< Eigen::Vector3d > smoothingResult;

    //add positions to spline
    while (maxDistanceToOriginalPosition >= maxAcceptedDistanceToOriginalPosition && splineX->GetNumberOfPoints() < startIndex)
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
            double distance = dsearch(tempPoint, positions).second;
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

void RouteToTarget::addRouteInformationToFile(VisServicesPtr services)
{
    QString RTTpath = services->patient()->getActivePatientFolder() + "/RouteToTargetInformation/";
    QDir RTTDirectory(RTTpath);
    if (!RTTDirectory.exists()) // Creating RouteToTargetInformation folder if it does not exist
        RTTDirectory.mkpath(RTTpath);

    QString format = timestampSecondsFormat();
    QString filePath = RTTpath + QDateTime::currentDateTime().toString(format) + "_RouteToTargetInformation.txt";

    QFile outfile(filePath);
    if (outfile.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&outfile);

        stream << "#Target position:" << endl;
        stream << mTargetPosition(0) << " " << mTargetPosition(1) << " " << mTargetPosition(2) << " " << endl;
        if (mProjectedBranchPtr)
        {
            stream << "#Route to target generations:" << endl;
            stream << mProjectedBranchPtr->findGenerationNumber() << endl;
        }

        stream << "#Trachea length (mm):" << endl;
        BranchPtr trachea = mBranchListPtr->getBranches()[0];
        int numberOfPositionsInTrachea = trachea->getPositions().cols();
        double tracheaLength = calculateRouteLength(smoothBranch(trachea, numberOfPositionsInTrachea-1, trachea->getPositions().col(numberOfPositionsInTrachea-1)));
        stream << tracheaLength << endl;

        stream << "#Route to target length - from Carina (mm):" << endl;
        stream << calculateRouteLength(mRoutePositions) - tracheaLength << endl;
        stream << "#Extended route to target length - from Carina (mm):" << endl;
        stream << calculateRouteLength(mExtendedRoutePositions) - tracheaLength << endl;
    }
}

double RouteToTarget::calculateRouteLength(std::vector< Eigen::Vector3d > route)
{
    double routeLenght = 0;
    for (int i=0; i<route.size()-1; i++)
    {
        double d0 = route[i+1](0) - route[i](0);
        double d1 = route[i+1](1) - route[i](1);
        double d2 = route[i+1](2) - route[i](2);

        routeLenght += sqrt( d0*d0 +d1*d1 + d2*d2 );
    }

    return routeLenght;
}

void RouteToTarget::setBloodVesselRadius()
{
	std::vector<BranchPtr> branches = mBloodVesselBranchListPtr->getBranches();

	for (int i = 0; i < branches.size(); i++)
		{
			Eigen::MatrixXd positions = branches[i]->getPositions();
			Eigen::MatrixXd orientations = branches[i]->getOrientations();
			Eigen::VectorXd radius(positions.cols());

			for (int j = 0; j < positions.cols(); j++)
			{
				radius(j) = calculateBloodVesselRadius(positions.col(j), orientations.col(j));
				std::cout << "Calculating radius: " << radius(j) << std::endl; //debug
			}

			branches[i]->setRadius(radius);
		}

}

double RouteToTarget::calculateBloodVesselRadius(Eigen::Vector3d position, Eigen::Vector3d orientation)
{
	double radius = 0;
	if (!mBloodVesselVolume)
		return radius;

	vtkImageDataPtr bloodVesselImage = mBloodVesselVolume->getBaseVtkImageData();
	int* dim = bloodVesselImage->GetDimensions();
	double* spacing = bloodVesselImage->GetSpacing();
	int x = (int) boost::math::round(position[0]/spacing[0]);
	int y = (int) boost::math::round(position[1]/spacing[1]);
	int z = (int) boost::math::round(position[2]/spacing[2]);

	//std::cout << "x position: " << position[0] << " - y position: " << position[1] << " - z position: " << position[2] << std::endl; //debug
	//std::cout << "x index: " << x << " - y index: " << y << " - z index: " << z << std::endl; //debug

	double maxValue = bloodVesselImage->GetScalarRange()[1];
	//bool maxRadiusReached = false;
	int radiusVoxels = 1;
	while (radiusVoxels < 50) // Maximum radius - to avoid infinite loop
	{
		Eigen::Vector3d prependicularX = crossproduct(orientation, Eigen::Vector3d::UnitX());
		if (prependicularX.sum() != 0)
		{
			Eigen::Vector3d searchDirection =  prependicularX.normalized() * radiusVoxels;
			int xIndex = std::max(std::min(x + (int) std::round(searchDirection(0)), dim[0]-1), 0);
			int yIndex = std::max(std::min(y + (int) std::round(searchDirection(1)), dim[1]-1), 0);
			int zIndex = std::max(std::min(z + (int) std::round(searchDirection(2)), dim[2]-1), 0);
			if (bloodVesselImage->GetScalarComponentAsDouble(xIndex, yIndex, zIndex, 0) < maxValue)
			{
				radius = std::sqrt( std::pow(searchDirection(0)*spacing[0],2) + std::pow(searchDirection(1)*spacing[1],2) + std::pow(searchDirection(2)*spacing[2],2) );
				break;
			}
		}

		Eigen::Vector3d prependicularY = crossproduct(orientation, Eigen::Vector3d::UnitY());
		if (prependicularY.sum() != 0)
		{
			Eigen::Vector3d searchDirection =  prependicularY.normalized() * radiusVoxels;
			int xIndex = std::max(std::min(x + (int) std::round(searchDirection(0)), dim[0]-1), 0);
			int yIndex = std::max(std::min(y + (int) std::round(searchDirection(1)), dim[1]-1), 0);
			int zIndex = std::max(std::min(z + (int) std::round(searchDirection(2)), dim[2]-1), 0);
			if (bloodVesselImage->GetScalarComponentAsDouble(xIndex, yIndex, zIndex, 0) < maxValue)
			{
				radius = std::sqrt( std::pow(searchDirection(0)*spacing[0],2) + std::pow(searchDirection(1)*spacing[1],2) + std::pow(searchDirection(2)*spacing[2],2) );
				break;
			}
		}

		Eigen::Vector3d prependicularZ = crossproduct(orientation, Eigen::Vector3d::UnitZ());
		if (prependicularZ.sum() != 0)
		{
			Eigen::Vector3d searchDirection =  prependicularZ.normalized() * radiusVoxels;
			int xIndex = std::max(std::min(x + (int) std::round(searchDirection(0)), dim[0]-1), 0);
			int yIndex = std::max(std::min(y + (int) std::round(searchDirection(1)), dim[1]-1), 0);
			int zIndex = std::max(std::min(z + (int) std::round(searchDirection(2)), dim[2]-1), 0);
			if (bloodVesselImage->GetScalarComponentAsDouble(xIndex, yIndex, zIndex, 0) < maxValue)
			{
				radius = std::sqrt( std::pow(searchDirection(0)*spacing[0],2) + std::pow(searchDirection(1)*spacing[1],2) + std::pow(searchDirection(2)*spacing[2],2) );
				break;
			}
		}

		radiusVoxels += 1;
	}


	return radius;
}

void RouteToTarget::makeMarianaCenterlineFile(QString filename)
{
	if (mExtendedRoutePositions.empty())
	{
			std::cout << "mExtendedRoutePositions is empty." << std::endl;
			return;
	}

	int numberOfExtendedPositions = mExtendedRoutePositions.size() - mRoutePositions.size();

	ofstream out(filename.toStdString().c_str());
	out << "# [xPos yPos zPos BranchingPoint (0=Normal, 1=Branching position, 2=Extended from airway to target)] ";
	out << "Number of positions: ";
	out << mExtendedRoutePositions.size(); // write number of positions
	out << "\n";

	 for (int i = 1; i < mExtendedRoutePositions.size(); i++)
	 {
			out <<  mExtendedRoutePositions[i](0) << " "; // write x coordinate
			out <<  mExtendedRoutePositions[i](1) << " "; // write y coordinate
			out <<  mExtendedRoutePositions[i](2) << " "; // write z coordinate

			if ( std::find(mBranchingIndex.begin(), mBranchingIndex.end(), i - numberOfExtendedPositions) != mBranchingIndex.end() )
					out <<  "1 ";
			else if (i <= numberOfExtendedPositions)
				out <<  "2 ";
			else
					out <<  "0 ";

			out << "\n";
	 }

	 out.close();
}

QJsonArray RouteToTarget::makeMarianaCenterlineJSON()
{
	QJsonArray array;
	if ( mRoutePositions.empty() || mExtendedRoutePositions.empty() )
	{
			std::cout << "mRoutePositions is empty." << std::endl;
			return array;
	}

	int numberOfExtendedPositions = mExtendedRoutePositions.size() - mRoutePositions.size();

	for (int i = 1; i < mExtendedRoutePositions.size(); i++)
	{
		QJsonObject position;
		position.insert( "x", mExtendedRoutePositions[i](0) );
		position.insert( "y", mExtendedRoutePositions[i](1) );
		position.insert( "z", mExtendedRoutePositions[i](2) );

		if ( std::find(mBranchingIndex.begin(), mBranchingIndex.end(), i - numberOfExtendedPositions) != mBranchingIndex.end() )
			position.insert("Flag", 1);
		else if (i <= numberOfExtendedPositions)
			position.insert("Flag", 2);
		else
			position.insert("Flag", 0);

		array.append(position);
	 }

	 return array;
}

// to be delete
//Eigen::MatrixXd removeSmallAndPeripheralBloodVesselSegments(Eigen::MatrixXd bloodVesselPositions , Eigen::MatrixXd airwayPositions)
//{
//	double maxDistanceToAirway = 10; //mm
//	int minNumberOfPositionsInSegment = 100;
//
//	Eigen::MatrixXd filteredBloodVesselPositions;
//
//	while (bloodVesselPositions.cols() > minNumberOfPositionsInSegment)
//	{
//		std::cout << "bloodVesselPositions.cols(): " << bloodVesselPositions.cols() << std::endl;//debug
//		std::pair< Eigen::MatrixXd, Eigen::MatrixXd > localPositions = findLocalPointsInCT(0 , bloodVesselPositions);
//		bloodVesselPositions = localPositions.second;
//
//		if ( localPositions.first.cols() >= minNumberOfPositionsInSegment &&
//				dsearchn(airwayPositions, localPositions.first).second.minCoeff() <= maxDistanceToAirway )
//		{
//			filteredBloodVesselPositions.conservativeResize(3, filteredBloodVesselPositions.cols() + localPositions.first.cols());
//			filteredBloodVesselPositions.rightCols(localPositions.first.cols()) = localPositions.first;
//			std::cout << "filteredBloodVesselPositions.cols(): " << filteredBloodVesselPositions.cols() << std::endl;//debug
//		}
//	}
//
//	return filteredBloodVesselPositions;
//}

Eigen::MatrixXd findClosestBloodVesselSegments(Eigen::MatrixXd bloodVesselPositions , Eigen::MatrixXd airwayPositions, Vector3D targetPosition)
{
	double maxDistanceToAirway = 10; //mm
	int minNumberOfPositionsInSegment = 100; //to avoid small segments which are probably not true blood vessels

	Eigen::MatrixXd bloodVesselSegment;

	while (bloodVesselPositions.cols() > minNumberOfPositionsInSegment)
	{
		std::cout << "bloodVesselPositions.cols(): " << bloodVesselPositions.cols() << std::endl;//debug
		Eigen::MatrixXd::Index closestBloodVesselPositionToTarget = dsearch(targetPosition, bloodVesselPositions).first;
		std::pair< Eigen::MatrixXd, Eigen::MatrixXd > localPositions = findLocalPointsInCT(closestBloodVesselPositionToTarget , bloodVesselPositions);
		bloodVesselPositions = localPositions.second;

		if ( localPositions.first.cols() >= minNumberOfPositionsInSegment &&
				dsearchn(airwayPositions, localPositions.first).second.minCoeff() <= maxDistanceToAirway )
		{
			bloodVesselSegment = localPositions.first;
			break;
		}
	}

	return bloodVesselSegment;
}

std::pair< Eigen::MatrixXd, Eigen::MatrixXd > findLocalPointsInCT(int closestCLIndex , Eigen::MatrixXd CLpositions)
{
	Eigen::MatrixXd includedPositions;
	Eigen::MatrixXd positionsNotIncluded = CLpositions;
	int startIndex = closestCLIndex;

	bool closePositionFound = true;
	while (closePositionFound)
	{
		closePositionFound = false;
		std::pair<Eigen::MatrixXd,Eigen::MatrixXd> connectedPoints = findConnectedPointsInCT(startIndex , positionsNotIncluded);
		positionsNotIncluded = connectedPoints.second;

		if (includedPositions.cols() > 0)
		{
			includedPositions.conservativeResize(Eigen::NoChange, includedPositions.cols() + connectedPoints.first.cols());
			includedPositions.rightCols(connectedPoints.first.cols()) = connectedPoints.first;
		}
		else
			includedPositions = connectedPoints.first;

		for (int i = 0; i < includedPositions.cols(); i++)
		{
			std::pair<Eigen::MatrixXd::Index, double> closePositionSearch = dsearch(includedPositions.col(i), positionsNotIncluded);
			if (closePositionSearch.second < 3) //Invlude positions closer than 3 mm
			{
				closePositionFound = true;
				startIndex = closePositionSearch.first;
				break;
			}
		}
	}

	return std::make_pair(includedPositions, positionsNotIncluded);
}

double findDistanceToLine(Eigen::MatrixXd point, std::vector< Eigen::Vector3d > line)
{
    double minDistance = findDistance(point, line[0]);
    for (int i=1; i<line.size(); i++)
        if (minDistance > findDistance(point, line[i]))
            minDistance = findDistance(point, line[i]);

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

Eigen::MatrixXd convertToEigenMatrix(std::vector< Eigen::Vector3d > positionsVector)
{
	Eigen::MatrixXd positionsMatrix(3, positionsVector.size());
	for (int i = 0; i < positionsVector.size(); i++)
	{
		positionsMatrix(0, i) = positionsVector[i](0);
		positionsMatrix(1, i) = positionsVector[i](1);
		positionsMatrix(2, i) = positionsVector[i](2);
	}
	return positionsMatrix;
}

Eigen::Vector3d crossproduct(Eigen::Vector3d A, Eigen::Vector3d B)
{
	Eigen::Vector3d R;
	R(0) = A(1)*B(2)-A(2)*B(1);
	R(1) = A(2)*B(0)-A(0)*B(2);
	R(2) = A(0)*B(1)-A(1)*B(0);
	return R;
}

} /* namespace cx */
