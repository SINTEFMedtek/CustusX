#ifndef CXROUTETOTARGET_H
#define CXROUTETOTARGET_H

#include "cxMesh.h"
#include <QDomElement>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class RouteToTarget
{
public:
	RouteToTarget();
	RouteToTarget(vtkPolyDataPtr centerline, Transform3D prMd);
	virtual ~RouteToTarget();
	void setCenterline(vtkPolyDataPtr centerline, Transform3D prMd);
    Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline, Transform3D prMd);
	void processCenterline(vtkPolyDataPtr centerline, Transform3D prMd);
	void	 findClosestPointInBranches(Vector3D targetCoordinate);
	void findRoutePositions();
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex);
	vtkPolyDataPtr findRouteToTarget(Vector3D targetCoordinate);
	vtkPolyDataPtr addVTKPoints();

private:
	Eigen::MatrixXd mCLpoints;
	BranchListPtr mBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	int mProjectedIndex;
	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
};

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXROUTETOTARGET_H
