/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEMANAGERIMPLSERVICE_H
#define CXFILEMANAGERIMPLSERVICE_H

#include "cxFileManagerServiceBase.h"
#include "cxFileReaderWriterService.h"
#include "org_custusx_core_filemanager_Export.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 * Implementation of the FileManagerService
 *
 * \ingroup org_custusx_core_filemanager
 */
class org_custusx_core_filemanager_EXPORT FileManagerImpService : public FileManagerServiceBase
{
public:
	Q_INTERFACES(cx::FileManagerService)

	FileManagerImpService(ctkPluginContext *context);
	virtual ~FileManagerImpService();

private:
	void initServiceListener(ctkPluginContext *context);
	void onServiceAdded(FileReaderWriterService *service);
	void onServiceRemoved(FileReaderWriterService *service);

	boost::shared_ptr<ServiceTrackerListener<FileReaderWriterService> > mServiceListener;

};

typedef boost::shared_ptr<FileManagerImpService> FileManagerImplServicePtr;

} //cx

#endif // CXFILEMANAGERIMPLSERVICE_H
