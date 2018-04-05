/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef MATRIXINTERPOLATION_H_
#define MATRIXINTERPOLATION_H_

#include "org_custusx_usreconstruction_Export.h"

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
cxPluginUSReconstruction_EXPORT  std::vector<vnl_matrix_double> matrixInterpolation(
                       vnl_vector<double> DataPoints,
                       std::vector<vnl_matrix_double> DataValues,
                       vnl_vector<double> InterpolationPoints,
                       std::string  InterpolationMethod );


#endif /* MATRIXINTERPOLATION_H_ */
