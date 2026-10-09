/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMATHUTILS_H
#define CXMATHUTILS_H

#include "cxResourceExport.h"
#include "cxTransform3D.h"


namespace cx
{

cxResource_EXPORT double roundAwayFromZero(double val);//Deprecated function. Not to be used. Just here to make sure the MultiGuide application compiles.
/** Pose as 7 elements: the rotation quaternion in Eigen's coefficient order (x, y, z, w), then the translation. */
cxResource_EXPORT Eigen::ArrayXd matrixToQuaternion(Transform3D Tx);
/** Inverse of matrixToQuaternion. The quaternion is normalized first, e.g. after averaging. */
cxResource_EXPORT Transform3D quaternionToMatrix(Eigen::ArrayXd qArray);
/** qArray with its quaternion sign flipped if needed to be closest to reference: q and -q are the same rotation. */
cxResource_EXPORT Eigen::ArrayXd alignQuaternionSign(Eigen::ArrayXd qArray, const Eigen::ArrayXd& reference);

}

#endif // CXMATHUTILS_H

