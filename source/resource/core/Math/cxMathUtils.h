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

namespace cx
{

cxResource_EXPORT double roundAwayFromZero(double val);
cxResource_EXPORT unsigned int roundUnsigned(double val);

}

#endif // CXMATHUTILS_H

