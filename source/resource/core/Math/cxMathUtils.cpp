/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMathUtils.h"

#include "cxVector3D.h"

// For debugging:
#include "cxLogger.h"


namespace cx {

//Deprecated. Don't use
double roundAwayFromZero(double val)
{
	CX_LOG_WARNING() << "cx::roundAwayFromZero() is deprecated, and will be removed. Use std::round() or std::lround() instead.";
	if(val >= 0)
		return int(val+0.5);
	else
		return int(val-0.5);
}

Eigen::ArrayXd matrixToQuaternion(Transform3D Tx)
// Converts a 4x4 transformation matrix to quaternion (7 elements)
{
    Eigen::ArrayXd qArray = Eigen::ArrayXd::Zero(7);
    Eigen::Quaterniond qA;

    qArray.segment<3>(4) = Tx.matrix().block<3, 1>(0,3); // Translation part
    qA = Eigen::Quaterniond(Tx.matrix().block<3, 3>(0,0)); //Convert rot to quaternions
    qArray.segment<4>(0) = qA.coeffs(); //Rotation parameters

    return qArray;
}

Transform3D quaternionToMatrix(Eigen::ArrayXd qArray)
// Converts a quaternion (7 elements) to a 4x4 transformation matrix
{
    Transform3D Tx;
    Eigen::Quaterniond qA;

    qA.coeffs() = qArray.segment<4>(0);
    Tx.matrix().block<3, 3>(0,0) = qA.toRotationMatrix();
    Tx.matrix().block<3, 1>(0,3) = qArray.segment<3>(4);
    Tx.matrix()(3,3) = 1;

    return Tx;
}

}
