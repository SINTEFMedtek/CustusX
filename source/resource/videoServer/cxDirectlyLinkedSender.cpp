/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDirectlyLinkedSender.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"

namespace cx
{

bool DirectlyLinkedSender::isReady() const
{
	return true;
}

void DirectlyLinkedSender::send(ImagePtr msg)
{
	if (!this->isReady())
		return;

	mImage = msg;

	emit newImage();
}

void DirectlyLinkedSender::send(ProbeDefinitionPtr msg)
{
	if (!this->isReady())
		return;
	mUSStatus = msg;
	emit newUSStatus();
}

ImagePtr DirectlyLinkedSender::popImage()
{
	return mImage;
}
ProbeDefinitionPtr DirectlyLinkedSender::popUSStatus()
{
	return mUSStatus;
}

}
