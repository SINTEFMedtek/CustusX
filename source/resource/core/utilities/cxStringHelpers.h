/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTRINGHELPERS_H
#define CXSTRINGHELPERS_H

#include "cxResourceExport.h"

class QString;
class QStringList;


/**
* \addtogroup cx_resource_core_utilities
* @{
*/

namespace cx
{

/** 
 *
 * \date 30 Aug 2013, 2013
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
  * given a text line containing fex [alpha beta "gamm ma" delta]
  * split into a list of ["alpha", "beta" ,"gamm ma", "delta"]
  */
cxResource_EXPORT QStringList splitStringContaingQuotes(QString line);

cxResource_EXPORT int convertStringWithDefault(QString text, int def);

} // namespace cx

/**
* @}
*/


#endif // CXSTRINGHELPERS_H
