/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLANDMARKTRANSLATIONREGISTRATION_H_
#define CXLANDMARKTRANSLATIONREGISTRATION_H_

#include "org_custusx_registration_Export.h"
#include <vector>
#include "cxTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration
 * @{
 */

/** Landmark registration in 3D with translation only.
 *
 * The translation is the mean of ref - target over the point pairs, which
 * minimizes the sum of squared distances between the pairs.
 */
class org_custusx_registration_EXPORT LandmarkTranslationRegistration
{
public:
  Transform3D registerPoints(std::vector<Vector3D> ref, std::vector<Vector3D> target, bool* ok);
};

/**
 * @}
 */
}

#endif /* CXLANDMARKTRANSLATIONREGISTRATION_H_ */
