/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSHAREDPOINTERCHECKER_H
#define CXSHAREDPOINTERCHECKER_H

#include "cxResourceExport.h"

#include "boost/shared_ptr.hpp"
#include <QString>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

cxResource_EXPORT void requireUnique(int use_count, QString objectName);

/** Utility for checking the usage count of share_ptr's
  * and reporting it.
  *
 * \date 2014-03-09
 * \author christiana
  */
template<class T>
void requireUnique(const boost::shared_ptr<T>& object, QString objectName)
{
	requireUnique(object.use_count(), objectName);
}

/**
* @}
*/

} // namespace cx


#endif // CXSHAREDPOINTERCHECKER_H
