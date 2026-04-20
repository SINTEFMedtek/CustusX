

#include "cxRouteToTarget.h"
#include <iostream>
#include <vtkPolyData.h>
#include "cxBranchList.h"
#include "cxBranch.h"
#include "cxAirwaysFromCenterline.h"
#include "cxPointMetric.h"
#include <vtkCellArray.h>
#include "vtkCardinalSpline.h"
#include "vtkImageData.h"
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

void RouteToTarget::setSmoothing(bool smoothing)
{
	mSmoothing = smoothing; // default true
}

void RouteToTarget::processCenterline(MeshPtr mesh)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

	vtkPolyDataPtr centerline_r = mesh->getTransformedPolyDataCopy(mesh->get_rMd());

	mCLpoints = getCenterlinePositions(centerline_r);

	mBranchListPtr->findBranchesInCenterline(mCLpoints);

    mBranchListPtr->smoothOrientations();
	//mBranchListPtr->smoothBranchPositions();
	mBranchListPtr->findBronchoscopeRotation();

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}

//Can be used instead of processCenterline if you wan to use an existing branchList.
void RouteToTarget::setBranchList(BranchListPtr branchList)
{
	mBranchListPtr = branchList;
}

BranchListPtr RouteToTarget::getBranchList()
{
	return mBranchListPtr;
}

void RouteToTarget::processBloodVesselCenterline(Eigen::MatrixXd positions)
{
	if (mBloodVesselBranchListPtr)
		mBloodVesselBranchListPtr->deleteAllBranches();

	mBloodVesselBranchListPtr->findBranchesInCenterline(positions, false);

	mBloodVesselBranchListPtr->smoothOrientations();
	mBloodVesselBranchListPtr->smoothBranchPositions();
	mBloodVesselBranchListPtr->setRadius(mBloodVesselVolume);
	mBloodVesselBranchListPtr->smoothRadius();

	BranchPtr branchWithLargestRadius = mBloodVesselBranchListPtr->findBranchWithLargestRadius();

	if (branchWithLargestRadius->getParentBranch()) //If the largest branch has a parent, something is wrong. -> Reprocess with largest branch as root.
	{
		Eigen::MatrixXd positions = branchWithLargestRadius->getPositions();
		Eigen::VectorXd radius = branchWithLargestRadius->getRadius();
		int maxRadiusIndex;
		radius.maxCoeff(&maxRadiusIndex);
		positions.col(positions.cols()-1).swap(positions.col(maxRadiusIndex)); //setting largest radius position last (which is processed first)

		std::vector<BranchPtr> allBranches = mBloodVesselBranchListPtr->getBranches();
		for (int i = 1; i < allBranches.size(); i++)
			if (allBranches[i] != branchWithLargestRadius) //add positions from all other branches
			{
				Eigen::MatrixXd positionsToAppend = allBranches[i]->getPositions();
				Eigen::MatrixXd resizedPositions(positions.rows(), positions.cols() + positionsToAppend.cols());
				resizedPositions.rightCols(positions.cols()) = positions;
				resizedPositions.leftCols(positionsToAppend.cols()) = positionsToAppend;
				positions = resizedPositions;
			}

		mBloodVesselBranchListPtr->deleteAllBranches();

		mBloodVesselBranchListPtr->findBranchesInCenterline(positions, false, false);
		mBloodVesselBranchListPtr->smoothOrientations();
		mBloodVesselBranchListPtr->smoothBranchPositions();
		mBloodVesselBranchListPtr->setRadius(mBloodVesselVolume);
		mBloodVesselBranchListPtr->smoothRadius();
	}


	CX_LOG_INFO() << "Number of branches in CT blood vessel centerline: " << mBloodVesselBranchListPtr->getBranches().size();
}

