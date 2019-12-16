#ifndef CXROUTETOTARGET_H
#define CXROUTETOTARGET_H

#include "org_custusx_filter_routetotarget_Export.h"

#include "cxMesh.h"
#include <QDomElement>


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;
typedef boost::shared_ptr<class BranchList> BranchListPtr;
typedef boost::shared_ptr<class Branch> BranchPtr;


class org_custusx_filter_routetotarget_EXPORT RouteToTarget
{
public:
	RouteToTarget();
	virtual ~RouteToTarget();
	//void setCenterline(vtkPolyDataPtr centerline);
	Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);
	void setSmoothing(bool smoothing);
	void processCenterline(MeshPtr mesh);
	void setBranchList(BranchListPtr branchList);
	void findClosestPointInBranches(Vector3D targetCoordinate_r);
	void findRoutePositions();
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex);
	vtkPolyDataPtr findRouteToTarget(PointMetricPtr targetPoint);
	vtkPolyDataPtr findExtendedRoute(PointMetricPtr targetPoint);
	vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
	void addRouteInformationToFile(VisServicesPtr services);
	double calculateRouteLength(std::vector< Eigen::Vector3d > route);
	void makeMarianaCenterlineFile(QString filename);
	QJsonArray makeMarianaCenterlineJSON();


private:
	Eigen::MatrixXd mCLpoints;
	bool mSmoothing = true;
	BranchListPtr mBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	int mProjectedIndex;
	Vector3D mTargetPosition;
	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector< Eigen::Vector3d > mExtendedRoutePositions;
	std::vector< int > mBranchingIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
};

org_custusx_filter_routetotarget_EXPORT QJsonArray makeMarianaCenterlineOfFullBranchTreeJSON(BranchListPtr branchList);
std::vector< Eigen::Vector3d > getBranchPositions(BranchPtr branchPtr, int startIndex);
org_custusx_filter_routetotarget_EXPORT std::vector<Eigen::Vector3d> smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition);
double findDistanceToLine(Eigen::MatrixXd point, Eigen::MatrixXd line);
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXROUTETOTARGET_H
