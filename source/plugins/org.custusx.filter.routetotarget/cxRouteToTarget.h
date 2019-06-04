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
	void processCenterline(MeshPtr mesh);
	void processBloodVesselCenterline(Eigen::MatrixXd positions);
	void findClosestPointInBranches(Vector3D targetCoordinate_r);
	void findClosestPointInBloodVesselBranches(Vector3D targetCoordinate_r);
	void findRoutePositions();
	void findRoutePositionsInBloodVessels();
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex);
	void searchBloodVesselBranchUp(BranchPtr searchBranchPtr, int startIndex);
	vtkPolyDataPtr findRouteToTarget(PointMetricPtr targetPoint);
	vtkPolyDataPtr findExtendedRoute(PointMetricPtr targetPoint);
	vtkPolyDataPtr findRouteToTargetAlongBloodVesselCenterlines(MeshPtr bloodVesselCenterlineMesh, PointMetricPtr targetPoint);
	vtkPolyDataPtr generateAirwaysFromBloodVesselCenterlines();
	vtkPolyDataPtr getConnectedAirwayAndBloodVesselRoute();
	vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
	std::vector< Eigen::Vector3d > getBranchPositions(BranchPtr branchPtr, int startIndex);
	void addRouteInformationToFile(VisServicesPtr services);
	double calculateRouteLength(std::vector< Eigen::Vector3d > route);
	void makeMarianaCenterlineFile(QString filename);
	QJsonArray makeMarianaCenterlineJSON();


private:
	Eigen::MatrixXd mCLpoints;
	BranchListPtr mBranchListPtr;
	BranchListPtr mBloodVesselBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	BranchPtr mProjectedBloodVesselBranchPtr;
	int mProjectedIndex;
	int mProjectedBloodVesselIndex;
	Vector3D mTargetPosition;
	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector< Eigen::Vector3d > mExtendedRoutePositions;
	std::vector< Eigen::Vector3d > mBloodVesselRoutePositions;
	std::vector< int > mBranchingIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
	Eigen::MatrixXd mConnectedPointsInBVCL;
	std::vector<Eigen::Vector3d> smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition);
	bool checkIfRouteToTargetEndsAtEndOfLastBranch();
};

Eigen::MatrixXd findLocalPointsInCT(int closestCLIndex , Eigen::MatrixXd CLpoints);
double findDistanceToLine(Eigen::MatrixXd point, Eigen::MatrixXd line);
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);

} /* namespace cx */

#endif // CXROUTETOTARGET_H
