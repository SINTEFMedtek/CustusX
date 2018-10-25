/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFileManagerImplService.h"
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxUtilHelpers.h"
#include "cxNullDeleter.h"
#include "cxLogger.h"
#include "boost/bind.hpp"
#include "cxCoreServices.h"

namespace cx
{
FileManagerImpService::FileManagerImpService(ctkPluginContext *context)
{
	this->setObjectName("FileManagerImpService");
	this->initServiceListener(context);

}

FileManagerImpService::~FileManagerImpService()
{
}

void FileManagerImpService::initServiceListener(ctkPluginContext *context)
{
	mServiceListener.reset(new ServiceTrackerListener<FileReaderWriterService>(
								 context,
								 boost::bind(&FileManagerImpService::onServiceAdded, this, _1),
								 boost::function<void (FileReaderWriterService*)>(),
								 boost::bind(&FileManagerImpService::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}

void FileManagerImpService::onServiceAdded(FileReaderWriterService *service)
{
	this->addFileReaderWriter(service);
}

void FileManagerImpService::onServiceRemoved(FileReaderWriterService *service)
{
	this->removeFileReaderWriter(service);
}

} // cx
