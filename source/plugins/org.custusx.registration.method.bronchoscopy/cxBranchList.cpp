/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxBranchList.h"
#include "cxBranch.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include <vtkPolyData.h>
#include <vtkCardinalSpline.h>
#include "cxLogger.h"
#include <boost/math/special_functions/fpclassify.hpp> // isnan

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{

BranchList::BranchList()
{

}


BranchList::~BranchList()
{
	//	for (int i = 0; i < mBranches.size(); i++)
	//		mBranches[i]->~Branch();
}

void BranchList::addBranch(BranchPtr b)
{
	mBranches.push_back(b);
}

void BranchList::deleteBranch(BranchPtr b)
{
	if(b->getParentBranch())
		b->getParentBranch()->deleteChildBranches();

	for( int i = 0; i < mBranches.size(); i++ )
	{
		if (b == mBranches[i])
		{
			mBranches.erase(mBranches.begin() + i);
			return;
		}
	}
}

void BranchList::deleteAllBranches()
{
	mBranches.clear();
}

std::vector<BranchPtr> BranchList::getBranches()
{
	return mBranches;
}

void BranchList::selectGenerations(int maxGeneration)
{
	std::vector<int> branchNumbersToBeDeleted;
	for( int i = 0; i < mBranches.size(); i++ )
	{
		int generationCounter = 1;
		BranchPtr currentBranch = mBranches[i];
		while (currentBranch->getParentBranch()){
			generationCounter++;
			currentBranch = currentBranch->getParentBranch();
			if (generationCounter > maxGeneration)
			{
				branchNumbersToBeDeleted.push_back(i);
				break;
			}
		}

	}

	for ( int i = branchNumbersToBeDeleted.size() - 1; i >= 0; i-- )
		deleteBranch(mBranches[branchNumbersToBeDeleted[i]]);
}

void BranchList::findBronchoscopeRotation()
{
	if(mBranches.empty())
		return;
	BranchPtr trachea = mBranches[0];
	if(trachea)
		calculateBronchoscopeRotation(trachea);
}

void BranchList::calculateBronchoscopeRotation(BranchPtr branch)
// recursive function on all child branches
{
	if(!branch->getParentBranch())
		branch->setBronchoscopeRotation(0);
	else
	{
		double parentRotation = branch->getParentBranch()->getBronchoscopeRotation();
		Eigen::MatrixXd branchOrientations = branch->getOrientations();
		Vector3D branchOrientationStart = branchOrientations.leftCols(std::min(25, (int) branchOrientations.cols())).rowwise().mean();
		Eigen::MatrixXd parentBranchOrientations = branch->getParentBranch()->getOrientations();
		Vector3D parentBranchOrientationEnd = parentBranchOrientations.rightCols(std::min(50, (int) parentBranchOrientations.cols())).rowwise().mean();

		Vector3D bendingDirection = calculateBronchoscopeBendingDirection(parentBranchOrientationEnd, branchOrientationStart);
		double bronchoscopeRotation = bendingDirectionToBronchoscopeRotation(bendingDirection, parentBranchOrientationEnd, parentRotation);

		branch->setBronchoscopeBendingDirection(bendingDirection);
		branch->setBronchoscopeRotation(bronchoscopeRotation);
	}

	branchVector childBranches = branch->getChildBranches();
	for(int i=0; i<childBranches.size(); i++)
		calculateBronchoscopeRotation(childBranches[i]);
}

double bendingDirectionToBronchoscopeRotation(Vector3D bendingDirection, Vector3D parentBranchOrientation, double parentRotation)
{
	double bronchoscopeRotation;

	Vector3D xVector = Vector3D(1,0,0);
	Vector3D up = cross(parentBranchOrientation, xVector).normalized();
	if(up(1) > 0)
		up = -up;

	bronchoscopeRotation = acos( up.dot(bendingDirection) );

	Vector3D N = cross(up, bendingDirection).normalized();

	if(parentBranchOrientation.dot(N) < 0)
		bronchoscopeRotation = -bronchoscopeRotation;

	double rotationDifferenceFromParent = bronchoscopeRotation - parentRotation;

	//Make sure rotation difference is between -180 and 180 deg.
	if ( rotationDifferenceFromParent > M_PI )
		rotationDifferenceFromParent = rotationDifferenceFromParent - 2*M_PI;
	else if ( rotationDifferenceFromParent < -M_PI )
		rotationDifferenceFromParent = rotationDifferenceFromParent + 2*M_PI;

	//Tilt down if needed rotation is less than maxRotationToTiltDown
	if( rotationDifferenceFromParent < (MAX_ROTATION_TO_TILT_DOWN_DEGREES - 180)*M_PI/180 )
		bronchoscopeRotation = bronchoscopeRotation + M_PI;
	else if( rotationDifferenceFromParent > (180 - MAX_ROTATION_TO_TILT_DOWN_DEGREES)*M_PI/180 )
		bronchoscopeRotation = bronchoscopeRotation - M_PI;

	//Not allowing rotation above 180 deg
	rotationDifferenceFromParent = bronchoscopeRotation - parentRotation;
	if( rotationDifferenceFromParent > M_PI )
		bronchoscopeRotation = bronchoscopeRotation - M_PI;
	else if( rotationDifferenceFromParent < -M_PI )
		bronchoscopeRotation = bronchoscopeRotation + M_PI;



	//Sett rotasjon til samme som parent dersom endring er mindre enn 15?? grader

	return bronchoscopeRotation;
}

Vector3D calculateBronchoscopeBendingDirection(Vector3D A, Vector3D B)
{
	A = A.normalized();
	B = B.normalized();
	Vector3D N = A.cross(B);
	N = N.normalized();
	Vector3D C;

	C(2) = 1;
	C(1) = - ( ( N(2) - N(0)*A(2)/A(0) ) / ( N(1) - N(0)*A(1)/A(0) ) ) * C(2);
	if(boost::math::isnan(C(1)) || boost::math::isinf(C(1)))
		C(1) = 0;
	if( similar(A(0),0) )
		C(0) = 0;
	else
		C(0) = - ( A(1)*C(1) + A(2)*C(2) ) / A(0);

	C = C.normalized();

	if (B.dot(C) < 0)
		C = -C;

	return C;
}

void BranchList::smoothOrientations()
{
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd orientations = mBranches[i]->getOrientations();
		Eigen::MatrixXd newOrientations(orientations.rows(),orientations.cols());
		int numberOfColumns = orientations.cols();
		for (int j = 0; j < numberOfColumns; j++)
		{
			newOrientations.col(j) = orientations.block(0,std::max(j-2,0),orientations.rows(),std::min(5,numberOfColumns-j)).rowwise().mean(); //smoothing
			newOrientations.col(j) = newOrientations.col(j) / newOrientations.col(j).norm(); // normalizing
		}
		mBranches[i]->setOrientations(newOrientations);
	}
}

