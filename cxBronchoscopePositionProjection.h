#ifndef CXBRONCHOSCOPEPOSITIONPROJECTION_H
#define CXBRONCHOSCOPEPOSITIONPROJECTION_H

#include "org_custusx_bronchoscopynavigation_Export.h"
#include "cxMesh.h"
#include <QDomElement>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class BronchoscopePositionProjection> BronchoscopePositionProjectionPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class org_custusx_bronchoscopynavigation_EXPORT BronchoscopePositionProjection
{
public:
	BronchoscopePositionProjection();
    BronchoscopePositionProjection(vtkPolyDataPtr centerline, Transform3D prMd);
		void setRunFromWidget(bool runFromWidget);
	virtual ~BronchoscopePositionProjection();
	void setAdvancedCenterlineOption(bool useAdvancedCenterlineProjection);
	void createMaxDistanceToCenterlineOption(QDomElement root);
	DoublePropertyPtr getMaxDistanceToCenterlineOption();
	double getMaxDistanceToCenterlineValue();
	void createMaxSearchDistanceOption(QDomElement root);
	DoublePropertyPtr getMaxSearchDistanceOption();
	double getMaxSearchDistanceValue();
	void createAlphaOption(QDomElement root);
	DoublePropertyPtr getAlphaOption();
	double getAlphaValue();
	Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D rMd);
	void processCenterline(vtkPolyDataPtr centerline, Transform3D rMd, Transform3D rMpr);
	void setBranchList(BranchListPtr branchList, Transform3D rMpr);
	void setMaxDistanceToCenterline(double maxDistance);
	void setMaxSearchDistance(double maxDistance);
	void setAlpha(double alpha);
	Transform3D findClosestPoint(Transform3D prMt, double maxDistance);
	Transform3D findClosestPointInBranches(Transform3D prMt, double maxDistance);
	Transform3D findClosestPointInSearchPositions(Transform3D prMt, double maxDistance);
	void findSearchPositions(double maxSearchDistance);
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	void searchBranchDown(BranchPtr searchBranchPtr, int startIndex, double currentSearchDistance, double maxSearchDistance);
	Transform3D findProjectedPoint(Transform3D prMt, double maxDistance, double maxSearchDistance);
	bool isAdvancedCenterlineProjectionSelected();
	Transform3D updateProjectedCameraOrientation(Transform3D prMt, BranchPtr branch, int index);

private:
	bool isPreviousProjectedPointSet;
	Eigen::MatrixXd mCLpoints;
	DoublePropertyPtr mMaxDistanceToCenterline;
	DoublePropertyPtr mMaxSearchDistance;
	DoublePropertyPtr mAlpha;
	double mMaxDistanceToCenterlineValue;
	double mMaxSearchDistanceValue;
	double mAlphaValue;
	BranchListPtr mBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	int mProjectedIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
	bool mUseAdvancedCenterlineProjection;
	Vector3D mProjectedViewDirection;
	Transform3D m_rMpr;
	bool mRunFromWidget = true;
};
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
double findDistanceWithOrientation(Eigen::VectorXd p1, Eigen::VectorXd p2, Eigen::VectorXd o1, Eigen::VectorXd o2, double alpha);

} /* namespace cx */

#endif // CXBRONCHOSCOPEPOSITIONPROJECTION_H