void RouteToTarget::findClosestPointInBranches(Vector3D targetCoordinate_r)
{
	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	std::vector<BranchPtr> branches = mBranchListPtr->getBranches();
	for (int i = 1; i < branches.size(); i++) //starting at i=1, not including Trachea (0)
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


void RouteToTarget::findRoutePositions(std::vector<Eigen::Vector3d> initialRoutePositions)
{
	mRoutePositions.clear();
	if(!initialRoutePositions.empty())
	{
		std::vector< Eigen::Vector3d > branchPositions = getBranchPositions(mProjectedBranchPtr,mProjectedIndex);
		if(!branchPositions.empty())
			initialRoutePositions = insertAndinterpolate(initialRoutePositions, branchPositions.front(), 0.25);
	}

	mRoutePositions = initialRoutePositions;

	double cameraRotation = mProjectedBranchPtr->getBronchoscopeRotation();
	std::vector< double > initialRouteRotations(mRoutePositions.size(), cameraRotation);
	mCameraRotation = initialRouteRotations;

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

	std::vector< Eigen::Vector3d > positions;
	if (mSmoothing)
		 positions = smoothBranch(searchBranchPtr, startIndex, searchBranchPtr->getPositions().col(startIndex));
	else
		positions = getBranchPositions(searchBranchPtr, startIndex);

    double cameraRotation = searchBranchPtr->getBronchoscopeRotation();

	for (int i = 0; i<=startIndex && i<positions.size(); i++)
	{
		mRoutePositions.push_back(positions[i]);
		mRoutePositionsBranch.push_back(searchBranchPtr);
		mCameraRotation.push_back(cameraRotation);
		mGenerationNumber.push_back(searchBranchPtr->findGenerationNumber());
		Eigen::VectorXd radius = searchBranchPtr->getRadius();

		if(radius.size() > i)
			mRadius.push_back(radius(i));
		else
			mRadius.push_back(0.0);
	}

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
	{
		mBloodVesselRoutePositions.push_back(positions[i]);
	}

	BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();
	if (parentBranchPtr)
	{
		if(parentBranchPtr->getAverageRadius() >= searchBranchPtr->getAverageRadius() && variance(parentBranchPtr->getRadius()) < 1.0 )
		{
			searchBloodVesselBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1);
		}
	}
}


vtkPolyDataPtr RouteToTarget::findRouteToTarget(PointMetricPtr targetPoint, std::map<QString, PointMetricPtr> extraAirwayPoints)
{
	mTargetPosition = targetPoint->getCoordinate();
	Vector3D startPosition;
	std::vector<Eigen::Vector3d> initialRoutePositions;
	if(!extraAirwayPoints.empty())
	{
		initialRoutePositions = findRouteAlongExtraPoints(mTargetPosition, extraAirwayPoints);
		startPosition = initialRoutePositions.back();
	}
		else
		startPosition = mTargetPosition;

	findClosestPointInBranches(startPosition);
	findRoutePositions(initialRoutePositions);

	vtkPolyDataPtr retval = addVTKPoints(mRoutePositions);

	return retval;
}

std::vector<Eigen::Vector3d> RouteToTarget::findRouteAlongExtraPoints(Vector3D targetPosition,std::map<QString, PointMetricPtr> extraAirwayPoints)
{
	std::map<QString, PointMetricPtr>::iterator it = extraAirwayPoints.begin();
	std::vector<Eigen::Vector3d> extraAirwayPointsPositions;
	for( ; it != extraAirwayPoints.end(); ++it)
		extraAirwayPointsPositions.push_back(it->second->getCoordinate());

	std::vector<Eigen::Vector3d> routePositions;
	routePositions.push_back(targetPosition);
	Vector3D closestPointCoordinate = targetPosition;
	while(!extraAirwayPointsPositions.empty())
	{
		std::pair<int, double> closestPoint = findDistanceFromPointToLine(closestPointCoordinate, extraAirwayPointsPositions);
		int index = closestPoint.first;
		closestPointCoordinate = extraAirwayPointsPositions[index];
		routePositions = insertAndinterpolate(routePositions, closestPointCoordinate, 0.25);
		extraAirwayPointsPositions.erase(extraAirwayPointsPositions.begin() + index);
	}

	return routePositions;
}

std::vector<Eigen::Vector3d> RouteToTarget::insertAndinterpolate(std::vector<Eigen::Vector3d> routePositions, Eigen::Vector3d newPosition, double interpolationDistance/*mm*/)
{
	if(routePositions.size()<1)
		return routePositions;

	Eigen::Vector3d lastPosition = routePositions.back();
	double extensionDistance = findDistance(lastPosition,newPosition);
	Eigen::Vector3d extensionVectorNormalized = ( newPosition - lastPosition ) / extensionDistance;
	int numberOfextensionPoints = int(extensionDistance / interpolationDistance);
	Eigen::Vector3d extensionPointIncrementVector = extensionVectorNormalized * extensionDistance / numberOfextensionPoints;

	for (int i = 1; i<= numberOfextensionPoints; i++)
	{
		routePositions.push_back(lastPosition + extensionPointIncrementVector*i);
	}

return routePositions;
}

