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
#ifndef MATRIXINTERPOLATION_H_
#define MATRIXINTERPOLATION_H_

#include <vector>
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
typedef vnl_matrix<double> vnl_matrix_double;

//! Operation: Interpolate transformation matrices.
// Suppose that T(t) is a time-variant 4-by-4 transformation matrix
// whos values at certain points, given in the vector DataPoints, are
// given in the vector DataValues. This operation, then, interpolates
// to find the values of T at the intermediate points given in the
// vector InterpolationPoints.
//
// The interpolated values are found in two different ways depending
// on the value of the InterpolationMethod string:
//   1. InterpolationMethod == "closest point": In this case, the
//      value at a given point is set to the value at the nearest
//      neighboring point with a known value.
//   2. InterpolationMethod == "linear": In this case, the rotation
//      part of the transformation matrix is found using the slerp
//      (spherical linear interpolation), while the translation part
//      is found using ordinary linear interpolation.
//
// Trondheim, 12.01.09.
// Lars Eirik B�  <larseirik.bo@sintef.no>, SINTEF Technology and Society.
std::vector<vnl_matrix_double> matrixInterpolation(
                       vnl_vector<double> DataPoints,
                       std::vector<vnl_matrix_double> DataValues,
                       vnl_vector<double> InterpolationPoints,
                       std::string  InterpolationMethod );


#endif /* MATRIXINTERPOLATION_H_ */
