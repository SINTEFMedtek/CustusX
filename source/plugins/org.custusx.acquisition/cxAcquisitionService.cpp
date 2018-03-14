/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAcquisitionService.h"
#include "cxAcquisitionServiceNull.h"
#include "cxNullDeleter.h"
#include "cxRecordSession.h"
#include "cxReporter.h"

namespace cx
{

AcquisitionServicePtr AcquisitionService::getNullObject()
{
	static AcquisitionServicePtr mNull;
	if (!mNull)
		mNull.reset(new AcquisitionServiceNull, null_deleter());
	return mNull;
}

RecordSessionPtr AcquisitionService::getSession(QString uid)
{
	RecordSessionPtr retval;
    std::vector<RecordSessionPtr> sessions = this->getSessions();
    for(unsigned i = 0; i < sessions.size(); ++i)
    {
        if(sessions[i] && (sessions[i]->getUid() == uid))
            retval = sessions[i];
    }

//    if(!retval)
//		reporter()->sendError("Did not find record session " + uid);
	return retval;
}

//void AcquisitionService::toggleRecord(AcquisitionService::TYPES context, QString category)
//{
//	if (this->getState()==AcquisitionService::sRUNNING)
//		this->stopRecord();
//	else
//		this->startRecord(context, category);
//}


} //cx
