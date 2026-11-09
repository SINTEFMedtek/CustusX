/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXVERSION_H
#define CXVERSION_H

#include "cxResourceExport.h"

namespace cx
{
/** The version of the build, from the generated cxVersion.cpp.
 * \ingroup cx_resource_core_settings
 */
cxResource_EXPORT const char* getVersionString();
} // namespace cx

#define CustusX_VERSION_STRING cx::getVersionString()

#endif // CXVERSION_H
