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

vtkImageDataPtr generateVtkImageData(Vector3D dim, Vector3D spacing, const unsigned char initValue);

}


#endif /* SSCVOLUMEHELPERS_H_ */
