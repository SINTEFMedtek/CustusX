/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAcquisitionServiceNull.h"
#include "cxLogger.h"

namespace cx
{

AcquisitionServiceNull::AcquisitionServiceNull()
{
}

bool AcquisitionServiceNull::isNull()
{
	return true;
}

RecordSessionPtr AcquisitionServiceNull::getLatestSession()
{
	printWarning();
	return RecordSessionPtr();
}

std::vector<RecordSessionPtr> AcquisitionServiceNull::getSessions()
{
	printWarning();
	return std::vector<RecordSessionPtr>();
}

bool AcquisitionServiceNull::isReady(TYPES context) const
{
	printWarning();
	return false;
}

QString AcquisitionServiceNull::getInfoText(TYPES context) const
{
	printWarning();
	return QString();
}

AcquisitionService::STATE AcquisitionServiceNull::getState() const
{
	printWarning();
	return AcquisitionService::sNOT_RUNNING;
}

void AcquisitionServiceNull::startRecord(TYPES context, QString category, RecordSessionPtr session)
{
	printWarning();
}

void AcquisitionServiceNull::stopRecord()
{
	printWarning();
}

void AcquisitionServiceNull::cancelRecord()
{
	printWarning();
}

void AcquisitionServiceNull::startPostProcessing()
{
	printWarning();
}

void AcquisitionServiceNull::stopPostProcessing()
{
	printWarning();
}

int AcquisitionServiceNull::getNumberOfSavingThreads() const
{
	printWarning();
	return 0;
}

void AcquisitionServiceNull::printWarning() const
{
	reportWarning("Trying to use AcquisitionServiceNull. Is AcquisitionService (org.custusx.acquisition) disabled?");
}

} // cx
