/*
 * sscVolumeHelpers.h
 *
 *  Created on: Dec 13, 2010
 *      Author: dev
 */

#ifndef SSCVOLUMEHELPERS_H_
#define SSCVOLUMEHELPERS_H_

#include "sscVector3D.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

/**
 * \addtogroup sscUtility
 * \{
 */

vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim, Vector3D spacing, const unsigned char initValue);
vtkImageDataPtr generateVtkImageDataDouble(Eigen::Array3i dim, Vector3D spacing, double initValue);

/**
 * \}
 */

}


#endif /* SSCVOLUMEHELPERS_H_ */
