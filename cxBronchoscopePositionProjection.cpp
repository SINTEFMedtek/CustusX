

#include "cxBronchoscopePositionProjection.h"
#include <vtkPolyData.h>
#include "cxDoubleProperty.h"
#include "cxBranchList.h"
#include "cxBranch.h"

namespace cx
{

BronchoscopePositionProjection::BronchoscopePositionProjection():
	isPreviousProjectedPointSet(false),
	mBranchListPtr(new BranchList),
	mProjectedIndex(0),
	mUseAdvancedCenterlineProjection(false)
{
}

BronchoscopePositionProjection::BronchoscopePositionProjection(vtkPolyDataPtr centerline, Transform3D prMd):
	isPreviousProjectedPointSet(false),
	mProjectedIndex(0),
	mUseAdvancedCenterlineProjection(false)
{
    mCLpoints = this->getCenterlinePositions(centerline, prMd);
}

BronchoscopePositionProjection::~BronchoscopePositionProjection()
{
}

void BronchoscopePositionProjection::setAdvancedCenterlineOption(bool useAdvancedCenterlineProjection)
{
	mUseAdvancedCenterlineProjection = useAdvancedCenterlineProjection;
}

void BronchoscopePositionProjection::createMaxDistanceToCenterlineOption(QDomElement root)
{
    mMaxDistanceToCenterline = DoubleProperty::initialize("Max distance to centerline (mm)", "",
	"Set max distance to centerline in mm", 30, DoubleRange(1, 100, 1), 0,
					root);
    mMaxDistanceToCenterline->setGuiRepresentation(DoubleProperty::grSLIDER);
}

DoublePropertyPtr BronchoscopePositionProjection::getMaxDistanceToCenterlineOption()
{

	return mMaxDistanceToCenterline;
}

Eigen::MatrixXd BronchoscopePositionProjection::getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D rMd)
{

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
	return CLpoints;
}

void BronchoscopePositionProjection::processCenterline(vtkPolyDataPtr centerline, Transform3D rMd, Transform3D rMpr)
{
	m_rMpr = rMpr;
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

	mCLpoints = getCenterlinePositions(centerline, rMd);

	mBranchListPtr->findBranchesInCenterline(mCLpoints);
	//mBranchListPtr->interpolateBranchPositions(10);
    //mBranchListPtr->smoothBranchPositions();
	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}


Transform3D BronchoscopePositionProjection::findClosestPoint(Transform3D prMt, double maxDistance)
{
	Transform3D rMt = m_rMpr * prMt;
	Eigen::VectorXd toolPos  = rMt.matrix().topRightCorner(3 , 1);
	Eigen::MatrixXd::Index index;
	Transform3D new_rMt = rMt;

		Eigen::VectorXd P(mCLpoints.cols());
		for (int i = 0; i < mCLpoints.cols(); i++)
		{
			double p0 = ( mCLpoints(0,i) - toolPos(0) );
			double p1 = ( mCLpoints(1,i) - toolPos(1) );
			double p2 = ( mCLpoints(2,i) - toolPos(2) );

			P(i) = sqrt( p0*p0 + p1*p1 + p2*p2 );
		}

		P.minCoeff(&index);
		if (P.minCoeff() < maxDistance)
		{
			new_rMt.matrix().topRightCorner(3 , 1) = mCLpoints.col(index);
		}

	Transform3D new_prMt = m_rMpr.inverse() * new_rMt;
	return new_prMt;
}

