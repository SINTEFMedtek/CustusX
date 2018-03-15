/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVIEWUTILITIES_H_
#define CXVIEWUTILITIES_H_

#include "cxLayoutData.h"
class QGridLayout;

namespace cx
{
/**
* \defgroup cx_resource_view_internal
* \ingroup cx_resource_view
*/

/**
* \file
* \addtogroup cx_resource_view_internal
* @{
*/

namespace view_utils
{

void setStretchFactors(QGridLayout* layout, LayoutRegion region, int stretchFactor);
}

/**
* @}
*/
} /* namespace cx */
#endif /* CXVIEWUTILITIES_H_ */
