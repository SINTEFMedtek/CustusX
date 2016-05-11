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

/**
 * sscMathBase.h
 *
 *  Created on: May 8, 2011
 *      Author: christiana
 *
 * Base file for all math that is derived from the library Eigen.
 *
 * http://eigen.tuxfamily.org/index.php?title=Main_Page#Documentation
 *
 * This file must be included before any other inclusion of the Eigen headers,
 * because of the class extensions defined here.
 *
 */


#ifndef CXMATHBASE_H_
#define CXMATHBASE_H_

#include "cxPrecompiledHeader.h"


// necessary includes for the addons
#include <boost/array.hpp>
class QString;
class QStringList;
#include <QStringList> // can be removed by moving impl of Vector3D::fromString to cpp file, as done with Transform3D.
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;

/**
 * \file
 *
 * Include this file before using any Eigen classes. It contains defines that
 * redefine Eigen behaviour.
 *
 * \addtogroup cx_resource_core_math
 * @{
 */

// Extensions of the Eigen classes
// http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html

// important for ssc::Vector3D, our most used class. This line moves all onto one line.
// for default operation, use std::cout << M.format(Eigen::IOFormat()) << std::endl;
#define EIGEN_DEFAULT_IO_FORMAT Eigen::IOFormat(6, 0, " ", " ", "","","","")

#define EIGEN_PLAINOBJECTBASE_PLUGIN "cxPlainObjectEigenAddons.h"
#define EIGEN_MATRIXBASE_PLUGIN "cxMatrixBaseEigenAddons.h"
#define EIGEN_TRANSFORM_PLUGIN "cxTransformEigenAddons.h"

// Eigen library
#include <Eigen/Core>
#include <Eigen/Geometry>

/**
 * @}
 */

// --------------------------------------------------------

#endif /* CXMATHBASE_H_ */
