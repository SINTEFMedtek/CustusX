/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef BRANCHLIST_H_
#define BRANCHLIST_H_

#include "cxBranch.h"
#include "cxForwardDeclarations.h"
#include "cxMesh.h"
#include "cxVector3D.h"
#include "org_custusx_registration_method_bronchoscopy_Export.h"


namespace cx
{

typedef std::vector< Eigen::Matrix4d > M4Vector;

#define MAX_ROTATION_TO_TILT_DOWN_DEGREES 30 //Threshold between rotation direction of scope to tilt up or down in a bifurcation.
#define MAX_DISTANCE_BETWEEN_CONNECTED_POINTS_IN_BRANCH 3 //mm
#define MIN_BRANCH_SEGMENT_LENGTH 5 //Number of connected points
#define MAX_DISTANCE_TO_EXISTING_BRANCH 20 //mm. Max distance to main airway tree for inclusion of points.
#define DISTANCE_TO_USE_BRANCH_DIRECTION_FOR_CONNECTION 3 //mm. If distance to main branch tree exceeds value, use direction to connect to correct branch.
#define MAX_DIRECTION_DEVIATION_FOR_CONNECTION_EXISTING_BRANCH 60 //deg
#define MAX_DIRECTION_DEVIATION_FOR_CONNECTION_NEW_BRANCH 40 //deg
#define MAX_ORIENTATION_VARIANCE_IN_NEW_BRANCH 0.6 // To exclude "noisy" branches which are difficult to connect correctly


class org_custusx_registration_method_bronchoscopy_EXPORT BranchList
{
	std::vector<BranchPtr> mBranches;
public:
	BranchList();
	virtual ~BranchList();
	void addBranch(BranchPtr b);
	void deleteBranch(BranchPtr b);
	void deleteAllBranches();
	std::vector<BranchPtr> getBranches();
	void findBranchesInCenterline(Eigen::MatrixXd positions_r, bool sortByZindex = true, bool connectSeparateSegments = true);
	void selectGenerations(int maxGeneration);
	void findBronchoscopeRotation();
	void calculateBronchoscopeRotation(BranchPtr branch);
	void smoothOrientations();
	void smoothRadius();
	BranchPtr findBranchWithLargestRadius();
	void interpolateBranchPositions(double resolution = 0.1);
	void smoothBranchPositions(int controlPointDistance = 40);
	void excludeClosePositionsInCTCenterline(double minPointDistance);
	void markLungLap(QString name, Vector3D position);
	void setLapName(BranchPtr branch, QString name);
	QString findClosestLungLap(Vector3D position);
	double findDistance(Vector3D p1, Vector3D p2);
	BranchPtr findClosestBranch(Vector3D targetCoordinate_r);
	std::vector<BranchPtr> findClosesBranches(Vector3D position, double maxDistance);
	BranchListPtr removePositionsForLocalRegistration(Eigen::MatrixXd trackingPositions, double maxDistance);
	vtkPolyDataPtr createVtkPolyDataFromBranches(bool fullyConnected = false, bool straightBranches = false) const;
	Eigen::MatrixXd findMainConnectedAirwayTree(Eigen::MatrixXd positions_r);
};

std::pair<Eigen::MatrixXd,Eigen::MatrixXd > org_custusx_registration_method_bronchoscopy_EXPORT findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed);
bool checkIfTwoPointCloudsAreClose(Eigen::MatrixXd C1, Eigen::MatrixXd C2, double maxDistance/*mm*/);
Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix);
Eigen::MatrixXd org_custusx_registration_method_bronchoscopy_EXPORT eraseCol(int removeIndex, Eigen::MatrixXd positions);
std::pair<Eigen::MatrixXd::Index, double> org_custusx_registration_method_bronchoscopy_EXPORT dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions);
std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > org_custusx_registration_method_bronchoscopy_EXPORT dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
std::vector<Eigen::Vector3d> org_custusx_registration_method_bronchoscopy_EXPORT smoothBranch(BranchPtr branchPtr, int startIndex, Eigen::MatrixXd startPosition);
double bendingDirectionToBronchoscopeRotation(Vector3D bendingDirection, Vector3D parentBranchOrientation, double parentRotation);
double calculateAngleBetweenTwo3DVectors(Vector3D A, Vector3D B);
double calculate3DVaiance(Eigen::MatrixXd A);

org_custusx_registration_method_bronchoscopy_EXPORT Vector3D calculateBronchoscopeBendingDirection(Vector3D A, Vector3D B);

}//namespace cx

#endif /* BRANCHLIST_H_ */
