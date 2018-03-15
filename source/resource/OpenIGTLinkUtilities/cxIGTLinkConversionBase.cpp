/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxIGTLinkConversionBase.h"

namespace cx
{

QDateTime IGTLinkConversionBase::decode_timestamp(igtl::MessageBase* msg)
{
	// get timestamp from igtl second-format:
	igtl::TimeStamp::Pointer timestamp = igtl::TimeStamp::New();
	msg->GetTimeStamp(timestamp);
	double timestampMS = timestamp->GetTimeStamp() * 1000;
	return QDateTime::fromMSecsSinceEpoch(timestampMS);
}

void IGTLinkConversionBase::encode_timestamp(QDateTime ts, igtl::MessageBase* msg)
{
	igtl::TimeStamp::Pointer timestamp;
	timestamp = igtl::TimeStamp::New();
	double grabTime = 1.0 / 1000 * (double) ts.toMSecsSinceEpoch();
	timestamp->SetTime(grabTime);
	msg->SetTimeStamp(timestamp);
}


} // namespace cx