void BranchList::smoothRadius()
{
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::VectorXd radius = mBranches[i]->getRadius();
		Eigen::VectorXd newRadius(radius.rows(),radius.cols());
		int numberOfPoints = radius.size();
		for (int j = 0; j < numberOfPoints; j++)
		{
			newRadius[j] = radius.segment(std::max(j-2,0), std::min(5,numberOfPoints-j)).mean(); //smoothing
		}
		mBranches[i]->setRadius(newRadius);
	}
}

BranchPtr BranchList::findBranchWithLargestRadius()
{
	BranchPtr branchWithLargestRadius = mBranches[0];
	double largestRadius = mBranches[0]->getAverageRadius();

	for (int i = 1; i < mBranches.size(); i++)
	{
		if (mBranches[i]->getAverageRadius() > largestRadius)
		{
			largestRadius = mBranches[i]->getAverageRadius();
			branchWithLargestRadius = mBranches[i];
		}
	}

	return branchWithLargestRadius;
}

void BranchList::interpolateBranchPositions(double resolution /*mm*/)
{

	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();

		if (mBranches[i]->getParentBranch()) // Add parents last position to interpolate between branches
		{
			Eigen::MatrixXd parentPositions = mBranches[i]->getParentBranch()->getPositions();
			Eigen::MatrixXd positionsResized(positions.rows(), positions.cols()+1);
			positionsResized.col(0) = parentPositions.rightCols(1);
			positionsResized.rightCols(positions.cols()) = positions;
			positions = positionsResized;
		}

		std::vector<Eigen::Vector3d> interpolatedPositions;
		for (int j = 0; j < positions.cols()-1; j++)
		{
			int interpolationFactor = static_cast<int>(std::ceil( ( positions.col(j+1) - positions.col(j) ).norm() / resolution ));
			for (int k = 0; k < interpolationFactor; k++){
				Eigen::Vector3d interpolationPoint;
				interpolationPoint[0] = (positions(0,j)*(interpolationFactor-k) + positions(0,j+1)*(k) ) / interpolationFactor;
				interpolationPoint[1] = (positions(1,j)*(interpolationFactor-k) + positions(1,j+1)*(k) ) / interpolationFactor;
				interpolationPoint[2] = (positions(2,j)*(interpolationFactor-k) + positions(2,j+1)*(k) ) / interpolationFactor;
				interpolatedPositions.push_back(interpolationPoint);
			}
		}
		if (mBranches[i]->getParentBranch()) // Remove parents last position after interpolation
		{
			Eigen::MatrixXd positionsResized(positions.rows(), positions.cols()-1);
			positionsResized = positions.rightCols(positionsResized.cols());
			positions = positionsResized;
		}
		Eigen::MatrixXd interpolationResult(3 , interpolatedPositions.size());
		for (int j = 0; j < interpolatedPositions.size(); j++)
		{
			interpolationResult(0,j) = interpolatedPositions[j](0);
			interpolationResult(1,j) = interpolatedPositions[j](1);
			interpolationResult(2,j) = interpolatedPositions[j](2);
		}
		mBranches[i]->setPositions(interpolationResult);
	}

}