vtkPolyDataPtr RouteToTarget::findExtendedRoute(PointMetricPtr targetPoint)
{
	mTargetPosition = targetPoint->getCoordinate();
    double extensionPointIncrement = 0.25; //mm
    mExtendedRoutePositions.clear();
    mExtendedRoutePositions = mRoutePositions;
    mExtendedCameraRotation.clear();
    mExtendedCameraRotation = mCameraRotation;
		mExtendedGenerationNumber = mGenerationNumber;
		mExtendedRadius = mRadius;
	if(mRoutePositions.size() > 0)
	{
		double extensionDistance = findDistance(mRoutePositions.front(),mTargetPosition);
		Eigen::Vector3d extensionVectorNormalized = ( mTargetPosition - mRoutePositions.front() ) / extensionDistance;
		int numberOfextensionPoints = int(extensionDistance / extensionPointIncrement);
		Eigen::Vector3d extensionPointIncrementVector = extensionVectorNormalized * extensionDistance / numberOfextensionPoints;

		for (int i = 1; i<= numberOfextensionPoints; i++)
		{
			mExtendedRoutePositions.insert(mExtendedRoutePositions.begin(), mRoutePositions.front() + extensionPointIncrementVector*i);
			mExtendedCameraRotation.insert(mExtendedCameraRotation.begin(), mExtendedCameraRotation.front());
			mExtendedGenerationNumber.insert(mExtendedGenerationNumber.begin(), mExtendedGenerationNumber.front());
			mExtendedRadius.insert(mExtendedRadius.begin(), 0.0);
		}
	}

	vtkPolyDataPtr retval = addVTKPoints(mExtendedRoutePositions);
	return retval;
}


vtkPolyDataPtr RouteToTarget::findRouteToTargetAlongBloodVesselCenterlines(MeshPtr bloodVesselCenterlineMesh, PointMetricPtr targetPoint)
{
	vtkPolyDataPtr retval;

	vtkPolyDataPtr BVcenterline_r = bloodVesselCenterlineMesh->getTransformedPolyDataCopy(bloodVesselCenterlineMesh->get_rMd());
	Eigen::MatrixXd BVCLpoints_r = getCenterlinePositions(BVcenterline_r);
	mConnectedPointsInBVCL = findClosestBloodVesselSegments(BVCLpoints_r , mCLpoints, targetPoint->getCoordinate());

	if (mRoutePositions.empty())
		return retval;

	Eigen::MatrixXd::Index closestPositionToEndOfAirwayRTTIndex = dsearch(mRoutePositions[0], mConnectedPointsInBVCL).first;

	//setting position closest to RTT from airways in first index, where RTT should  continue
	mConnectedPointsInBVCL.col(mConnectedPointsInBVCL.cols()-1).swap(mConnectedPointsInBVCL.col(closestPositionToEndOfAirwayRTTIndex));

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
	if (mConnectedPointsInBVCL.cols() == 0 || !mPathToBloodVesselsFound)
		return airwayMesh;

	AirwaysFromCenterlinePtr airwaysFromBVCenterlinePtr = AirwaysFromCenterlinePtr(new AirwaysFromCenterline());
	airwaysFromBVCenterlinePtr->setTypeToBloodVessel(true);
	mBloodVesselBranchListPtr->interpolateBranchPositions();
	airwaysFromBVCenterlinePtr->setBranches(mBloodVesselBranchListPtr);

	airwayMesh = airwaysFromBVCenterlinePtr->generateTubes(2);

	return airwayMesh;
}

