/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxBronchoscopyRegistration.h"
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataWriter.h>
#include <vtkCellArray.h>
#include <vtkMatrix4x4.h>
#include <vtkLinearTransform.h>
#include <vtkLandmarkTransform.h>
#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxLogger.h"
#include <boost/math/special_functions/fpclassify.hpp> // isnan

namespace cx
{


BronchoscopyRegistration::BronchoscopyRegistration():
	mCenterlineProcessed(false),
	mBranchListPtr(new BranchList)
{

}

M4Vector excludeClosePositions(M4Vector Tnavigation)
//		To avoid having too many tracking data samples in the registration,
//		we are only including tracking data with a distance to last included
//		tracking position of more than 1 mm.
{
	M4Vector TnavigationIncluded;
	if(Tnavigation.empty())
		return TnavigationIncluded;
	TnavigationIncluded.push_back(Tnavigation[0]); // first position is always included
	int numberOfIncluded = 0;
	size_t numberOfPos = Tnavigation.size();
	for ( size_t index = 1; index < numberOfPos; index++)
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


Eigen::VectorXd findMedian(Eigen::MatrixXd matrix)
{
    Eigen::VectorXd medianValues(matrix.rows());
    for (int i = 0; i < matrix.rows(); i++)  {
        Eigen::MatrixXd sortedMatrix = sortMatrix(i, matrix);
        if (sortedMatrix.cols()%2==1) {// odd number
            medianValues(i) = (sortedMatrix(i,(sortedMatrix.cols()+1)/2) );
        }
        else { // even number
            medianValues(i) = ( sortedMatrix(i,sortedMatrix.cols()/2) + sortedMatrix(i,sortedMatrix.cols()/2 - 1) ) / 2;
        }
    }
        return medianValues;
}


std::pair<Eigen::MatrixXd , Eigen::MatrixXd> findPositionsWithSmallesAngleDifference(int percentage , Eigen::VectorXd DAngle , Eigen::MatrixXd trackingPositions , Eigen::MatrixXd nearestCTPositions)
{
	Eigen::VectorXd DAngleSorted = sortVector(DAngle);
	int numberOfPositionsIncluded = floor((double)(DAngle.size() * percentage/100));
	Eigen::MatrixXd trackingPositionsIncluded(3 , numberOfPositionsIncluded );
	Eigen::MatrixXd nearestCTPositionsIncluded(3 , numberOfPositionsIncluded );
	float maxDAngle = DAngleSorted( numberOfPositionsIncluded );
	int counter = 0;
			for (int i = 0; i < DAngle.size(); i++)
			{
				if ((DAngle(i) <= maxDAngle) && (counter < numberOfPositionsIncluded))
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
    std::cout << "Warning in performLandmarkRegistration: Need >3 positions, returning identity matrix." << std::endl;
    return Eigen::Matrix4d::Identity();
  }

  vtkLandmarkTransformPtr landmarktransform = vtkLandmarkTransformPtr::New();
  landmarktransform->SetSourceLandmarks(source);
  landmarktransform->SetTargetLandmarks(target);
  landmarktransform->SetModeToRigidBody();
  source->Modified();
  target->Modified();
  //landmarktransform->Update();

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

	if ( boost::math::isnan(tar_M_src.sum()) )
  {
       std::cout << "Warning in performLandmarkRegistration: Returning identity matrix due to nan." << std::endl;
       return Eigen::Matrix4d::Identity();
  }

  return tar_M_src;
}




    std::vector<Eigen::MatrixXd::Index> dsearch2n(Eigen::MatrixXd pos1, Eigen::MatrixXd pos2, Eigen::MatrixXd ori1, Eigen::MatrixXd ori2)
    {
        Eigen::MatrixXd::Index index;
        std::vector<Eigen::MatrixXd::Index> indexVector;
        
        for (int i = 0; i < pos1.cols(); i++)
        {
            Eigen::VectorXd D(pos2.cols());
            Eigen::VectorXd P(pos2.cols());
            Eigen::VectorXd O(pos2.cols());
            Eigen::VectorXd R(pos2.cols());
            
            for (int j = 0; j < pos2.cols(); j++)
            {
                float p0 = ( pos2(0,j) - pos1(0,i) );
                float p1 = ( pos2(1,j) - pos1(1,i) );
                float p2 = ( pos2(2,j) - pos1(2,i) );
                float o0 = fmod( ori2(0,j) - ori1(0,i) , 2 );
                float o1 = fmod( ori2(1,j) - ori1(1,i) , 2 );
                float o2 = fmod( ori2(2,j) - ori1(2,i) , 2 );
                
                P(j) = sqrt( p0*p0 + p1*p1 + p2*p2 );
                O(j) = sqrt( o0*o0 + o1*o1 + o2*o2 );

								if (boost::math::isnan( O(j) ))
                    O(j) = 4;

								if ( (o0>2) || (o1>2) || (o2>2) )
                    std::cout << "Warning in bronchoscopyRegistration.cpp: Error on oriantation calculation in dsearch2n. Orientation > 2." << std::endl;

                R(j) = P(j) / O(j);

            }
            float alpha = sqrt( R.mean() );
						if (boost::math::isnan( alpha ))
                alpha = 0;

            D = P + alpha * O;
            D.minCoeff(&index);
            //std::cout << "index: " << index << std::endl;
            indexVector.push_back(index);
        }
        return indexVector;
    }

std::pair<Eigen::MatrixXd , Eigen::MatrixXd> RemoveInvalidData(Eigen::MatrixXd positionData, Eigen::MatrixXd orientationData)
{
	std::vector<int> indicesToBeDeleted;

	for (int i = 0; i < fmin(positionData.cols(), orientationData.cols()); i++)
	{
		if ( boost::math::isinf( positionData.block(0 , i , 3 , 1).sum() ) || boost::math::isinf( orientationData.block(0 , i , 3 , 1).sum() ) )
			indicesToBeDeleted.push_back(i);
		else if (boost::math::isnan( positionData.block(0 , i , 3 , 1).sum() ) || boost::math::isnan( orientationData.block(0 , i , 3 , 1).sum() ))
			indicesToBeDeleted.push_back(i);
		else if ( (positionData.block(0 , i , 1 , 1).sum() == 0 && positionData.block(1 , i , 1 , 1).sum() == 0 && positionData.block(2 , i , 1 , 1).sum() == 0) ||
				  (orientationData.block(0 , i , 1 , 1).sum() == 0 && orientationData.block(1 , i , 1 , 1).sum() == 0 && orientationData.block(2 , i , 1 , 1).sum() == 0))
			indicesToBeDeleted.push_back(i);
	}

	for ( int i = indicesToBeDeleted.size() - 1; i >= 0; i-- )
	{
		std::cout << "Warning in bronchoscopyRegistration: Removed corrupted data: " << positionData.block(0 , indicesToBeDeleted[i] , 3 , 1) << " " << orientationData.block(0 , indicesToBeDeleted[i] , 3 , 1) << std::endl;
		positionData = eraseCol(indicesToBeDeleted[i],positionData);
		orientationData = eraseCol(indicesToBeDeleted[i],orientationData);
	}
	return std::make_pair(positionData , orientationData);
}

M4Vector RemoveInvalidData(M4Vector T_vector)
{
	Eigen::Vector3d position;
	Eigen::Vector3d orientation;
	std::vector<int> indicesToBeDeleted;

	for (int i = 0; i < T_vector.size(); i++)
	{
	position = T_vector[i].topRightCorner(3 , 1);
	orientation = T_vector[i].block(0 , 2 , 3 , 1);
	if ( boost::math::isinf( position.sum() ) || boost::math::isinf( orientation.sum() ) )
		indicesToBeDeleted.push_back(i);
	else if (boost::math::isnan( position.sum() ) || boost::math::isnan( orientation.sum() ))
		indicesToBeDeleted.push_back(i);
	else if ( (position[0] == 0 && position[1] == 0 && position[2] == 0) ||
			  (orientation[0] == 0 && orientation[1] == 0 && orientation[2] == 0) )
		indicesToBeDeleted.push_back(i);
	}

	for ( int i = indicesToBeDeleted.size() - 1; i >= 0; i-- )
	{
		std::cout << "Warning in bronchoscopyRegistration: Removed corrupted data: " << T_vector[i].topRightCorner(3 , 1) << " " <<  T_vector[i].block(0 , 2 , 3 , 1) << std::endl;
		T_vector.erase(T_vector.begin() + indicesToBeDeleted[i]);
	}
	return T_vector;
}

Eigen::Matrix4d registrationAlgorithm(BranchListPtr branches, M4Vector Tnavigation, Transform3D old_rMpr)
{
	Eigen::Matrix4d registrationMatrix;
	Eigen::MatrixXd CTPositions;
	Eigen::MatrixXd CTOrientations;
	Eigen::MatrixXd trackingPositions(3 , Tnavigation.size());
	Eigen::MatrixXd trackingOrientations(3, Tnavigation.size());

	std::vector<BranchPtr> branchVector = branches->getBranches();
	CTPositions = branchVector[0]->getPositions();
	CTOrientations = branchVector[0]->getOrientations();

	if (trackingPositions.cols() < 10)
	{
		std::cout << "Warning: Too few positions in tracking data to perform registration." << std::endl;
		return Eigen::Matrix4d::Identity();
	}

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

	if (CTPositions.cols() < 10)
	{
		std::cout << "Warning: Too few positions in centerline to perform registration." << std::endl;
		return Eigen::Matrix4d::Identity();
	}

	std::pair<Eigen::MatrixXd , Eigen::MatrixXd> qualityCheckedData = RemoveInvalidData(CTPositions, CTOrientations);
	CTPositions = qualityCheckedData.first;
	CTOrientations = qualityCheckedData.second;

	for (int i = 0; i < Tnavigation.size(); i++)
		Tnavigation[i] = old_rMpr * Tnavigation[i];

	Tnavigation = RemoveInvalidData(Tnavigation);

	for (int i = 0; i < Tnavigation.size(); i++)
	{
		trackingPositions.block(0 , i , 3 , 1) = Tnavigation[i].topRightCorner(3 , 1);
		trackingOrientations.block(0 , i , 3 , 1) = Tnavigation[i].block(0 , 2 , 3 , 1);
	}

	//Adjusting points for centeroids
    Eigen::MatrixXd::Index maxIndex;
    trackingPositions.row(2).maxCoeff( &maxIndex );
    //std::cout << "maxIndex: " << maxIndex << std::endl;
    //Eigen::Vector3d translation = CTPositions.col(0) - trackingPositions.col(maxIndex);
    //std::cout << "CTPositions.col(0): " << CTPositions.col(0) << std::endl;
    Eigen::Vector3d translation = findMedian(CTPositions) - findMedian(trackingPositions);
    //Eigen::Matrix3d rotation;
    //trackingPositions = trackingPositions.colwise() + translation;
    //std::cout << "trackingPositions.col(maxIndex): " << trackingPositions.col(maxIndex) << std::endl;


    registrationMatrix << 1, 0, 0, translation(0),
                          0, 1, 0, translation(1),
                          0, 0, 1, translation(2),
						  0, 0, 0, 1;

    for (int i = 0; i < Tnavigation.size(); i++)
    {
        Tnavigation[i] = registrationMatrix * Tnavigation[i];
    }
    //std::cout << "Tracking data 1 after initial translation: " << Tnavigation[0] << std::endl;
    //std::cout << "Tracking data maxIndex after initial translation: " << Tnavigation[maxIndex] << std::endl;

    int iterationNumber = 0;
    int maxIterations = 50;
    while ( translation.array().abs().sum() > 1 && iterationNumber < maxIterations)
	{
        for (int i = 0; i < Tnavigation.size(); i++)
        {
            trackingPositions.block(0 , i , 3 , 1) = Tnavigation[i].topRightCorner(3 , 1);
            trackingOrientations.block(0 , i , 3 , 1) = Tnavigation[i].block(0 , 2 , 3 , 1);
        }


        iterationNumber++;
		std::vector<Eigen::MatrixXd::Index> indexVector = dsearch2n( trackingPositions, CTPositions, trackingOrientations, CTOrientations );
		Eigen::MatrixXd nearestCTPositions(3,indexVector.size());
		Eigen::MatrixXd nearestCTOrientations(3,indexVector.size());
		Eigen::VectorXd DAngle(indexVector.size());
		for (int i = 0; i < indexVector.size(); i++)
		{
			nearestCTPositions.col(i) = CTPositions.col(indexVector[i]);
			nearestCTOrientations.col(i) = CTOrientations.col(indexVector[i]);
			float o0 = fmod( trackingOrientations(0,i) - nearestCTOrientations(0,i) , 2 );
			float o1 = fmod( trackingOrientations(1,i) - nearestCTOrientations(1,i) , 2 );
			float o2 = fmod( trackingOrientations(2,i) - nearestCTOrientations(2,i) , 2 );
			DAngle(i) = sqrt(o0*o0+o1*o1+o2*o2);
		}

		std::pair<Eigen::MatrixXd , Eigen::MatrixXd> result = findPositionsWithSmallesAngleDifference(70 , DAngle , trackingPositions , nearestCTPositions);
        vtkPointsPtr trackingPositions_vtk = convertTovtkPoints(result.first);
		vtkPointsPtr CTPositions_vtk = convertTovtkPoints(result.second);

        Eigen::Matrix4d tempMatrix = performLandmarkRegistration(trackingPositions_vtk, CTPositions_vtk);

		registrationMatrix = tempMatrix * registrationMatrix;

        for (int i = 0; i < Tnavigation.size(); i++)
        {
            Tnavigation[i] = tempMatrix * Tnavigation[i];
        }

        translation << tempMatrix(0,3), tempMatrix(1,3), tempMatrix(2,3);

        std::cout << "Iteration nr " << iterationNumber << " translation: " << translation.array().abs().sum() << std::endl;
        //for (int i = 0; i < 4; i++)
        //    std::cout << tempMatrix.row(i) << std::endl;
	}

    if (translation.array().abs().sum() > 1)
        std::cout << "Warning: Registration did not converge within " << maxIterations <<" iterations, which is max number of iterations." << std::endl;

	return registrationMatrix;
}

Eigen::Matrix4d registrationAlgorithmImage2Image(BranchListPtr branchesFixed, BranchListPtr branchesMoving)
{
    Eigen::Matrix4d registrationMatrix;
    Eigen::MatrixXd CTPositionsFixed;
    Eigen::MatrixXd CTOrientationsFixed;
    Eigen::MatrixXd CTPositionsMoving;
    Eigen::MatrixXd CTOrientationsMoving;

    std::vector<BranchPtr> branchVectorFixed = branchesFixed->getBranches();
    CTPositionsFixed = branchVectorFixed[0]->getPositions();
    CTOrientationsFixed = branchVectorFixed[0]->getOrientations();

    std::vector<BranchPtr> branchVectorMoving = branchesMoving->getBranches();
    CTPositionsMoving = branchVectorMoving[0]->getPositions();
    CTOrientationsMoving = branchVectorMoving[0]->getOrientations();

    for (int i = 1; i < branchVectorFixed.size(); i++)
    {
        Eigen::MatrixXd CTPositionsFixedNew(CTPositionsFixed.rows() , CTPositionsFixed.cols() + branchVectorFixed[i]->getPositions().cols());
        Eigen::MatrixXd CTOrientationsFixedNew(CTOrientationsFixed.rows() , CTOrientationsFixed.cols() + branchVectorFixed[i]->getOrientations().cols());
        CTPositionsFixedNew.leftCols(CTPositionsFixed.cols()) = CTPositionsFixed;
        CTPositionsFixedNew.rightCols(branchVectorFixed[i]->getPositions().cols()) = branchVectorFixed[i]->getPositions();
        CTOrientationsFixedNew.leftCols(CTOrientationsFixed.cols()) = CTOrientationsFixed;
        CTOrientationsFixedNew.rightCols(branchVectorFixed[i]->getOrientations().cols()) = branchVectorFixed[i]->getOrientations();
        CTPositionsFixed.swap(CTPositionsFixedNew);
        CTOrientationsFixed.swap(CTOrientationsFixedNew);
    }

    for (int i = 1; i < branchVectorMoving.size(); i++)
    {
        Eigen::MatrixXd CTPositionsMovingNew(CTPositionsMoving.rows() , CTPositionsMoving.cols() + branchVectorMoving[i]->getPositions().cols());
        Eigen::MatrixXd CTOrientationsMovingNew(CTOrientationsMoving.rows() , CTOrientationsMoving.cols() + branchVectorMoving[i]->getOrientations().cols());
        CTPositionsMovingNew.leftCols(CTPositionsMoving.cols()) = CTPositionsMoving;
        CTPositionsMovingNew.rightCols(branchVectorMoving[i]->getPositions().cols()) = branchVectorMoving[i]->getPositions();
        CTOrientationsMovingNew.leftCols(CTOrientationsMoving.cols()) = CTOrientationsMoving;
        CTOrientationsMovingNew.rightCols(branchVectorMoving[i]->getOrientations().cols()) = branchVectorMoving[i]->getOrientations();
        CTPositionsMoving.swap(CTPositionsMovingNew);
        CTOrientationsMoving.swap(CTOrientationsMovingNew);
    }

    std::cout << "Positions in centerline fixed:" << CTPositionsFixed.cols() << std::endl;
    std::cout << "Positions in centerline moving:" << CTPositionsMoving.cols() << std::endl;

    if (CTPositionsFixed.cols() < 10 || CTPositionsMoving.cols() < 10)
    {
        std::cout << "Warning: Too few positions in centerline to perform registration." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

    std::pair<Eigen::MatrixXd , Eigen::MatrixXd> qualityCheckedDataFixed = RemoveInvalidData(CTPositionsFixed, CTOrientationsFixed);
    CTPositionsFixed = qualityCheckedDataFixed.first;
    CTOrientationsFixed = qualityCheckedDataFixed.second;

    std::pair<Eigen::MatrixXd , Eigen::MatrixXd> qualityCheckedDataMoving = RemoveInvalidData(CTPositionsMoving, CTOrientationsMoving);
    CTPositionsMoving = qualityCheckedDataMoving.first;
    CTOrientationsMoving = qualityCheckedDataMoving.second;

    //Adjusting points for centeroids
    Eigen::Vector3d translation = findMedian(CTPositionsFixed) - findMedian(CTPositionsMoving);

    registrationMatrix << 1, 0, 0, translation(0),
                          0, 1, 0, translation(1),
                          0, 0, 1, translation(2),
                          0, 0, 0, 1;

    for (int i = 0; i < CTPositionsMoving.cols(); i++)
    {
        CTPositionsMoving.col(i) = CTPositionsMoving.col(i) + translation;
    }

    int iterationNumber = 0;
    int maxIterations = 200;
    while ( translation.array().abs().sum() > 0.5 && iterationNumber < maxIterations)
    {

        iterationNumber++;
        std::vector<Eigen::MatrixXd::Index> indexVector = dsearch2n( CTPositionsMoving, CTPositionsFixed, CTOrientationsMoving, CTOrientationsFixed );
        Eigen::MatrixXd nearestCTPositions(3,indexVector.size());
        Eigen::MatrixXd nearestCTOrientations(3,indexVector.size());
        Eigen::VectorXd DAngle(indexVector.size());
        for (int i = 0; i < indexVector.size(); i++)
        {
            nearestCTPositions.col(i) = CTPositionsFixed.col(indexVector[i]);
            nearestCTOrientations.col(i) = CTOrientationsFixed.col(indexVector[i]);
            float o0 = fmod( CTOrientationsMoving(0,i) - nearestCTOrientations(0,i) , 2 );
            float o1 = fmod( CTOrientationsMoving(1,i) - nearestCTOrientations(1,i) , 2 );
            float o2 = fmod( CTOrientationsMoving(2,i) - nearestCTOrientations(2,i) , 2 );
            DAngle(i) = sqrt(o0*o0+o1*o1+o2*o2);
        }

        std::pair<Eigen::MatrixXd , Eigen::MatrixXd> result = findPositionsWithSmallesAngleDifference(70 , DAngle , CTPositionsMoving , nearestCTPositions);
        vtkPointsPtr CTPositionsMoving_vtk = convertTovtkPoints(result.first);
        vtkPointsPtr CTPositionsFixed_vtk = convertTovtkPoints(result.second);

        Eigen::Matrix4d tempMatrix = performLandmarkRegistration(CTPositionsMoving_vtk, CTPositionsFixed_vtk);

        registrationMatrix = tempMatrix * registrationMatrix;

        for (int i = 0; i < CTPositionsMoving.cols(); i++)
        {
            CTPositionsMoving.col(i) = tempMatrix.topLeftCorner(3,3) * CTPositionsMoving.col(i) + tempMatrix.topRightCorner(3,1);
        }

        translation << tempMatrix(0,3), tempMatrix(1,3), tempMatrix(2,3);

        std::cout << "Iteration nr " << iterationNumber << " translation: " << translation.array().abs().sum() << std::endl;
        //for (int i = 0; i < 4; i++)
        //    std::cout << tempMatrix.row(i) << std::endl;
    }

    if (translation.array().abs().sum() > 1)
        std::cout << "Warning: Registration did not converge within " << maxIterations <<" iterations, which is max number of iterations." << std::endl;

    return registrationMatrix;
}

vtkPolyDataPtr BronchoscopyRegistration::processCenterline(vtkPolyDataPtr centerline, Transform3D rMd, int numberOfGenerations)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

	int N = centerline->GetNumberOfPoints();
	Eigen::MatrixXd CLpoints(3,N);
	for(vtkIdType i = 0; i < N; i++)
		{
		double p[3];
		centerline->GetPoint(i,p);
		Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		CLpoints.block(0 , i , 3 , 1) = rMd.coord(position);
		}
	mBranchListPtr->findBranchesInCenterline(CLpoints);
	if (numberOfGenerations != 0)
	{
		mBranchListPtr->selectGenerations(numberOfGenerations);
	}

	mBranchListPtr->smoothBranchPositions();
	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();

	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	vtkPointsPtr points = vtkPointsPtr::New();
	vtkCellArrayPtr lines = vtkCellArrayPtr::New();

	std::vector<BranchPtr> branches = mBranchListPtr->getBranches();
	int positionCounter = 0;
	for (int i = 0; i < branches.size(); i++)
	{
		Eigen::MatrixXd positions = branches[i]->getPositions();
		for (int j = 0; j < positions.cols(); j++)
		{
			positionCounter ++;
			points->InsertNextPoint(positions(0,j),positions(1,j),positions(2,j));
			if (j	 < positions.cols()-1)
			{
				vtkIdType connection[2] = {positionCounter-1, positionCounter};
				lines->InsertNextCell(2, connection);
			}
		}
	}
	retval->SetPoints(points);
	retval->SetLines(lines);

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;

	mCenterlineProcessed = true;

	return retval;

}

BranchListPtr BronchoscopyRegistration::processCenterlineImage2Image(vtkPolyDataPtr centerline, int numberOfGenerations)
{
    BranchListPtr branchListPtr;
    branchListPtr = BranchListPtr(new BranchList());

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

    branchListPtr->findBranchesInCenterline(CLpoints);

    if (numberOfGenerations != 0)
    {
        branchListPtr->selectGenerations(numberOfGenerations);
    }

    branchListPtr->smoothBranchPositions();
    branchListPtr->calculateOrientations();
    branchListPtr->smoothOrientations();

    return branchListPtr;
}

Eigen::Matrix4d BronchoscopyRegistration::runBronchoscopyRegistration(TimedTransformMap trackingData_prMt, Transform3D old_rMpr, double maxDistanceForLocalRegistration)
{
    if(trackingData_prMt.empty())
		reportError("BronchoscopyRegistration::runBronchoscopyRegistration(): No tracking data");

	M4Vector Tnavigation;
    for(TimedTransformMap::iterator iter=trackingData_prMt.begin(); iter!=trackingData_prMt.end(); ++iter)
	{
		Tnavigation.push_back(iter->second.	matrix());
	}

    //vtkPointsPtr points = centerline->GetPoints();

	Tnavigation = excludeClosePositions(Tnavigation);

	Eigen::Matrix4d regMatrix;
	if (maxDistanceForLocalRegistration != 0)
	{
		Eigen::MatrixXd trackingPositions_temp(3 , Tnavigation.size());
		M4Vector Tnavigation_temp = Tnavigation;
		for (int i = 0; i < Tnavigation.size(); i++)
		{
			Tnavigation_temp[i] = old_rMpr * Tnavigation[i];
			trackingPositions_temp.block(0 , i , 3 , 1) = Tnavigation_temp[i].topRightCorner(3 , 1);
		}
		BranchListPtr tempPtr = mBranchListPtr->removePositionsForLocalRegistration(trackingPositions_temp, maxDistanceForLocalRegistration);
		regMatrix = registrationAlgorithm(tempPtr, Tnavigation, old_rMpr);
	}
	else
		regMatrix = registrationAlgorithm(mBranchListPtr, Tnavigation, old_rMpr);


	if ( boost::math::isnan(regMatrix.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'nan' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

		if ( boost::math::isinf(regMatrix.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'inf' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

    std::cout << "prMt from bronchoscopyRegistration: " << std::endl;
        for (int i = 0; i < 4; i++)
			std::cout << regMatrix.row(i) << std::endl;

	return regMatrix;
}

Eigen::Matrix4d BronchoscopyRegistration::runBronchoscopyRegistrationImage2Image(vtkPolyDataPtr centerlineFixed, vtkPolyDataPtr centerlineMoving)
{

    int numberOfGenerations = 4;
    Eigen::Matrix4d regMatrix;

    BranchListPtr branchesFixed;
    BranchListPtr branchesMoving;
    branchesFixed = processCenterlineImage2Image(centerlineFixed, numberOfGenerations);
    branchesMoving = processCenterlineImage2Image(centerlineMoving, numberOfGenerations);

    regMatrix = registrationAlgorithmImage2Image(branchesFixed, branchesMoving);


    if ( boost::math::isnan(regMatrix.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'nan' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

        if ( boost::math::isinf(regMatrix.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'inf' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

    std::cout << "prMt from bronchoscopyRegistration: " << std::endl;
        for (int i = 0; i < 4; i++)
            std::cout << regMatrix.row(i) << std::endl;

    return regMatrix;
}

bool BronchoscopyRegistration::isCenterlineProcessed()
{
	return mCenterlineProcessed;
}


BronchoscopyRegistration::~BronchoscopyRegistration()
{

}



}//namespace cx