void BranchList::smoothBranchPositions(int controlPointDistance)
{
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		int numberOfInputPoints = positions.cols();
		//int controlPointFactor = 10;
		//int numberOfControlPoints = numberOfInputPoints / controlPointFactor;
		double branchLength = (positions.rightCols(1) - positions.leftCols(1)).norm();
		int numberOfControlPoints = std::ceil(branchLength/controlPointDistance);
		numberOfControlPoints = std::max(numberOfControlPoints, 2); // at least two control points

		vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
		vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
		vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

		//add control points to spline
		for(int j=0; j<numberOfControlPoints; j++)
		{
			int indexP = (j*numberOfInputPoints)/numberOfControlPoints;

			splineX->AddPoint(indexP,positions(0,indexP));
			splineY->AddPoint(indexP,positions(1,indexP));
			splineZ->AddPoint(indexP,positions(2,indexP));
		}
		//Always add the last point to complete spline
		splineX->AddPoint(numberOfInputPoints-1,positions(0,numberOfInputPoints-1));
		splineY->AddPoint(numberOfInputPoints-1,positions(1,numberOfInputPoints-1));
		splineZ->AddPoint(numberOfInputPoints-1,positions(2,numberOfInputPoints-1));

		//evaluate spline - get smoothed positions
		Eigen::MatrixXd smoothingResult(3 , numberOfInputPoints);
		for(int j=0; j<numberOfInputPoints; j++)
		{
			double splineParameter = j;
			smoothingResult(0,j) = splineX->Evaluate(splineParameter);
			smoothingResult(1,j) = splineY->Evaluate(splineParameter);
			smoothingResult(2,j) = splineZ->Evaluate(splineParameter);
		}
		mBranches[i]->setPositions(smoothingResult);
	}
}

