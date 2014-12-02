#ifndef CXBRONCHOSCOPEPOSITIONPROJECTION_H
#define CXBRONCHOSCOPEPOSITIONPROJECTION_H

#include "cxMesh.h"
#include <QDomElement>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class BronchoscopePositionProjection> BronchoscopePositionProjectionPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class BronchoscopePositionProjection
{
public:
	BronchoscopePositionProjection();
    BronchoscopePositionProjection(vtkPolyDataPtr centerline, Transform3D prMd);
	virtual ~BronchoscopePositionProjection();
	void setCenterline(vtkPolyDataPtr centerline, Transform3D prMd, bool useAdvancedCenterlineProjection);
	void createMaxDistanceToCenterlineOption(QDomElement root);
	DoubleDataAdapterXmlPtr getMaxDistanceToCenterlineOption();
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D prMd);
	void processCenterline(vtkPolyDataPtr centerline, Transform3D rMd);
	Transform3D findClosestPoint(Transform3D prMt, double maxDistance);
	Transform3D findClosestPointInBranches(Transform3D prMd, double maxDistance);
	Transform3D findClosestPointInSearchPositions(Transform3D prMd, double maxDistance);
	void findSearchPositions(double maxSearchDistance);
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	void searchBranchDown(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	Transform3D findProjectedPoint(Transform3D prMd, double maxDistance);
	bool isAdvancedCenterlineProjectionSelected();

private:
	Transform3D mPreviousProjectedPoint;
	bool isPreviousProjectedPointSet;
	Eigen::MatrixXd mCLpoints;
	DoubleDataAdapterXmlPtr mMaxDistanceToCenterline;
	BranchListPtr mBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	int mProjectedIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
	bool mUseAdvancedCenterlineProjection;
};

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXBRONCHOSCOPEPOSITIONPROJECTION_H