bool RouteToTarget::makeConnectedAirwayAndBloodVesselRoute()
{
	//vtkPolyDataPtr mergedRoute;

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
	double minDistance = ( mRoutePositions[0] - mBloodVesselRoutePositions[mBloodVesselRoutePositions.size()-1] ).norm();
	int connectionIndexBloodVesselRoute = mBloodVesselRoutePositions.size()-1;
	bool closerAirwayFound = false;
	Vector3D closestPosition;
	for (int i = 0; i<branches.size(); i++) //check for closer airway branch to blood vessel route
	{
		if ( branches[i]->findGenerationNumber() > 2 )//Needs to be deeper branch
		{
			Eigen::MatrixXd branchPositinos = branches[i]->getPositions();
			for (int j = 0; j<branchPositinos.cols(); j++)
			{
				double distance = findDistanceFromPointToLine(branchPositinos.col(j), mBloodVesselRoutePositions).second;
				if (minDistance > distance)
				{
					minDistance = distance;
					closestPosition = branchPositinos.col(j);
					closerAirwayFound = true;
					mProjectedBranchPtr = branches[i];
					mProjectedIndex = j;
					connectionIndexBloodVesselRoute = findDistanceFromPointToLine(branchPositinos.col(j), mBloodVesselRoutePositions).first;
				}
			}
		}
	}

	if (closerAirwayFound) //calculating new route
	{
		findClosestPointInBranches(closestPosition);
		findRoutePositions();
	}

	std::vector< Eigen::Vector3d > mergedPositions;
	mergedPositions.insert( mergedPositions.end(), mBloodVesselRoutePositions.begin(), mBloodVesselRoutePositions.begin() + connectionIndexBloodVesselRoute );
	mergedPositions.insert( mergedPositions.end(), mRoutePositions.begin(), mRoutePositions.end() );

	//make extension from blood vessel to target
	double extensionPointIncrement = 0.25; //mm
	double extensionDistance = findDistance(mergedPositions.front(),mTargetPosition);
	Eigen::Vector3d extensionVectorNormalized = ( mTargetPosition - mergedPositions.front() ) / extensionDistance;
	int numberOfextensionPoints = int(extensionDistance / extensionPointIncrement);
	Eigen::Vector3d extensionPointIncrementVector = extensionVectorNormalized * extensionDistance / numberOfextensionPoints;

	for (int i = 1; i<= numberOfextensionPoints; i++)
	{
		mergedPositions.insert(mergedPositions.begin(), mBloodVesselRoutePositions.front() + extensionPointIncrementVector*i);
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

		stream << "#Target position:" << "\n";
		stream << mTargetPosition(0) << " " << mTargetPosition(1) << " " << mTargetPosition(2) << " " << "\n";
		if (mProjectedBranchPtr)
		{
			stream << "#Route to target generations:" << "\n";
			stream << mProjectedBranchPtr->findGenerationNumber() << "\n";
		}

		stream << "#Trachea length (mm):" << "\n";
		double tracheaLength = this->getTracheaLength();
		stream << tracheaLength << "\n";

		stream << "#Route to target length - from Carina (mm):" << "\n";
		stream << calculateRouteLength(mRoutePositions) - tracheaLength << "\n";
		stream << "#Extended route to target length - from Carina (mm):" << "\n";
		stream << calculateRouteLength(mExtendedRoutePositions) - tracheaLength << "\n";
	}
}

double RouteToTarget::getTracheaLength()
{
	BranchPtr trachea = mBranchListPtr->getBranches()[0];
	int numberOfPositionsInTrachea = trachea->getPositions().cols();
	double tracheaLength = calculateRouteLength(smoothBranch(trachea, numberOfPositionsInTrachea-1, trachea->getPositions().col(numberOfPositionsInTrachea-1)));
	return tracheaLength;
}

