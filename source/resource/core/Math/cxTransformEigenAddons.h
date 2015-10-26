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

//! @cond Doxygen_Suppress
/*
 * sscTransformEigenAddons.h
 *
 * Plugin extension for the eigen::Transform type
 *
 *
 *  Created on: Apr 24, 2011
 *      Author: christiana
 */

Vector3d vector(const Vector3d& v) const;///< transform a free vector [x,y,z,0]
Vector3d unitVector(const Vector3d& v) const;///< transform a unit vector [x,y,z,0], force |v|=1 after transform.
Vector3d coord(const Vector3d& v) const;///< transform a coordinate [x,y,z,1].
Transform inv()
const;                       ///< return an inverted transform M^-1
boost::array<double, 16> flatten() const;///< return matrix as a flat array, vtk ordering
explicit Transform(vtkMatrix4x4* m);
explicit Transform(double* m);
vtkMatrix4x4Ptr getVtkMatrix()
const;
vtkTransformPtr getVtkTransform()
const;
std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
static Transform fromString(const QString& text, bool* ok=0); ///< construct a transform matrix from a string containing 16 whitespace-separated numbers, vtk ordering
static Transform fromVtkMatrix(vtkMatrix4x4Ptr m);
static Transform fromFloatArray(float m[4][4]);

//! @endcond
