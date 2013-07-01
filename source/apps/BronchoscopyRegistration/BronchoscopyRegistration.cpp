/*
 * BronchoscopyRegistration.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: ehofstad
 */

#include "BronchoscopyRegistration.h"
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include "vtkMatrix4x4.h"
#include "vtkLinearTransform.h"
#include "vtkLandmarkTransform.h"
#include "sscTransform3D.h"

namespace cx
{


BronchoscopyRegistration::BronchoscopyRegistration()
{

}

M4Vector excludeClosePositions(M4Vector Tnavigation)
//		To avoid having too many tracking data samples in the registration,
//		we are only including tracking data with a distance to last included
//		tracking position of more than 1 mm.
{
	M4Vector TnavigationIncluded;
	TnavigationIncluded.push_back(Tnavigation[0]); // first position is always included
	int numberOfIncluded = 0;
	int numberOfPos = Tnavigation.size();
	for ( int index = 1; index <= numberOfPos; index++)
	{
		double xDistance = (TnavigationIncluded[numberOfIncluded](0,3) - Tnavigation[index](0,3) );
		double xDistanceSquared = xDistance * xDistance;
		double yDistance = (TnavigationIncluded[numberOfIncluded](1,3) - Tnavigation[index](1,3) );
		double yDistanceSquared = yDistance * yDistance;
		double zDistance = (TnavigationIncluded[numberOfIncluded](2,3) - Tnavigation[index](2,3) );
		double zDistanceSquared = zDistance * zDistance;
		double distanceToLastIncluded = sqrt (xDistanceSquared + yDistanceSquared + zDistanceSquared);

		if (distanceToLastIncluded > 1) //condition of at least movement of 1 mm from last included sample
		{
			numberOfIncluded ++;
			TnavigationIncluded.push_back(Tnavigation[index]);
		}
	}

	return TnavigationIncluded;
}



Eigen::VectorXd sortVector(Eigen::VectorXd v)
{
	for (int i = 0; i < v.size() - 1; i++)  {
	   for (int j = i + 1; j < v.size(); j++) {
		  if (v(i) > v(j)){
			  std::swap(v(i) , v(j));
		  }
	   }
	}
return v;
}

std::pair<Eigen::MatrixXd , Eigen::MatrixXd> findPositionsWithSmallesAngleDifference(int percentage , Eigen::VectorXd DAngle , Eigen::MatrixXd trackingPositions , Eigen::MatrixXd nearestCTPositions)
{
	Eigen::VectorXd DAngleSorted = sortVector(DAngle);
	int numberOfPositionsIncluded = floor(DAngle.size() * percentage/100);
	Eigen::MatrixXd trackingPositionsIncluded(3 , numberOfPositionsIncluded );
	Eigen::MatrixXd nearestCTPositionsIncluded(3 , numberOfPositionsIncluded );
	float maxDAngle = DAngleSorted( numberOfPositionsIncluded );
	int counter = 0;
			for (int i = 0; i < DAngle.size(); i++)
			{
				if (DAngle(i) <= maxDAngle and counter < numberOfPositionsIncluded)
				{
					trackingPositionsIncluded.col(counter) = trackingPositions.col(i);
					nearestCTPositionsIncluded.col(counter) = nearestCTPositions.col(i);
					counter++;
				}
			}

	return std::make_pair(trackingPositionsIncluded , nearestCTPositionsIncluded);
}


vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions)
{
  vtkPointsPtr retval = vtkPointsPtr::New();

  for (unsigned i=0; i<positions.cols(); ++i)
  {
    retval->InsertNextPoint(positions(0,i), positions(1,i), positions(2,i));
  }
  return retval;
}

/** Perform a landmark registration between the data sets source and target.
 *  Return transform from source to target.
 */
