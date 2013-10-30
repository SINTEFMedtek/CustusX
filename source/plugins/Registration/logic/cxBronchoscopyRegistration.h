/*
 * BronchoscopyRegistration.h
 *
 *  Created on: Jun 20, 2013
 *      Author: ehofstad
 */

#ifndef BRONCHOSCOPYREGISTRATION_H_
#define BRONCHOSCOPYREGISTRATION_H_

//#include "PositionData.h"
#include "cxBranchList.h"
#include <vector>
#include "vtkForwardDeclarations.h"


typedef std::vector< Eigen::Matrix4d > M4Vector;


namespace cx
{

typedef std::map<double, Transform3D> TimedTransformMap;

class BronchoscopyRegistration
{
public:
	BronchoscopyRegistration();
    Eigen::Matrix4d runBronchoscopyRegistration(vtkPolyDataPtr centerline, TimedTransformMap trackingData_prMt, Transform3D old_rMpr);
	virtual ~BronchoscopyRegistration();
};

M4Vector excludeClosePositions();
Eigen::Matrix4d registrationAlgorithm(BranchList* branches, M4Vector Tnavigation);
std::vector<Eigen::MatrixXd::Index> dsearch2n(Eigen::MatrixXd pos1, Eigen::MatrixXd pos2, Eigen::MatrixXd ori1, Eigen::MatrixXd ori2);
vtkPointsPtr convertTovtkPoints(Eigen::MatrixXd positions);
Eigen::Matrix4d performLandmarkRegistration(vtkPointsPtr source, vtkPointsPtr target, bool* ok);
}//namespace cx

#endif /* BRONCHOSCOPYREGISTRATION_H_ */
