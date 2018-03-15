/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSyncedValue.h"

namespace cx
{

SyncedValue::SyncedValue(QVariant val) :
				mValue(val)
{
}
SyncedValuePtr SyncedValue::create(QVariant val)
{
	return SyncedValuePtr(new SyncedValue(val));
}
void SyncedValue::set(QVariant val)
{
	if (mValue == val)
		return;
	mValue = val;
	emit changed();
}
QVariant SyncedValue::get() const
{
	return mValue;
}


} //namespace cx
