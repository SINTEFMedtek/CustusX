/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXVECTOR3D_H_
#define CXVECTOR3D_H_


#include "cxMathBase.h"
class QString;

namespace cx
{

/**
 * \addtogroup cx_resource_core_math
 * @{
 */


bool similar(double a, double b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol

/**\brief Vector3D is a representation of a point or vector in 3D.
 *
 * Vector3D is implemented using the Eigen library type Eigen::Vector3d,
 * and extended with convenience methods using the Eigen plugin system.
 *
 * Some of the extension exist for backward compatibility with
 * the old inhouse Vector3D.
 *
 */
typedef Eigen::Vector3d Vector3D;
typedef boost::shared_ptr<Vector3D> Vector3DPtr;

Vector3D multiply_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise multiplication of a and b.
Vector3D divide_elems(const Vector3D& a, const Vector3D& b); ///< perform element-wise division of a and b.
Vector3D cross(const Vector3D& a, const Vector3D& b); ///< compute cross product of a and b.
double dot(const Vector3D& a, const Vector3D& b); ///< compute inner product (or dot product) of a and b.
bool similar(const Vector3D& a, const Vector3D& b, double tol = 1.0E-6); ///< check for equality with a tolerance: |b-a|<tol
Vector3D unitVector(double thetaXY, double thetaZ = 0.0); ///< compute a unit vector given angles xy in the xy plane and z meaning the elevation from the xy plane.
double getThetaXY(Vector3D v); ///< get thetaXY, meaning the angle of v projected onto the xy plane
double getThetaZ(Vector3D v); ///< get thetaZ, z meaning the elevation from the xy plane

Vector3D round(const Vector3D& a);
Vector3D ceil(const Vector3D& a);

bool similar(const Eigen::Array3i& a, const Eigen::Array3i& b); ///< identical to the equality operator, exists for symmetry with Vector3D.

QString prettyFormat(Vector3D val, int decimals=1, int fieldWidth=3);

Eigen::Vector2d fromString(const QString& text);

/**
 * @}
 */

} // namespace cx
// --------------------------------------------------------

#endif /*CXVECTOR3D_H_*/
