/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNDITRANSFORMUTILS_H_
#define CXNDITRANSFORMUTILS_H_

#include "cxTransform3D.h"

namespace cx
{

/** Convert an NDI transform (unit quaternion q0,qx,qy,qz and translation in mm, as in
 *  the BX reply) to a Transform3D. The quaternion is normalized first.
 */
inline Transform3D ndiQuaternionToTransform3D(double q0, double qx, double qy, double qz,
											   double tx, double ty, double tz)
{
	Eigen::Quaterniond q(q0, qx, qy, qz);
	q.normalize();
	Transform3D retval = Transform3D::Identity();
	retval.matrix().block<3, 3>(0, 0) = q.toRotationMatrix();
	retval.matrix()(0, 3) = tx;
	retval.matrix()(1, 3) = ty;
	retval.matrix()(2, 3) = tz;
	return retval;
}

} // namespace cx

#endif /* CXNDITRANSFORMUTILS_H_ */
