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
    DoublePropertyPtr getMaxDistanceToCenterlineOption();
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D prMd);
	void processCenterline(vtkPolyDataPtr centerline, Transform3D prMd);
	Transform3D findClosestPoint(Transform3D prMt, double maxDistance);
	Transform3D findClosestPointInBranches(Transform3D prMt, double maxDistance);
	Transform3D findClosestPointInSearchPositions(Transform3D prMt, double maxDistance);
	void findSearchPositions(double maxSearchDistance);
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	void searchBranchDown(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	Transform3D findProjectedPoint(Transform3D prMt, double maxDistance);
	bool isAdvancedCenterlineProjectionSelected();
	Transform3D updateProjectedCameraOrientation(Transform3D prMt, BranchPtr branch, int index);

private:
	bool isPreviousProjectedPointSet;
	Eigen::MatrixXd mCLpoints;
    DoublePropertyPtr mMaxDistanceToCenterline;
	BranchListPtr mBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	int mProjectedIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
	bool mUseAdvancedCenterlineProjection;
	Vector3D mProjectedViewDirection;
};

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXBRONCHOSCOPEPOSITIONPROJECTION_H
