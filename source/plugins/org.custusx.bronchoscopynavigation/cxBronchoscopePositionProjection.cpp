

#include "cxBronchoscopePositionProjection.h"
#include <vtkPolyData.h>
#include "cxDoubleProperty.h"
#include "cxBranchList.h"
#include "cxBranch.h"
#include <boost/math/special_functions/fpclassify.hpp> // isnan
#include "cxLogger.h"


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

void BronchoscopePositionProjection::setRunFromWidget(bool runFromWidget)
{
	mRunFromWidget = runFromWidget;
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

double BronchoscopePositionProjection::getMaxDistanceToCenterlineValue()
{
	if (mRunFromWidget && mMaxDistanceToCenterline->getValue())
		mMaxDistanceToCenterlineValue = mMaxDistanceToCenterline->getValue();

	return mMaxDistanceToCenterlineValue;
}

void BronchoscopePositionProjection::createMaxSearchDistanceOption(QDomElement root)
{
    mMaxSearchDistance = DoubleProperty::initialize("Max search distance along centerline (mm)", "",
    "Set max search distance along centerline in mm", 20, DoubleRange(1, 100, 1), 0,
                    root);
    mMaxSearchDistance->setGuiRepresentation(DoubleProperty::grSLIDER);
}

DoublePropertyPtr BronchoscopePositionProjection::getMaxSearchDistanceOption()
{
	return mMaxSearchDistance;
}

double BronchoscopePositionProjection::getMaxSearchDistanceValue()
{
	if (mRunFromWidget && mMaxSearchDistance->getValue())
		mMaxSearchDistanceValue = mMaxSearchDistance->getValue();

	return mMaxSearchDistanceValue;
}

void BronchoscopePositionProjection::createAlphaOption(QDomElement root)
{
    mAlpha = DoubleProperty::initialize("Alpha ", "",
    "Weighting of position and orientation in projection (high alpha is giving orientations high weight).", 1.0, DoubleRange(0, 10, 0.1), 1, root);
    mAlpha->setGuiRepresentation(DoubleProperty::grSLIDER);
}

DoublePropertyPtr BronchoscopePositionProjection::getAlphaOption()
{
	return mAlpha;
}

double BronchoscopePositionProjection::getAlphaValue()
{
	if (mRunFromWidget && mAlpha->getValue())
		mAlphaValue = mAlpha->getValue();

	return mAlphaValue;
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
	mBranchListPtr->smoothOrientations();

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}

//Can be used instead of processCenterline(...) if you have a preprosessed branchList to be used in the registration process.
void BronchoscopePositionProjection::setBranchList(BranchListPtr branchList, Transform3D rMpr)
{
	if (!branchList)
		return;

	m_rMpr = rMpr;
	mBranchListPtr = branchList;
}

//Used when run without widget
void BronchoscopePositionProjection::setMaxDistanceToCenterline(double maxDistance)
{
	mMaxDistanceToCenterlineValue = maxDistance;
}

//Used when run without widget
void BronchoscopePositionProjection::setMaxSearchDistance(double maxDistance)
{
	mMaxSearchDistanceValue = maxDistance;
}

//Used when run without widget
void BronchoscopePositionProjection::setAlpha(double alpha)
{
	mAlphaValue = alpha;
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
            //new_rMt = updateProjectedCameraOrientation(new_rMt, mProjectedBranchPtr, mProjectedIndex);
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
    Eigen::VectorXd toolOrientation  = rMt.matrix().block(0 , 2 , 3 , 1);
	Transform3D new_rMt = rMt;

	double minDistance = 100000;
	int minDistancePositionIndex;
	BranchPtr minDistanceBranch;
	for (int i = 0; i < mSearchBranchPtrVector.size(); i++)
	{
		Eigen::MatrixXd positions = mSearchBranchPtrVector[i]->getPositions();
		Eigen::MatrixXd orientations = mSearchBranchPtrVector[i]->getOrientations();

		//double D = findDistance(positions.col(mSearchIndexVector[i]), toolPos);
		double alpha = getAlphaValue();
		double D = findDistanceWithOrientation(positions.col(mSearchIndexVector[i]), toolPos, orientations.col(mSearchIndexVector[i]), toolOrientation, alpha);
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
        //new_rMt = updateProjectedCameraOrientation(new_rMt, mProjectedBranchPtr, mProjectedIndex);
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

Transform3D BronchoscopePositionProjection::findProjectedPoint(Transform3D prMt, double maxDistance, double maxSearchDistance)
{
    //double maxSearchDistance = 25; //mm
    Transform3D new_prMt;
	if (isPreviousProjectedPointSet)
	{
		findSearchPositions(maxSearchDistance);
        new_prMt = findClosestPointInSearchPositions(prMt, maxDistance);
	}
	else
        new_prMt = findClosestPointInBranches(prMt, maxDistance);

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

double findDistanceWithOrientation(Eigen::VectorXd position1, Eigen::VectorXd position2, Eigen::VectorXd orientation1, Eigen::VectorXd orientation2, double alpha)
{
    double d0 = position1(0) - position2(0);
    double d1 = position1(1) - position2(1);
    double d2 = position1(2) - position2(2);
    double o0 = fmod( orientation2(0) - orientation1(0) , 2 );
    double o1 = fmod( orientation2(1) - orientation1(1) , 2 );
    double o2 = fmod( orientation2(2) - orientation1(2) , 2 );

    double P = sqrt( d0*d0 + d1*d1 + d2*d2 );
    double O = sqrt( o0*o0 + o1*o1 + o2*o2 );

    if (boost::math::isnan( O ) )
        O = 4;

    if ( (o0>2) || (o1>2) || (o2>2) )
        std::cout << "Warning in bronchoscopyRegistration.cpp: Error on oriantation calculation in dsearch2n. Orientation > 2." << std::endl;

    double D = P + alpha * O;

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
