/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLANDMARKTRANSLATIONREGISTRATION_H_
#define CXLANDMARKTRANSLATIONREGISTRATION_H_

#include <vector>
#include "cxTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration
 * @{
 */

/** Use ITK to perform a landmark registration in 3D with translation only.
 *
 * Code is based on  itk/Examples/Registration/IterativeClosestPoint1.cxx
 *
 */
class LandmarkTranslationRegistration
{
public:
  Transform3D registerPoints(std::vector<Vector3D> ref, std::vector<Vector3D> target, bool* ok);
};

/**
 * @}
 */
}

#endif /* CXLANDMARKTRANSLATIONREGISTRATION_H_ */
