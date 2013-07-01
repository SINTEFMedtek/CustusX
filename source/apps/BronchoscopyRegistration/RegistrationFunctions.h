/*
 * RegistrationFunctions.h
 *
 *  Created on: Jun 24, 2013
 *      Author: ehofstad
 */

#ifndef REGISTRATIONFUNCTIONS_H_
#define REGISTRATIONFUNCTIONS_H_

#include <vector>
#include "sscDataManager.h"
#include "sscMesh.h"
#include "sscVector3D.h"

namespace cx
{

Eigen::MatrixXd sortMatrix(int rowNumber, Eigen::MatrixXd matrix);
Eigen::MatrixXd eraseColumn(int removeIndex, Eigen::MatrixXd positions);
std::pair<Eigen::MatrixXd::Index, double> dsearch(Eigen::Vector3d p, Eigen::MatrixXd positions);
std::pair<std::vector<Eigen::MatrixXd::Index>, Eigen::VectorXd > dsearchn(Eigen::MatrixXd p1, Eigen::MatrixXd p2);
std::pair<Eigen::MatrixXd,Eigen::MatrixXd > findConnectedPointsInCT(int startIndex , Eigen::MatrixXd positionsNotUsed);

} /* namespace cx */
#endif /* REGISTRATIONFUNCTIONS_H_ */
