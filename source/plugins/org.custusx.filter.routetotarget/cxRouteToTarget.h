#ifndef CXROUTETOTARGET_H
#define CXROUTETOTARGET_H

#include "org_custusx_filter_routetotarget_Export.h"

#include "cxMesh.h"
#include <QDomElement>
#include "cxForwardDeclarations.h"


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;
typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;

class org_custusx_filter_routetotarget_EXPORT RouteToTarget
{
public:
	RouteToTarget();
	virtual ~RouteToTarget();
	void setBloodVesselVolume(ImagePtr bloodVesselVolume);
	//void setCenterline(vtkPolyDataPtr centerline);
	void setSmoothing(bool smoothing);
	void processCenterline(MeshPtr mesh);
	void setBranchList(BranchListPtr branchList);
	BranchListPtr getBranchList();
	void processBloodVesselCenterline(Eigen::MatrixXd positions);
	void findClosestPointInBranches(Vector3D targetCoordinate_r);
	void findClosestPointInBloodVesselBranches(Vector3D targetCoordinate_r);
	void findRoutePositions(std::vector< Eigen::Vector3d > initialRoutePositions = std::vector< Eigen::Vector3d >());
	void findRoutePositionsInBloodVessels();
	void searchBranchUp(BranchPtr searchBranchPtr, int startIndex);
	void searchBloodVesselBranchUp(BranchPtr searchBranchPtr, int startIndex);
	vtkPolyDataPtr findRouteToTarget(PointMetricPtr targetPoint, QString lobeName = "", std::map<QString, PointMetricPtr> extraAirwayPoints = std::map<QString, PointMetricPtr>());
	std::vector<Eigen::Vector3d> findRouteAlongExtraPoints(Vector3D targetPosition, std::map<QString, PointMetricPtr> extraAirwayPoints);
	vtkPolyDataPtr findExtendedRoute(PointMetricPtr targetPoint);
	vtkPolyDataPtr findRouteToTargetAlongBloodVesselCenterlines(MeshPtr bloodVesselCenterlineMesh, PointMetricPtr targetPoint);
	vtkPolyDataPtr generateAirwaysFromBloodVesselCenterlines();
	bool makeConnectedAirwayAndBloodVesselRoute();
	vtkPolyDataPtr getConnectedAirwayAndBloodVesselRoute();
	vtkPolyDataPtr addVTKPoints(std::vector< Eigen::Vector3d > positions);
	void addRouteInformationToFile(VisServicesPtr services);
	static double calculateRouteLength(std::vector< Eigen::Vector3d > route);
	void makeMarianaCenterlineFile(QString filename);
	QJsonArray makeMarianaCenterlineJSON();
	void limitCameraRotation(int maxGenerationNumber);
	std::vector< Eigen::Vector3d > getRoutePositions(bool extendedRoute = true);
	std::vector< BranchPtr > getRouteBranches();
	std::vector< double > getCameraRotation();
	std::vector< int > getGenerationNumbers();
	std::vector< double > getRadius();
	std::vector< int > getBranchingIndex();

	double getTracheaLength();
	static std::vector<Eigen::Vector3d> getRoutePositions(MeshPtr route);


private:
	std::vector<Eigen::Vector3d> insertAndinterpolate(std::vector<Eigen::Vector3d> routePositions, Vector3D newPosition, double interpolationDistance);

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
	QString mLobeName;
	Vector3D mEndPointAlongCenterline;
	std::vector< Eigen::Vector3d > mExtraAirwayPoints;
	std::vector< Eigen::Vector3d > mRoutePositions;
	std::vector< Eigen::Vector3d > mExtendedRoutePositions;
	std::vector<BranchPtr> mRoutePositionsBranch;
	std::vector< double > mCameraRotation;
	std::vector< double > mExtendedCameraRotation;
	std::vector< int > mGenerationNumber;
	std::vector< int > mExtendedGenerationNumber;
	std::vector< double > mRadius;
	std::vector< double > mExtendedRadius;
	std::vector< Eigen::Vector3d > mBloodVesselRoutePositions;
	std::vector< Eigen::Vector3d > mMergedAirwayAndBloodVesselRoutePositions;
	std::vector< int > mBranchingIndex;
	std::vector<int> mSearchIndexVector;
	Eigen::MatrixXd mConnectedPointsInBVCL;
	bool checkIfRouteToTargetEndsAtEndOfLastBranch();
	bool mPathToBloodVesselsFound = false;
};

Eigen::MatrixXd findClosestBloodVesselSegments(Eigen::MatrixXd bloodVesselPositions , Eigen::MatrixXd airwayPositions, Vector3D targetPosition);
std::pair< Eigen::MatrixXd, Eigen::MatrixXd > findLocalPointsInCT(int closestCLIndex , Eigen::MatrixXd CLpoints);
std::vector< Eigen::Vector3d > getBranchPositions(BranchPtr branchPtr, int startIndex);
Eigen::MatrixXd convertToEigenMatrix(std::vector< Eigen::Vector3d > positionsVector);
double variance(Eigen::VectorXd X);

org_custusx_filter_routetotarget_EXPORT QJsonArray makeMarianaCenterlineOfFullBranchTreeJSON(BranchListPtr branchList);
org_custusx_filter_routetotarget_EXPORT double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
org_custusx_filter_routetotarget_EXPORT std::pair<int, double> findDistanceFromPointToLine(Eigen::MatrixXd point, std::vector< Eigen::Vector3d > line);
org_custusx_filter_routetotarget_EXPORT	Eigen::MatrixXd getCenterlinePositions(vtkPolyDataPtr centerline_r);

} /* namespace cx */

#endif // CXROUTETOTARGET_H
