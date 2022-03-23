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
	void setBloodVesselVolume(ImagePtr bloodVesselVolume);
	//void setCenterline(vtkPolyDataPtr centerline);
	Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);
	void setSmoothing(bool smoothing);
	void processCenterline(MeshPtr mesh);
	void setBranchList(BranchListPtr branchList);
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
	bool makeConnectedAirwayAndBloodVesselRoute();
	vtkPolyDataPtr getConnectedAirwayAndBloodVesselRoute();
	vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
	void addRouteInformationToFile(VisServicesPtr services);
	static double calculateRouteLength(std::vector< Eigen::Vector3d > route);
	void setBloodVesselRadius();
	double calculateBloodVesselRadius(Eigen::Vector3d position, Eigen::Vector3d orientation);
	double findDistanceToSegmentationEdge(vtkImageDataPtr bloodVesselImage, Eigen::Vector3i indexVector, Eigen::Vector3d perpendicularVector, int* dim, double* spacing, int direction);
	void makeMarianaCenterlineFile(QString filename);
	QJsonArray makeMarianaCenterlineJSON();
	std::vector< Eigen::Vector3d > getRoutePositions();
	std::vector< double > getCameraRotation();

	double getTracheaLength();
	static std::vector<Eigen::Vector3d> getRoutePositions(MeshPtr route);


private:
	Eigen::MatrixXd mCLpoints;
	bool mSmoothing = true;
	BranchListPtr mBranchListPtr;
	BranchListPtr mBloodVesselBranchListPtr;
	BranchPtr mProjectedBranchPtr;
	BranchPtr mProjectedBloodVesselBranchPtr;
	int mProjectedIndex;
	int mProjectedBloodVesselIndex;
	ImagePtr mBloodVesselVolume;
	Vector3D mTargetPosition;
	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector< Eigen::Vector3d > mExtendedRoutePositions;
	std::vector< double > mCameraRotation;
    std::vector< double > mExtendedCameraRotation;
	std::vector< Eigen::Vector3d > mBloodVesselRoutePositions;
	std::vector< Eigen::Vector3d > mMergedAirwayAndBloodVesselRoutePositions;
	std::vector< int > mBranchingIndex;
	std::vector<BranchPtr> mSearchBranchPtrVector;
	std::vector<int> mSearchIndexVector;
	Eigen::MatrixXd mConnectedPointsInBVCL;
	bool checkIfRouteToTargetEndsAtEndOfLastBranch();
	bool mPathToBloodVesselsFound = false;
};

Eigen::MatrixXd findClosestBloodVesselSegments(Eigen::MatrixXd bloodVesselPositions , Eigen::MatrixXd airwayPositions, Vector3D targetPosition);
std::pair< Eigen::MatrixXd, Eigen::MatrixXd > findLocalPointsInCT(int closestCLIndex , Eigen::MatrixXd CLpoints);
std::pair<int, double> findDistanceFromPointToLine(Eigen::MatrixXd point, std::vector< Eigen::Vector3d > line);
std::vector< Eigen::Vector3d > getBranchPositions(BranchPtr branchPtr, int startIndex);
double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
Eigen::MatrixXd convertToEigenMatrix(std::vector< Eigen::Vector3d > positionsVector);
double variance(Eigen::VectorXd X);

org_custusx_filter_routetotarget_EXPORT QJsonArray makeMarianaCenterlineOfFullBranchTreeJSON(BranchListPtr branchList);

} /* namespace cx */

#endif // CXROUTETOTARGET_H