Transform3D BronchoscopePositionProjection::findClosestPointInBranches(Transform3D prMt, double maxDistance)
{
	Transform3D rMt = m_rMpr * prMt;
	Eigen::VectorXd toolPos  = rMt.matrix().topRightCorner(3 , 1);
	Transform3D new_rMt = rMt;

	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	std::vector<BranchPtr> branches = mBranchListPtr->getBranches();
	for (int i = 0; i < branches.size(); i++)
	{
		Eigen::MatrixXd positions = branches[i]->getPositions();
		for (int j = 0; j < positions.cols(); j++)
		{
            double D = findDistance(positions.col(j), toolPos);
			if (D < minDistance)
			{
				minDistance = D;
				minDistanceBranch = branches[i];
				minDistancePositionIndex = j;
			}
		}
	}

		if (minDistance < maxDistance)
		{
			Eigen::MatrixXd positions = minDistanceBranch->getPositions();
			new_rMt.matrix().topRightCorner(3 , 1) = positions.col(minDistancePositionIndex);
			mProjectedBranchPtr = minDistanceBranch;
			mProjectedIndex = minDistancePositionIndex;
			isPreviousProjectedPointSet = true;
			new_rMt = updateProjectedCameraOrientation(new_rMt, mProjectedBranchPtr, mProjectedIndex);
		}
		else
		{
			isPreviousProjectedPointSet = false;
		}

	Transform3D new_prMt = m_rMpr.inverse() * new_rMt;
	return new_prMt;
}

Transform3D BronchoscopePositionProjection::findClosestPointInSearchPositions(Transform3D prMt, double maxDistance)
{
	Transform3D rMt = m_rMpr * prMt;
	Eigen::VectorXd toolPos  = rMt.matrix().topRightCorner(3 , 1);
	Transform3D new_rMt = rMt;

	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	for (int i = 0; i < mSearchBranchPtrVector.size(); i++)
	{
		Eigen::MatrixXd positions = mSearchBranchPtrVector[i]->getPositions();

        double D = findDistance(positions.col(mSearchIndexVector[i]), toolPos);
			if (D < minDistance)
			{
				minDistance = D;
                minDistanceBranch = mSearchBranchPtrVector[i];
				minDistancePositionIndex = mSearchIndexVector[i];
			}
	}

	if (minDistance < maxDistance)
	{
		Eigen::MatrixXd positions = minDistanceBranch->getPositions();
		new_rMt.matrix().topRightCorner(3 , 1) = positions.col(minDistancePositionIndex);
		mProjectedBranchPtr = minDistanceBranch;
		mProjectedIndex = minDistancePositionIndex;
		isPreviousProjectedPointSet = true;
		new_rMt = updateProjectedCameraOrientation(new_rMt, mProjectedBranchPtr, mProjectedIndex);
	}
	else
	{
		isPreviousProjectedPointSet = false;
	}

	Transform3D new_prMt = m_rMpr.inverse() * new_rMt;
	return new_prMt;

}

void BronchoscopePositionProjection::findSearchPositions(double maxSearchDistance)
{
	mSearchBranchPtrVector.clear();
	mSearchIndexVector.clear();
	mSearchBranchPtrVector.push_back(mProjectedBranchPtr);
	mSearchIndexVector.push_back(mProjectedIndex);
	double currentSearchDistance = 0;

	Eigen::MatrixXd positions = mProjectedBranchPtr->getPositions();
	if (mProjectedIndex < positions.cols() - 1)
	{
        currentSearchDistance = findDistance( positions.col(mProjectedIndex), positions.col(mProjectedIndex+1) );
		searchBranchDown(mProjectedBranchPtr, mProjectedIndex + 1, currentSearchDistance, maxSearchDistance);
	}
	if (mProjectedIndex > 0)
	{
        currentSearchDistance = findDistance( positions.col(mProjectedIndex), positions.col(mProjectedIndex-1) );
		searchBranchUp(mProjectedBranchPtr, mProjectedIndex - 1, currentSearchDistance, maxSearchDistance);
	}

}

