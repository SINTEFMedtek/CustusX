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

#ifndef CXOPTIONALVALUE_H
#define CXOPTIONALVALUE_H

namespace cx
{

/** Template that holds a value and validity of that value.
 *
 * \ingroup cx_resource_core_utilities
 * \date 2014-03-27
 * \author christiana
 */
template<class T> class OptionalValue
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
