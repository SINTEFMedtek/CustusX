/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMATHUTILS_H
#define CXMATHUTILS_H

#include "cxResourceExport.h"
#include "cxTransform3D.h"


namespace cx
{

cxResource_EXPORT double roundAwayFromZero(double val);//Deprecated function. Not to be used. Just here to make sure the MultiGuide application compiles.
cxResource_EXPORT Eigen::ArrayXd matrixToQuaternion(Transform3D Tx);
cxResource_EXPORT Transform3D quaternionToMatrix(Eigen::ArrayXd qArray);

}

#endif // CXMATHUTILS_H

