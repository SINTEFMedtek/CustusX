/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStreamerServiceNull.h"

namespace cx
{

QString StreamerServiceNull::getName()
{
    return "StreamerServiceNull";
}

QString StreamerServiceNull::getType() const
{
	return "Streamer_Service_Null";
}

void StreamerServiceNull::stop()
{}

std::vector<PropertyPtr> StreamerServiceNull::getSettings(QDomElement root)
{
	Q_UNUSED(root);
	std::vector<PropertyPtr> retval;
	return retval;
}

StreamerPtr StreamerServiceNull::createStreamer(QDomElement root)
{
	Q_UNUSED(root);
	StreamerPtr retval;
	return retval;
}

} //end namespace cx