void BranchList::findBranchesInCenterline(Eigen::MatrixXd positions_r, bool sortByZindex, bool connectSeparateSegments)
{
	if (sortByZindex)
		positions_r = sortMatrix(2,positions_r);

	Eigen::MatrixXd mainAirwayTree_r = findMainConnectedAirwayTree(positions_r);
	if (sortByZindex)
		mainAirwayTree_r = sortMatrix(2,mainAirwayTree_r);

	Eigen::MatrixXd positionsNotUsed_r = positions_r;

	int splitIndex;
	double minDistance;
	Eigen::MatrixXd::Index startIndex;
	BranchPtr branchToSplit;
	while (positionsNotUsed_r.cols() > 0)
	{
		if (!mBranches.empty())
		{
			minDistance = maxDistanceToExistingBranch(connectSeparateSegments);
			bool treeCompleted = findRemainingPointClosestToExistingBranch(connectSeparateSegments, positionsNotUsed_r, minDistance, startIndex, splitIndex, branchToSplit);
			if(treeCompleted)
				break;
		}
		else //if this is the first branch. Select the top position (Trachea).
		{
			std::pair<Eigen::MatrixXd::Index, double> dsearchResult = dsearch(mainAirwayTree_r.col(mainAirwayTree_r.cols()-1), positionsNotUsed_r);
			startIndex = dsearchResult.first;
			minDistance = 0;
		}

		std::pair<Eigen::MatrixXd,Eigen::MatrixXd > connectedPointsResult = findConnectedPointsInCT(startIndex , positionsNotUsed_r);
		Eigen::MatrixXd newBranchPositions = connectedPointsResult.first;
		positionsNotUsed_r = connectedPointsResult.second;

		if (newBranchPositions.cols() < MIN_BRANCH_SEGMENT_LENGTH) //only include branches of length >= 5 points
			continue;

		BranchPtr newBranch = BranchPtr(new Branch());
		newBranch->setPositions(newBranchPositions);

		if(minDistance > DISTANCE_TO_USE_BRANCH_DIRECTION_FOR_CONNECTION)
		{ // If distance to closest branch is above DISTANCE_TO_USE_BRANCH_DIRECTION_FOR_CONNECTION
			// we look at all branches at a distance below MAX_DISTANCE_TO_EXISTING_BRANCH and select the one wtih
			// lowest orientation deviation between the conection part and the new and the existing branch.
			BranchPtr branchToConnect = findBranchToConnect(newBranch, maxDistanceToExistingBranch(connectSeparateSegments));
			if (branchToConnect)
			{ // Checking if branch with orientation match is found
				branchToSplit = branchToConnect;
				splitIndex = branchToConnect->getPositions().cols()-1; //connect to last position
			}
			else
				continue; //do not add new branch if orientation match is not found
		}

		mBranches.push_back(newBranch);

		if (mBranches.size() <= 1) // do not try to split another branch when the first branch is processed
			continue;

		splitBranch(newBranch, branchToSplit, splitIndex);
	}
}

void BranchList::splitBranch(BranchPtr newBranch, BranchPtr branchToSplit, int splitIndex)
{
	if ((splitIndex + 1 >= MIN_BRANCH_SEGMENT_LENGTH) && (branchToSplit->getPositions().cols() - splitIndex - 1 >= MIN_BRANCH_SEGMENT_LENGTH))
		//do not split branch if the new branch is close to the edge of the branch
		//if the new branch is not close to one of the edges of the
		//connected existing branch: Split the existing branch
	{
		BranchPtr newBranchFromSplit = BranchPtr(new Branch());
		Eigen::MatrixXd branchToSplitPositions = branchToSplit->getPositions();
		newBranchFromSplit->setPositions(branchToSplitPositions.rightCols(branchToSplitPositions.cols() - splitIndex - 1));
		branchToSplit->setPositions(branchToSplitPositions.leftCols(splitIndex + 1));
		mBranches.push_back(newBranchFromSplit);
		newBranchFromSplit->setParentBranch(branchToSplit);
		newBranch->setParentBranch(branchToSplit);
		newBranchFromSplit->setChildBranches(branchToSplit->getChildBranches());
		branchVector branchToSplitChildren = branchToSplit->getChildBranches();
		for (int i = 0; i < branchToSplitChildren.size(); i++)
			branchToSplitChildren[i]->setParentBranch(newBranchFromSplit);
		branchToSplit->deleteChildBranches();
		branchToSplit->addChildBranch(newBranchFromSplit);
		branchToSplit->addChildBranch(newBranch);
	}
	else if (splitIndex + 1 < MIN_BRANCH_SEGMENT_LENGTH)
		// If the new branch is close to the start of the existing
		// branch: Connect it to the same position start as the
		// existing branch
	{
		newBranch->setParentBranch(branchToSplit->getParentBranch());
		if(branchToSplit->getParentBranch())
			branchToSplit->getParentBranch()->addChildBranch(newBranch);
	}
	else if (branchToSplit->getPositions().cols() - splitIndex - 1 < MIN_BRANCH_SEGMENT_LENGTH)
		// If the new branch is close to the end of the existing
		// branch: Connect it to the end of the existing branch
	{
		newBranch->setParentBranch(branchToSplit);
		branchToSplit->addChildBranch(newBranch);
	}

}

