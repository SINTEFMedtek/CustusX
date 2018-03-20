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


void BranchList::calculateOrientations()
{
	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		Eigen::MatrixXd diff = positions.rightCols(positions.cols() - 1) - positions.leftCols(positions.cols() - 1);
		Eigen::MatrixXd orientations(positions.rows(),positions.cols());
		orientations.leftCols(orientations.cols() - 1) = diff / diff.norm();
		orientations.rightCols(1) = orientations.col(orientations.cols() - 1);
		mBranches[i]->setOrientations(orientations);
	}
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

void BranchList::interpolateBranchPositions(int interpolationFactor){

	for (int i = 0; i < mBranches.size(); i++)
	{
		Eigen::MatrixXd positions = mBranches[i]->getPositions();
		std::vector<Eigen::Vector3d> interpolatedPositions;
		for (int j = 0; j < positions.cols()-1; j++)
		{
			for (int k = 0; k < interpolationFactor; k++){
				Eigen::Vector3d interpolationPoint;
				interpolationPoint[0] = (positions(0,j)*(interpolationFactor-k) + positions(0,j+1)*(k) ) / interpolationFactor;
				interpolationPoint[1] = (positions(1,j)*(interpolationFactor-k) + positions(1,j+1)*(k) ) / interpolationFactor;
				interpolationPoint[2] = (positions(2,j)*(interpolationFactor-k) + positions(2,j+1)*(k) ) / interpolationFactor;
				interpolatedPositions.push_back(interpolationPoint);
			}
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

void BranchList::findBranchesInCenterline(Eigen::MatrixXd positions_r)
{
    positions_r = sortMatrix(2,positions_r);
    Eigen::MatrixXd positionsNotUsed_r = positions_r;

//	int minIndex;
	int index;
	int splitIndex;
	Eigen::MatrixXd::Index startIndex;
	BranchPtr branchToSplit;
    while (positionsNotUsed_r.cols() > 0)
	{
		if (!mBranches.empty())
		{
			double minDistance = 1000;
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
            std::pair<Eigen::MatrixXd::Index, double> dsearchResult = dsearch(positionsNotUsed_r.col(startIndex) , branchToSplit->getPositions());
			splitIndex = dsearchResult.first;
		}
		else //if this is the first branch. Select the top position (Trachea).
            startIndex = positionsNotUsed_r.cols() - 1;

        std::pair<Eigen::MatrixXd,Eigen::MatrixXd > connectedPointsResult = findConnectedPointsInCT(startIndex , positionsNotUsed_r);
		Eigen::MatrixXd branchPositions = connectedPointsResult.first;
        positionsNotUsed_r = connectedPointsResult.second;

		if (branchPositions.cols() >= 5) //only include brances of length >= 5 points
		{
			BranchPtr newBranch = BranchPtr(new Branch());
			newBranch->setPositions(branchPositions);
			mBranches.push_back(newBranch);

			if (mBranches.size() > 1) // do not try to split another branch when the first branch is processed
			{
				if ((splitIndex + 1 >= 5) && (branchToSplit->getPositions().cols() - splitIndex - 1 >= 5))
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
				else if (splitIndex + 1 < 5)
					 // If the new branch is close to the start of the existing
					 // branch: Connect it to the same position start as the
					 // existing branch
				{
					newBranch->setParentBranch(branchToSplit->getParentBranch());
					if(branchToSplit->getParentBranch())
						branchToSplit->getParentBranch()->addChildBranch(newBranch);
				}
				else if (branchToSplit->getPositions().cols() - splitIndex - 1 < 5)
					// If the new branch is close to the end of the existing
					// branch: Connect it to the end of the existing branch
				{
					newBranch->setParentBranch(branchToSplit);
					branchToSplit->addChildBranch(newBranch);
				}

			}

		}
	}
}


BranchListPtr BranchList::removePositionsForLocalRegistration(Eigen::MatrixXd trackingPositions, double maxDistance)
{
	BranchListPtr retval = BranchListPtr(new BranchList());
	BranchPtr b;
	for (int i = 0; i < mBranches.size(); i++)
	{
		b = BranchPtr(new Branch());
		b->setPositions(mBranches[i]->getPositions());
		b->setOrientations(mBranches[i]->getOrientations());
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
		branches[i]->setOrientations(orientations);
	}
	return retval;
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
		if (d > 3) // more than 3 mm distance to closest point --> branch is compledted
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


}//namespace cx
