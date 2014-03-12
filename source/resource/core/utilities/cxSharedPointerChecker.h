// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSHAREDPOINTERCHECKER_H
#define CXSHAREDPOINTERCHECKER_H

#include "boost/shared_ptr.hpp"
#include <QString>

namespace cx
{

/**
* \file
* \addtogroup cx_resource_core_utilities
* @{
*/

void requireUnique(int use_count, QString objectName);

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