bool BranchList::findRemainingPointClosestToExistingBranch(bool connectSeparateSegments, Eigen::MatrixXd positionsNotUsed_r, double& minDistance, Eigen::MatrixXd::Index& startIndex, int& splitIndex, BranchPtr& branchToSplit)
{
	int index;
	for (int i = 0; i < mBranches.size(); i++)
	{
		std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd> distances;
		distances = dsearchn(positionsNotUsed_r, mBranches[i]->getPositions());
		double d = distances.second.minCoeff(&index);
		if (d < minDistance)
		{
			minDistance = d;
			branchToSplit = mBranches[i];
			startIndex = index;
			if (minDistance < 2)
				break;
		}
	}
	if(minDistance == maxDistanceToExistingBranch(connectSeparateSegments))
	{//No more close positions found
		return true; //Airway centerline tree completed.
	}
	std::pair<Eigen::MatrixXd::Index, double> dsearchResult = dsearch(positionsNotUsed_r.col(startIndex) , branchToSplit->getPositions());
	splitIndex = dsearchResult.first;

	return false;
}

double BranchList::maxDistanceToExistingBranch(bool connectSeparateSegments)
{
	if(connectSeparateSegments)
		return MAX_DISTANCE_TO_EXISTING_BRANCH;
	else
		return MAX_DISTANCE_BETWEEN_CONNECTED_POINTS_IN_BRANCH;
}

BranchPtr BranchList::findBranchToConnect(BranchPtr newBranch, double maxDistanceToExistingBranch)
{
	Eigen::MatrixXd newBranchPositions = newBranch->getPositions();
	Eigen::MatrixXd newBranchOrientations = newBranch->getOrientations();

	std::vector<BranchPtr> existingCloseBranches = findClosesBranches(newBranchPositions.col(0), maxDistanceToExistingBranch);
	double orientationVariance = calculate3DVaiance(newBranchOrientations);
	if(orientationVariance > MAX_ORIENTATION_VARIANCE_IN_NEW_BRANCH)
		return BranchPtr(); //Variance too large in new branch

	int numberOfColumnsInNewBranch = newBranchOrientations.cols();

	Vector3D newBranchOrientationStart = newBranchPositions.col(std::min(numberOfColumnsInNewBranch-1, 10)) - newBranchPositions.col(0);
	newBranchOrientationStart = newBranchOrientationStart / newBranchOrientationStart.norm(); // normalizing
	BranchPtr branchToConnect;
	double minOrintationDeviationToNewBranch = MAX_DIRECTION_DEVIATION_FOR_CONNECTION_NEW_BRANCH; //deg
	for (int i=0; i<existingCloseBranches.size(); i++)
	{
		if(existingCloseBranches[i]->findGenerationNumber()==1)
			continue; //Do not try to connect to trachea

		Eigen::MatrixXd existingCloseBranchPositions = existingCloseBranches[i]->getPositions();
		Eigen::MatrixXd existingCloseBranchOrientations = existingCloseBranches[i]->getOrientations();
		int numberOfColumnsInExistingBranch = existingCloseBranchOrientations.cols();
		Vector3D existingCloseBranchOrientationEnd = existingCloseBranchPositions.col(numberOfColumnsInExistingBranch-1) - existingCloseBranchPositions.col(std::max(numberOfColumnsInExistingBranch-10, 0));
		existingCloseBranchOrientationEnd = existingCloseBranchOrientationEnd / existingCloseBranchOrientationEnd.norm(); // normalizing
		Vector3D orientationBetweenBranches = newBranchPositions.leftCols(1) - existingCloseBranchPositions.rightCols(1);
		orientationBetweenBranches = orientationBetweenBranches / orientationBetweenBranches.norm(); // normalizing
		double angleDeviationInConnectionToExistingBranch = calculateAngleBetweenTwo3DVectors(existingCloseBranchOrientationEnd, orientationBetweenBranches) * 180/M_PI;
		double angleDeviationInConnectionToNewBranch = calculateAngleBetweenTwo3DVectors(orientationBetweenBranches, newBranchOrientationStart) * 180/M_PI;
		if (angleDeviationInConnectionToExistingBranch < MAX_DIRECTION_DEVIATION_FOR_CONNECTION_EXISTING_BRANCH && angleDeviationInConnectionToNewBranch < minOrintationDeviationToNewBranch)
		{// Finding min orientation deviation for all candidate branches to connect to
			minOrintationDeviationToNewBranch = angleDeviationInConnectionToNewBranch;
			branchToConnect = existingCloseBranches[i];
		}
	}
	return branchToConnect;
}