void BronchoscopePositionProjection::searchBranchUp(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance)
{
	Eigen::MatrixXd positions = searchBranchPtr->getPositions();
	mSearchBranchPtrVector.push_back(searchBranchPtr);
	mSearchIndexVector.push_back(startIndex);
	for (int i = startIndex-1; i>=0; i--)
	{
        currentSearchDistance = currentSearchDistance + findDistance( positions.col(i+1), positions.col(i) );
		if (currentSearchDistance < maxSearchDistance)
		{
			mSearchBranchPtrVector.push_back(searchBranchPtr);
			mSearchIndexVector.push_back(i);
		}
		else
			return;
	}
	BranchPtr parentBranchPtr = searchBranchPtr->getParentBranch();

    if (parentBranchPtr)
    {
        std::vector<BranchPtr> childBranches = parentBranchPtr->getChildBranches();
        searchBranchUp(parentBranchPtr, parentBranchPtr->getPositions().cols()-1, currentSearchDistance, maxSearchDistance);
        for (int i = 0; i < childBranches.size(); i++)
            if (childBranches[i] != searchBranchPtr)
                searchBranchDown(childBranches[i], 0, currentSearchDistance, maxSearchDistance);
    }
}

void BronchoscopePositionProjection::searchBranchDown(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance)
{
	Eigen::MatrixXd positions = searchBranchPtr->getPositions();
	mSearchBranchPtrVector.push_back(searchBranchPtr);
	mSearchIndexVector.push_back(startIndex);
	for (int i = startIndex+1; i<positions.cols(); i++)
	{
        currentSearchDistance = currentSearchDistance + findDistance( positions.col(i), positions.col(i-1) );
		if (currentSearchDistance < maxSearchDistance)
		{
			mSearchBranchPtrVector.push_back(searchBranchPtr);
			mSearchIndexVector.push_back(i);
		}
		else
			return;
	}
	std::vector<BranchPtr> childBranches = searchBranchPtr->getChildBranches();
	for (int i = 0; i < childBranches.size(); i++)
		searchBranchDown(childBranches[i], 0,currentSearchDistance,maxSearchDistance);
}

Transform3D BronchoscopePositionProjection::findProjectedPoint(Transform3D prMt, double maxDistance)
{
	Transform3D rMt = m_rMpr * prMt;
    double maxSearchDistance = 25; //mm
	Transform3D new_rMt;
	if (isPreviousProjectedPointSet)
	{
		findSearchPositions(maxSearchDistance);
		new_rMt = findClosestPointInSearchPositions(rMt, maxDistance);
	}
	else
		new_rMt = findClosestPointInBranches(rMt, maxDistance);

	Transform3D new_prMt = m_rMpr.inverse() * new_rMt;
	return new_prMt;
}

bool BronchoscopePositionProjection::isAdvancedCenterlineProjectionSelected()
{
	return mUseAdvancedCenterlineProjection;
}

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2)
{
	double d0 = p1(0) - p2(0);
	double d1 = p1(1) - p2(1);
	double d2 = p1(2) - p2(2);

	double D = sqrt( d0*d0 + d1*d1 + d2*d2 );

	return D;
}

Transform3D BronchoscopePositionProjection::updateProjectedCameraOrientation(Transform3D prMt, BranchPtr branch, int index)
{
	Eigen::MatrixXd branchPositions = branch->getPositions();
	int numberOfPositions = branchPositions.cols();
	int lookBackIndex; // = std::max(0 , index-20);
	Vector3D lookBackPosition;
//	if (index >=20){
//		lookBackIndex = index-20;
//		lookBackPosition = branchPositions.col(lookBackIndex);
//	}
//	else if (branch->getParentBranch())
//	{
//		Eigen::MatrixXd parentBranchPositions = branch->getParentBranch()->getPositions();
//		lookBackIndex = parentBranchPositions.cols() - 1 - index - 20;
//		lookBackIndex = std::max(0 , index-20);
//	}
//	else
//	{
//		lookBackIndex = 0;
//		lookBackPosition = branchPositions.col(lookBackIndex);
//	}

    lookBackIndex = std::max(0 , index-20);
    lookBackPosition = branchPositions.col(lookBackIndex);
    int lookForwardIndex = std::min(numberOfPositions-1 , index+20);
	Vector3D lookForwardPosition = branchPositions.col(lookForwardIndex);

	Vector3D viewDirection = (lookForwardPosition - lookBackPosition).normalized();

    prMt.matrix().col(2).head(3) = viewDirection;

	return prMt;
}

} /* namespace cx */
