/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxtestSender.h"

#include "cxTypeConversions.h"

namespace cxtest
{

bool TestSender::isReady() const
{
	return true;
}

void TestSender::send(cx::PackagePtr package)
{
	mPackage = package;
	emit newPackage();
}

cx::PackagePtr TestSender::getSentPackage()
{
	return mPackage;
}

} /* namespace cxtest */