BranchListPtr BranchList::removePositionsForLocalRegistration(Eigen::MatrixXd trackingPositions, double maxDistance)
{
	BranchListPtr retval = BranchListPtr(new BranchList());
	BranchPtr b;
	for (int i = 0; i < mBranches.size(); i++)
	{
		b = BranchPtr(new Branch());
		b->setPositions(mBranches[i]->getPositions());
		retval->addBranch(b);
	}

	std::vector<BranchPtr> branches = retval->getBranches();
	Eigen::MatrixXd positions;
	Eigen::MatrixXd orientations;
	for (int i = 0; i < branches.size(); i++)
	{
		positions = branches[i]->getPositions();
		orientations = branches[i]->getOrientations();
		std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd> distanceData;
		distanceData = dsearchn(positions, trackingPositions);
		Eigen::VectorXd distance = distanceData.second;
		for (int j = positions.cols() - 1; j >= 0; j--)
		{
			if (distance(j) > maxDistance)
			{
				positions = eraseCol(j, positions);
				orientations = eraseCol(j, orientations);
			}
		}
		branches[i]->setPositions(positions);
	}
	return retval;
}

void BranchList::excludeClosePositionsInCTCenterline(double minPointDistance){

	std::vector<BranchPtr> branchVector = this->getBranches();
	for (int i = 0; i < branchVector.size(); i++)
	{
		Eigen::MatrixXd positions = branchVector[i]->getPositions();
		Eigen::MatrixXd orientations = branchVector[i]->getOrientations();

		for (int i = positions.cols()-2; i > 0; i--){
			double distance = (positions.col(i) - positions.col(i+1)).norm();
			if ( distance < minPointDistance )
			{
				positions = eraseCol(i,positions);
				orientations = eraseCol(i,orientations);
			}
		}
		branchVector[i]->setPositions(positions);
	}

}

void BranchList::markLungLap(QString name, Vector3D position)
{
	BranchPtr branch = findClosestBranch(position);
	this->setLapName(branch, name);
}

//Recursive function. Setting lap name to branch and all sub-branches
void BranchList::setLapName(BranchPtr branch, QString name)
{
	branch->setLap(name);
	branchVector bv = branch->getChildBranches();
	for(int i=0; i<bv.size(); i++)
		setLapName(bv[i], name);
}

QString BranchList::findClosestLungLap(Vector3D position)
{
	BranchPtr branch = findClosestBranch(position);
	return branch->getLap();
}

double BranchList::findDistance(Vector3D p1, Vector3D p2)
{
	double d0 = p1(0) - p2(0);
	double d1 = p1(1) - p2(1);
	double d2 = p1(2) - p2(2);

	double D = sqrt( d0*d0 + d1*d1 + d2*d2 );

	return D;
}

BranchPtr BranchList::findClosestBranch(Vector3D targetCoordinate_r)
{
	double minDistance = 100000;
	BranchPtr minDistanceBranch;
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		for (int j = 0; j < positions.cols(); j++)
		{
			double D = findDistance(positions.col(j), targetCoordinate_r);
			if (D < minDistance)
			{
				minDistance = D;
				minDistanceBranch = mBranches[i];
			}
		}
	}

		return minDistanceBranch;
}

std::vector<BranchPtr> BranchList::findClosesBranches(Vector3D position, double maxDistance)
{//Returning branches with last position closer than maxDistance from input position
	std::vector<BranchPtr> closeBranches;
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		double D = findDistance(positions.rightCols(1), position);
		if (D < maxDistance)
			closeBranches.push_back(mBranches[i]);
	}
		return closeBranches;
}

/**
 * @brief BranchList::createVtkPolyDataFromBranches
 * Return a VtkPolyData object created from the
 * branches in this object.
 * @param fullyConnected
 * The original version of this code created an object
 * where there might be gaps between the end of parent
 * and child branches. This parameter lets you make
 * connections between the branches to fill these gaps.
 * Note however, that this option gives strange results on
 * many of the real centerlines tried. Not sure where the problem is.
 * It works on the dummy centerline used in the unit test.
 * @param straightBranches
 * By using this parameter, you will include only the first
 * and last points from a branch. Hence you will get
 * straight branches in your polydata.
 * @return a vtkpolydata object of your branch tree.
 */
