/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
