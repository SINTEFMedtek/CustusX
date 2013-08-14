/*
 * sscMathBase.h
 *
 *  Created on: May 8, 2011
 *      Author: christiana
 *
 * Base file for all ssc math that is derived from the library Eigen.
 *
 * http://eigen.tuxfamily.org/index.php?title=Main_Page#Documentation
 *
 * This file must be included before any other inclusion of the Eigen headers,
 * because of the class extensions defined here.
 *
 */

#ifndef SSCMATHBASE_H_
#define SSCMATHBASE_H_

// necessary includes for the addons
#include <boost/array.hpp>
class QString;
class QStringList;
#include <QStringList> // can be removed by moving impl of Vector3D::fromString to cpp file, as done with Transform3D.
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;

// Extensions of the Eigen classes
// http://eigen.tuxfamily.org/dox/TopicCustomizingEigen.html

// important for ssc::Vector3D, our most used class. This line moves all onto one line.
// for default operation, use std::cout << M.format(Eigen::IOFormat()) << std::endl;
#define EIGEN_DEFAULT_IO_FORMAT Eigen::IOFormat(6, 0, " ", " ", "","","","")

#define EIGEN_PLAINOBJECTBASE_PLUGIN "sscPlainObjectEigenAddons.h"
#define EIGEN_MATRIXBASE_PLUGIN "sscMatrixBaseEigenAddons.h"
#define EIGEN_TRANSFORM_PLUGIN "sscTransformEigenAddons.h"
#define EIGEN_DONT_ALIGN

// Eigen library
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

// --------------------------------------------------------

#endif /* SSCMATHBASE_H_ */