vtkPolyDataPtr BranchList::createVtkPolyDataFromBranches(bool fullyConnected, bool straightBranches) const
{
	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr lines = vtkCellArrayPtr::New();

	int positionCounter = 0;
	for (size_t i = 0; i < mBranches.size(); ++i)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		if(straightBranches)
		{
			++positionCounter;
			points->InsertNextPoint(positions(0,0),positions(1,0),positions(2,0));
			points->InsertNextPoint(positions(0,positions.cols()-1),positions(1,positions.cols()-1),positions(2,positions.cols()-1));
			vtkIdType connection[2] = {positionCounter-1, positionCounter};
			lines->InsertNextCell(2, connection);
			++positionCounter;
		}
		else
		{
			for (int j = 0; j < positions.cols(); ++j)
			{
				++positionCounter;
				points->InsertNextPoint(positions(0,j),positions(1,j),positions(2,j));
				if (j	 < positions.cols()-1)
				{
					vtkIdType connection[2] = {positionCounter-1, positionCounter};
					lines->InsertNextCell(2, connection);
				}
			}
		}
	}
	if(fullyConnected)
	{
		int this_branchs_first_point_in_full_polydata_point_list = 0;
		for(size_t i = 0; i < mBranches.size(); ++i)
		{
			if(i>0)
			{
				if(!straightBranches)
					this_branchs_first_point_in_full_polydata_point_list += mBranches[i-1]->getPositions().cols();
				else
					this_branchs_first_point_in_full_polydata_point_list += 2;
			}
			int parent_index_in_branch_list = mBranches[i]->findParentIndex(mBranches);

			if(parent_index_in_branch_list > -1)
			{
				int parent_branch_last_point_in_full_polydata = 0;
				for(int j = 0; j <= parent_index_in_branch_list; ++j)
				{
					if(!straightBranches)
						parent_branch_last_point_in_full_polydata += mBranches[j]->getPositions().cols() - 1;
					else
						parent_branch_last_point_in_full_polydata += (1 + j*2);
				}
				vtkIdType connection[2] = {parent_branch_last_point_in_full_polydata, this_branchs_first_point_in_full_polydata_point_list};
				lines->InsertNextCell(2, connection);
			}

		}

	}
	retval->SetPoints(points);
	retval->SetLines(lines);

	return retval;
}

Eigen::MatrixXd BranchList::findMainConnectedAirwayTree(Eigen::MatrixXd positions_r)
{
	Eigen::MatrixXd mainAirwayTree_r;
	std::vector<Eigen::MatrixXd> connectedSegments;

	while(positions_r.cols() > 0)
	{
		std::pair<Eigen::MatrixXd,Eigen::MatrixXd > connectedPointsResult = findConnectedPointsInCT(0 , positions_r);
		Eigen::MatrixXd connectedPositions = connectedPointsResult.first;
		positions_r = connectedPointsResult.second; //remaining positions
		connectedSegments.push_back(connectedPositions); // add new segment
		for(int i=connectedSegments.size()-2; i >= 0; i--) //iterating backwards to erase elements
		{//check if existing segments should be connected to new segment
			if(checkIfTwoPointCloudsAreClose(connectedSegments[i], connectedSegments.back(), MAX_DISTANCE_BETWEEN_CONNECTED_POINTS_IN_BRANCH))
			{
				connectedSegments.back().conservativeResize(Eigen::NoChange, connectedSegments[i].cols() + connectedSegments.back().cols());
				connectedSegments.back().rightCols(connectedSegments[i].cols()) = connectedSegments[i];
				connectedSegments.erase(connectedSegments.begin() + i);
			}
		}
	}

	for(int i=0; i<connectedSegments.size(); i++)
	{
		if(connectedSegments[i].cols() > mainAirwayTree_r.cols())
			mainAirwayTree_r = connectedSegments[i];
	}

	return mainAirwayTree_r;
}

bool checkIfTwoPointCloudsAreClose(Eigen::MatrixXd C1, Eigen::MatrixXd C2, double maxDistance/*mm*/)
{
	Eigen::MatrixXd::Index index;
	for (int i = 0; i < C1.cols(); i++)
	{
		// find nearest neighbour
		(C2.colwise() - C1.col(i)).colwise().squaredNorm().minCoeff(&index);
		double distance = (C2.col(index) - C1.col(i)).norm();
		if(distance < maxDistance)
			return true;
	}
	return false;
}

Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix)
{
	for (int i = 0; i < matrix.cols() - 1; i++)  {
		for (int j = i + 1; j < matrix.cols(); j++) {
			if (matrix(rowNumber,i) > matrix(rowNumber,j)){
				matrix.col(i).swap(matrix.col(j));
			}
		}
	}
	return matrix;
}



