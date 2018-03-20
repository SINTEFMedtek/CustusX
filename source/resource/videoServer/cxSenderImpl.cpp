/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSenderImpl.h"
#include "cxIGTLinkConversion.h"

namespace cx
{

void SenderImpl::send(PackagePtr package)
{
	if(package->mImage)
		this->send(package->mImage);

	if(package->mProbe)
		this->send(package->mProbe);
}


} /* namespace cx */