std::vector< Eigen::Vector3d > RouteToTarget::getRoutePositions(MeshPtr route)
{
	vtkPolyDataPtr centerline_r = route->getTransformedPolyDataCopy(route->get_rMd());

	std::vector< Eigen::Vector3d > routePositions;

	//Used example from getCenterlinePositions
	int N = centerline_r->GetNumberOfPoints();
	for(vtkIdType i = 0; i < N; i++)
		{
		double p[3];
		centerline_r->GetPoint(i,p);
		Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		routePositions.push_back(position);
		}
	return routePositions;
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

void RouteToTarget::limitCameraRotation(int maxGenerationNumber)
{
	if(mCameraRotation.size() == mGenerationNumber.size())
		for(int i=mCameraRotation.size()-2; i>=0; i--)
			if(mGenerationNumber[i]>maxGenerationNumber)
				mCameraRotation[i] = mCameraRotation[i+1];
}

std::vector< Eigen::Vector3d > RouteToTarget::getRoutePositions(bool extendedRoute)
{

	std::vector< Eigen::Vector3d > positions;
	if(extendedRoute)
		positions = mExtendedRoutePositions;
	else
		positions = mRoutePositions;

	std::reverse(positions.begin(), positions.end());
	return positions;
}

std::vector< BranchPtr > RouteToTarget::getRouteBranches()
{
	std::vector< BranchPtr > routePositionsBranch = mRoutePositionsBranch;
	std::reverse(routePositionsBranch.begin(), routePositionsBranch.end());
	return routePositionsBranch;
}

std::vector< double > RouteToTarget::getCameraRotation()
{
	std::vector< double > rotations = mExtendedCameraRotation;
	std::reverse(rotations.begin(), rotations.end());
	return rotations;
}

std::vector< int > RouteToTarget::getGenerationNumbers()
{
	std::vector< int > generationNumbers = mExtendedGenerationNumber;
	std::reverse(generationNumbers.begin(), generationNumbers.end());
	return generationNumbers;
}

std::vector< double > RouteToTarget::getRadius()
{
	std::vector< double > radius = mExtendedRadius;
	std::reverse(radius.begin(), radius.end());
	return radius;
}

std::vector< int > RouteToTarget::getBranchingIndex()
{
	return mBranchingIndex;
}

void RouteToTarget::makeMarianaCenterlineFile(QString filename)
{
	if (mExtendedRoutePositions.empty())
	{
			std::cout << "mExtendedRoutePositions is empty." << std::endl;
			return;
	}

	int numberOfExtendedPositions = mExtendedRoutePositions.size() - mRoutePositions.size();

	std::ofstream out(filename.toStdString().c_str());
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

QJsonArray makeMarianaCenterlineOfFullBranchTreeJSON(BranchListPtr branchList)
{
	QJsonArray array;

	std::vector<BranchPtr> branches = branchList->getBranches();
	for (int i = 0; i < branches.size(); i++)
	{
		Eigen::MatrixXd positions = branches[i]->getPositions();
		for (int j = 0; j < positions.cols(); j++)
		{
			QJsonObject JsonPosition;
			JsonPosition.insert( "x", positions(0,j) );
			JsonPosition.insert( "y", positions(1,j) );
			JsonPosition.insert( "z", positions(2,j) );
			array.append(JsonPosition);
		}
	 }

	 return array;
}

/*
		RouteToTarget::getBranchPositions is used to get positions of a branch without smoothing.
        Equivalent to smoothBranch without smoothing.
*/
std::vector< Eigen::Vector3d > getBranchPositions(BranchPtr branchPtr, int startIndex)
{
		Eigen::MatrixXd branchPositions = branchPtr->getPositions();
		std::vector< Eigen::Vector3d > positions;

		for (int i = startIndex; i >=0; i--)
				positions.push_back(branchPositions.col(i));

		return positions;
}



Eigen::MatrixXd findClosestBloodVesselSegments(Eigen::MatrixXd bloodVesselPositions , Eigen::MatrixXd airwayPositions, Vector3D targetPosition)
{
	double maxDistanceToAirway = 10; //mm
	int minNumberOfPositionsInSegment = 100; //to avoid small segments which are probably not true blood vessels

	Eigen::MatrixXd bloodVesselSegment;

	while (bloodVesselPositions.cols() > minNumberOfPositionsInSegment)
	{
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

std::pair<int, double> findDistanceFromPointToLine(Eigen::MatrixXd point, std::vector< Eigen::Vector3d > line)
{
	int index = 0;
	double minDistance = findDistance(point, line[0]);
	for (int i=1; i<line.size(); i++)
		if (minDistance > findDistance(point, line[i]))
		{
			minDistance = findDistance(point, line[i]);
			index = i;
		}

	return std::make_pair(index , minDistance);
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

double variance(Eigen::VectorXd X)
{
	double mean_X = X.mean();
	double var = 0;
	for (int i = 0; i < X.size(); i++)
		var += ( X[i]-mean_X ) * ( X[i]-mean_X );

	var = var/X.size();
	return var;
}

Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r)
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

} /* namespace cx */
