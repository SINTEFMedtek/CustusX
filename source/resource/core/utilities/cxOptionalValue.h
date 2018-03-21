/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXOPTIONALVALUE_H
#define CXOPTIONALVALUE_H

#include "cxResourceExport.h"

namespace cx
{

/** Template that holds a value and validity of that value.
 *
 * \ingroup cx_resource_core_utilities
 * \date 2014-03-27
 * \author christiana
 */
template<class T> class cxResource_EXPORT OptionalValue
{
public:
	OptionalValue() : mValid(false) {}
	explicit OptionalValue(T val) : mValue(val), mValid(val) {}
	const T& get() const { return mValue; }
	void set(const T& val) { mValue=val; mValid=true; }
	bool isValid() const { return mValid; }
	void reset() { mValid=false; }
private:
	T mValue;
	bool mValid;
};


} // namespace cx


#endif // CXOPTIONALVALUE_H
