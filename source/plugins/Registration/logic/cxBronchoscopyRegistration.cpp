/*
 * BronchoscopyRegistration.cpp
 *
 *  Created on: Jun 20, 2013
 *      Author: ehofstad
 */

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
#include "cxReporter.h"

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
	if(Tnavigation.empty())
		return TnavigationIncluded;
	TnavigationIncluded.push_back(Tnavigation[0]); // first position is always included
	int numberOfIncluded = 0;
	size_t numberOfPos = Tnavigation.size();
	for ( size_t index = 1; index <= numberOfPos; index++)
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

  if ( std::isnan(tar_M_src.sum()) )
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

                if (std::isnan( O(j) ))
                    O(j) = 4;

                if ( o0>2 | o1>2 | o2>2 )
                    std::cout << "Warning in bronchoscopyRegistration.cpp: Error on oriantation calculation in dsearch2n. Orientation > 2." << std::endl;

                R(j) = P(j) / O(j);

            }
            float alpha = sqrt( R.mean() );
            if (std::isnan( alpha ))
                alpha = 0;

            D = P + alpha * O;
            D.minCoeff(&index);
            //std::cout << "index: " << index << std::endl;
            indexVector.push_back(index);
        }
        return indexVector;
    }

Eigen::Matrix4d registrationAlgorithm(BranchList* branches, M4Vector Tnavigation, Transform3D old_rMpr)
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

    for (int i = 0; i < CTPositions.cols(); i++)  {
        if (CTPositions.block(0 , i , 1 , 1).sum() == 0 && CTPositions.block(1 , i , 1 , 1).sum() == 0 && CTPositions.block(2 , i , 1 , 1).sum() == 0)
             std::cout << "CL position number after sorting: " << i << ": " << CTPositions.col(i) << std::endl;
     }


    for (int i = 1; i < CTPositions.cols(); i++)
    {
        if (std::isinf( CTPositions.col(i).sum() ))
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed centerline position containing inf number: " << CTPositions.col(i) << std::endl;
            CTPositions = eraseCol(i,CTPositions);
            CTOrientations = eraseCol(i,CTOrientations);
        }
        else if (std::isnan( CTPositions.col(i).sum() ))
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed centerline position containing nan number: " << CTPositions.col(i) << std::endl;
            CTPositions = eraseCol(i,CTPositions);
            CTOrientations = eraseCol(i,CTOrientations);
        }
        else if (CTPositions.block(0 , i , 1 , 1).sum() == 0 && CTPositions.block(1 , i , 1 , 1).sum() == 0 && CTPositions.block(2 , i , 1 , 1).sum() == 0)
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed centerline position at origo: " << CTPositions.col(i) << std::endl;
            CTPositions = eraseCol(i,CTPositions);
            CTOrientations = eraseCol(i,CTOrientations);
        }

    }


    for (int i = 0; i < Tnavigation.size(); i++)
	{
        Tnavigation[i] = old_rMpr * Tnavigation[i];
		trackingPositions.block(0 , i , 3 , 1) = Tnavigation[i].topRightCorner(3 , 1);
		trackingOrientations.block(0 , i , 3 , 1) = Tnavigation[i].block(0 , 2 , 3 , 1);

        if ( std::isinf( trackingOrientations.block(0 , i , 3 , 1).sum() ) | std::isinf( trackingPositions.block(0 , i , 3 , 1).sum() ))
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed tool position containing inf number: " << trackingOrientations.block(0 , i , 3 , 1) << std::endl;
            trackingPositions = eraseCol(i,trackingPositions);
            trackingOrientations = eraseCol(i,trackingOrientations);
        }
        else if (std::isnan( trackingOrientations.block(0 , i , 3 , 1).sum() ) | std::isnan( trackingPositions.block(0 , i , 3 , 1).sum() ))
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed tool position containing nan number: " << trackingOrientations.block(0 , i , 3 , 1) << std::endl;
            trackingPositions = eraseCol(i,trackingPositions);
            trackingOrientations = eraseCol(i,trackingOrientations);
        }
        else if ( (trackingOrientations.block(0 , i , 1 , 1).sum() == 0 && trackingOrientations.block(1 , i , 1 , 1).sum() == 0 && trackingOrientations.block(2 , i , 1 , 1).sum() == 0) |
                  (trackingPositions.block(0 , i , 1 , 1).sum() == 0 && trackingPositions.block(1 , i , 1 , 1).sum() == 0 && trackingPositions.block(2 , i , 1 , 1).sum() == 0))
        {
            std::cout << "Warning in bronchoscopyRegistration: Removed tool position at origo: " << trackingOrientations.block(0 , i , 3 , 1) << std::endl;
            trackingPositions = eraseCol(i,trackingPositions);
            trackingOrientations = eraseCol(i,trackingOrientations);
        }
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



Eigen::Matrix4d BronchoscopyRegistration::runBronchoscopyRegistration(vtkPolyDataPtr centerline, TimedTransformMap trackingData_prMt, Transform3D old_rMpr, Transform3D rMd)
{

    if(trackingData_prMt.empty())
		reportError("BronchoscopyRegistration::runBronchoscopyRegistration(): No tracking data");

	M4Vector Tnavigation;
    for(TimedTransformMap::iterator iter=trackingData_prMt.begin(); iter!=trackingData_prMt.end(); ++iter)
	{
		Tnavigation.push_back(iter->second.	matrix());
	}


    //vtkPointsPtr points = centerline->GetPoints();

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

	Tnavigation = excludeClosePositions(Tnavigation);

	BranchList* branches = new BranchList();
	branches->findBranchesInCenterline(CLpoints);
	branches->calculateOrientations();
	branches->smoothOrientations();

    Eigen::Matrix4d regMatrix = registrationAlgorithm(branches, Tnavigation, old_rMpr);

	Eigen::Matrix4d regMatrixForCustusX = regMatrix;

	std::vector<Branch*> BL = branches->getBranches();

    std::cout << "Number of branches in CT centerline: " << BL.size() << std::endl;


    if ( std::isnan(regMatrixForCustusX.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'nan' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

    if ( std::isinf(regMatrixForCustusX.sum()) )
    {
        std::cout << "Warning: Registration matrix contains 'inf' number, using identity matrix." << std::endl;
        return Eigen::Matrix4d::Identity();
    }

    std::cout << "prMt from bronchoscopyRegistration: " << std::endl;
        for (int i = 0; i < 4; i++)
            std::cout << regMatrixForCustusX.row(i) << std::endl;

	return regMatrixForCustusX;


}

BronchoscopyRegistration::~BronchoscopyRegistration()
{

}



}//namespace cx