Eigen::Matrix4d performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target /*, bool* ok */)
{
  //*ok = false;

  // too few data samples: ignore
  if (source->GetNumberOfPoints() < 3)
  {
    return Eigen::Matrix4d::Identity();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  landmarktransform->Update();

  vtkMatrix4x4* temp = landmarktransform->GetMatrix();
  Eigen::Matrix4d tar_M_src;
  for (int i = 0; i < 4; i++)
  {
	  for (int j = 0; j < 4; j++)
	  {
		  tar_M_src(i,j) = temp->GetElement(i,j);
		  //std::cout << tar_M_src(i,j) << " ";
	  }
	  //std::cout << std::endl;
  }

  if (QString::number(tar_M_src(0,0))=="nan") // harry but quick way to check badness of transform...
  {
    return Eigen::Matrix4d::Identity();
  }

  //*ok = true;
  return tar_M_src;
}



std::vector<Eigen::MatrixXd::Index> dsearch2n(Eigen::MatrixXd pos1, Eigen::MatrixXd pos2, Eigen::MatrixXd ori1, Eigen::MatrixXd ori2)
{
	Eigen::MatrixXd::Index index;
	std::vector<Eigen::MatrixXd::Index> indexVector;
	for (int i = 0; i < pos1.cols(); i++)
	{
		Eigen::VectorXf D(pos2.cols());
		for (int j = 0; j < pos2.cols(); j++)
		{
			float p0 = ( pos2(0,j) - pos1(0,i) ) * ( pos2(0,j) - pos1(0,i) );
			float p1 = ( pos2(1,j) - pos1(1,i) ) * ( pos2(1,j) - pos1(1,i) );
			float p2 = ( pos2(2,j) - pos1(2,i) ) * ( pos2(2,j) - pos1(2,i) );
			float o0 = fmod( std::abs( ori2(0,j) - ori1(0,i) ) , 2 ) * fmod( std::abs( ori2(0,j) - ori1(0,i) ) , 2 );
			float o1 = fmod( std::abs( ori2(1,j) - ori1(1,i) ) , 2 ) * fmod( std::abs( ori2(1,j) - ori1(1,i) ) , 2 );
			float o2 = fmod( std::abs( ori2(2,j) - ori1(2,i) ) , 2 ) * fmod( std::abs( ori2(2,j) - ori1(2,i) ) , 2 );
			D(j) = sqrt(p0+p1+p2) + 100 * sqrt(o0+01+02);
		}
		D.minCoeff(&index);
		indexVector.push_back(index);
	}
	return indexVector;
}

Eigen::Matrix4d registrationAlgorithm(BranchList* branches, M4Vector Tnavigation)
{
	Eigen::Matrix4d registrationMatrix;
	Eigen::MatrixXd CTPositions;
	Eigen::MatrixXd CTOrientations;
	Eigen::MatrixXd trackingPositions(3 , Tnavigation.size());
	Eigen::MatrixXd trackingOrientations(3, Tnavigation.size());

	std::vector<Branch*> branchVector = branches->getBranches();
	CTPositions = branchVector[0]->getPositions();
	CTOrientations = branchVector[0]->getOrientations();

	for (int i = 1; i < branchVector.size(); i++)
		{
			Eigen::MatrixXd CTPositionsNew(CTPositions.rows() , CTPositions.cols() + branchVector[i]->getPositions().cols());
			Eigen::MatrixXd CTOrientationsNew(CTOrientations.rows() , CTOrientations.cols() + branchVector[i]->getOrientations().cols());
			CTPositionsNew.leftCols(CTPositions.cols()) = CTPositions;
			CTPositionsNew.rightCols(branchVector[i]->getPositions().cols()) = branchVector[i]->getPositions();
			CTOrientationsNew.leftCols(CTOrientations.cols()) = CTOrientations;
			CTOrientationsNew.rightCols(branchVector[i]->getOrientations().cols()) = branchVector[i]->getOrientations();
			CTPositions.swap(CTPositionsNew);
			CTOrientations.swap(CTOrientationsNew);
		}

	for (int i = 0; i < Tnavigation.size(); i++)
	{
		trackingPositions.block(0 , i , 3 , 1) = Tnavigation[i].topRightCorner(3 , 1);
		trackingOrientations.block(0 , i , 3 , 1) = Tnavigation[i].block(0 , 2 , 3 , 1);
	}

	//Adjusting points for centeroids
	Eigen::Vector3d translation = CTPositions.rowwise().mean() - trackingPositions.rowwise().mean();
	Eigen::Matrix3d rotation;
	trackingPositions = trackingPositions.colwise() + translation;

	registrationMatrix << 1, 0, 0, translation(0),
						  0, 1, 0, translation(1),
						  0, 0, 1, translation(2),
						  0, 0, 0, 1;

	while ( translation.array().abs().sum() > 1)
	{
		std::vector<Eigen::MatrixXd::Index> indexVector = dsearch2n( trackingPositions, CTPositions, trackingOrientations, CTOrientations );
		Eigen::MatrixXd nearestCTPositions(3,indexVector.size());
		Eigen::MatrixXd nearestCTOrientations(3,indexVector.size());
		Eigen::VectorXd DAngle(indexVector.size());
		for (int i = 0; i < indexVector.size(); i++)
		{
			nearestCTPositions.col(i) = CTPositions.col(indexVector[i]);
			nearestCTOrientations.col(i) = CTOrientations.col(indexVector[i]);
			//DAngle(i) = ( trackingOrientations.col(i) - nearestCTOrientations.col(i) ).squaredNorm();
			float o0 = fmod( std::abs( trackingOrientations(0,i) - nearestCTOrientations(0,i) ) , 2 );
			float o1 = fmod( std::abs( trackingOrientations(1,i) - nearestCTOrientations(1,i) ) , 2 );
			float o2 = fmod( std::abs( trackingOrientations(2,i) - nearestCTOrientations(2,i) ) , 2 );
			DAngle(i) = sqrt(o0*o0+01*o1+02*o2);
		}

		std::pair<Eigen::MatrixXd , Eigen::MatrixXd> result = findPositionsWithSmallesAngleDifference(70 , DAngle , trackingPositions , nearestCTPositions);
		vtkPointsPtr trackingPositions_vtk = convertTovtkPoints(result.first);
		vtkPointsPtr CTPositions_vtk = convertTovtkPoints(result.second);
		Eigen::Matrix4d tempMatrix = performLandmarkRegistration(trackingPositions_vtk, CTPositions_vtk);
		registrationMatrix = tempMatrix * registrationMatrix;
		translation << tempMatrix(0,3), tempMatrix(1,3), tempMatrix(2,3);
		rotation = tempMatrix.topLeftCorner(3,3);

		for (int i = 0; i < trackingPositions.cols(); i++)
			trackingPositions.col(i) = rotation * trackingPositions.col(i) + translation;

		std::cout << "translation: " << translation.array().abs().sum() << std::endl;
	}

	return registrationMatrix;
}



void BronchoscopyRegistration::runBronchoscopyRegistration()
{
	const char * filenameNavigation = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 004/Navigation bronchoscope removed bad fit.csv";
//	const char * filenameNavigation = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 018/Navigation_RemovedBadFit.csv";
	const char * filenameDirection = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 004/Fast registration - pointer direction.csv";
//	const char * filenameDirection = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 018/Direction2.csv";

	PositionData* posData = new PositionData();
	std::pair< M4Vector, Eigen::Matrix4d > loadPosResult = posData->loadBronchoscopeTracking(filenameNavigation, filenameDirection);
	M4Vector Tnavigation = loadPosResult.first;
	Eigen::Matrix4d TtoCTcoordinates = loadPosResult.second;

	QString CLname = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 004/Patient_004_segmented_osirix_20111216T141209_seg1_cl1.vtk";
//	QString CLname = "/Users/ehofstad/Data/Lunge/LungNav - pilot study/Patient 018/Patient018_SegmentedAirways_Mimics_20130506T152436_seg1_cl1.vtk";
	ssc::MeshPtr mesh = ssc::dataManager()->loadMesh(CLname, CLname, ssc::rtPOLYDATA);

	vtkPolyDataPtr poly = mesh->getVtkPolyData();
	vtkPointsPtr points = poly->GetPoints();

	int N = poly->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);

	for(vtkIdType i = 0; i < N; i++)
	    {
	    double p[3];
	    poly->GetPoint(i,p);
	    CLpoints(0,i) = p[0]; CLpoints(1,i) = p[1]; CLpoints(2,i) = p[2];
	    }

	Tnavigation = excludeClosePositions(Tnavigation);

	BranchList* branches = new BranchList();
	branches->findBranchesInCenterline(CLpoints);
	branches->calculateOrientations();
	branches->smoothOrientations();

	Eigen::Matrix4d regMatrix = registrationAlgorithm(branches, Tnavigation);

	Eigen::Matrix4d regMatrixForCustusX = regMatrix * TtoCTcoordinates.inverse();

	std::cout << "regMatrix: " << std::endl;
	for (int i = 0; i < 4; i++)
		std::cout << regMatrix.row(i) << std::endl;

	std::cout << "regMatrixForCustusX: " << std::endl;
		for (int i = 0; i < 4; i++)
			std::cout << regMatrixForCustusX.row(i) << std::endl;


	std::vector<Branch*> BL = branches->getBranches();

	int totalNumberOfPoints = 0;
	std::cout << "Number of branches: " << BL.size() << std::endl;
	for (int i = 0; i < BL.size(); i++)
	{
		totalNumberOfPoints += BL[i]->getPositions().cols();
	}
	std::cout << "totalNumberOfPoints: " << totalNumberOfPoints << std::endl;
	branches->~BranchList();

}

BronchoscopyRegistration::~BronchoscopyRegistration()
{

}



}//namespace cx