Eigen::MatrixXd eraseCol(int removeIndex, Eigen::MatrixXd positions)
{
	positions.block(0 , removeIndex , positions.rows() , positions.cols() - removeIndex - 1) = positions.rightCols(positions.cols() - removeIndex - 1);
	positions.conservativeResize(Eigen::NoChange, positions.cols() - 1);
	return positions;
}

std::pair<Eigen::MatrixXd::Index, double> dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions)
{
	Eigen::MatrixXd::Index index;
	// find nearest neighbour
	(positions.colwise() - p).colwise().squaredNorm().minCoeff(&index);
	double d = (positions.col(index) - p).norm();

	return std::make_pair(index , d);
}

std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2)
{
	Eigen::MatrixXd::Index index;
	std::vector<Eigen::MatrixXd::Index> indexVector;
	Eigen::VectorXd D(p1.cols());
	for (int i = 0; i < p1.cols(); i++)
	{
		// find nearest neighbour
		(p2.colwise() - p1.col(i)).colwise().squaredNorm().minCoeff(&index);
		D(i) = (p2.col(index) - p1.col(i)).norm();
		indexVector.push_back(index);
	}
	return std::make_pair(indexVector , D);
}

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed)
{
	//Eigen::MatrixXd branchPositions(positionsNotUsed.rows(), positionsNotUsed.cols());
	Eigen::MatrixXd thisPosition(3,1);
	std::vector<Eigen::MatrixXd> branchPositionsVector;
	thisPosition = positionsNotUsed.col(startIndex);
	branchPositionsVector.push_back(thisPosition); //add first position to branch
	positionsNotUsed = eraseCol(startIndex,positionsNotUsed);; //remove first position from list of remaining points

	while (positionsNotUsed.cols() > 0)
	{
		std::pair<Eigen::MatrixXd::Index, double > minDistance = dsearch(thisPosition, positionsNotUsed);
		Eigen::MatrixXd::Index index = minDistance.first;
		double d = minDistance.second;
		if (d > MAX_DISTANCE_BETWEEN_CONNECTED_POINTS_IN_BRANCH) // more than 3 mm distance to closest point --> branch is compledted
			break;

		thisPosition = positionsNotUsed.col(index);
		positionsNotUsed = eraseCol(index,positionsNotUsed);
		//add position to branch
		branchPositionsVector.push_back(thisPosition);

	}

	Eigen::MatrixXd branchPositions(3,branchPositionsVector.size());

	for (int j = 0; j < branchPositionsVector.size(); j++)
	{
		branchPositions.block(0,j,3,1) = branchPositionsVector[j];
	}

	return std::make_pair(branchPositions, positionsNotUsed);
}

/*
	smoothBranch is smoothing the positions of a centerline branch by using vtkCardinalSpline.
	The degree of smoothing is dependent on the branch radius and the shape of the branch.
	First, the method tests if a straight line from start to end of the branch is sufficient by the condition of
	all positions along the line being within the lumen of the airway (max distance from original centerline
	is set to branch radius).
	If this fails, one more control point is added to the spline at the time, until the condition is fulfilled.
	The control point added for each iteration is the position with the larges deviation from the original/unfiltered
	centerline.
*/
std::vector< Eigen::Vector3d > smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition)
{
	vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
	vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

	double branchRadius = branchPtr->findBranchRadius()/2;

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

double calculateAngleBetweenTwo3DVectors(Vector3D A, Vector3D B)
{
	double dot = A(0)*B(0) + A(1)*B(1) + A(2)*B(2);
	double cross_x = A(1)*B(2) - A(2)*B(1);
	double cross_y = A(2)*B(0) - A(0)*B(2);
	double cross_z = A(0)*B(1) - A(1)*B(0);
	double det = sqrt(cross_x*cross_x + cross_y*cross_y + cross_z*cross_z);
	double angle = atan2(det, dot);
	return angle;
}

double calculate3DVaiance(Eigen::MatrixXd A)
{ // Calculate variance in 3D. Variance found as the sum of variances for each dimension/axis.
	if(A.rows() != 3)
	{
		CX_LOG_WARNING() << "In calculate3DVaiance: Input matrix must be 3xN.";
		return 0;
	}
	Eigen::Vector3d mean = A.rowwise().mean();
	Eigen::Vector3d variance = Eigen::Vector3d::Zero();

	for (int i = 0; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
			variance(i) += ( A(i,j)-mean(i) ) * ( A(i,j)-mean(i) );
	}

	variance = variance/A.cols();
	double variance3D = variance.sum();

	return variance3D;
}

}//namespace cx
