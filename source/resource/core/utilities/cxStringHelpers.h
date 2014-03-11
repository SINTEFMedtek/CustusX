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

#ifndef CXSTRINGHELPERS_H
#define CXSTRINGHELPERS_H

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
QStringList splitStringContaingQuotes(QString line);


} // namespace cx

/**
* @}
*/


#endif // CXSTRINGHELPERS_H
