/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFILEMANAGERSERVICEPLUGINACTIVATOR_H
#define CXFILEMANAGERSERVICEPLUGINACTIVATOR_H
#include <ctkPluginActivator.h>
#include "boost/shared_ptr.hpp"
#include "cxRegisteredService.h"

namespace cx
{

/**
 * Activator for the port plugin
 *
 * \ingroup org_custusx_core_filemanager
 */
class FileManagerServicePluginActivator :  public QObject, public ctkPluginActivator
{
	Q_OBJECT
	Q_INTERFACES(ctkPluginActivator)
	Q_PLUGIN_METADATA(IID "org_custusx_core_filemanager")
public:
	FileManagerServicePluginActivator();
	~FileManagerServicePluginActivator();

	void start(ctkPluginContext* context);
	void stop(ctkPluginContext* context);

private:
	RegisteredServicePtr mRegisteredFileManagerService;
	std::vector<RegisteredServicePtr> mRegisteredFileReaderWriterServices;
	RegisteredServicePtr mRegisteredGuiExtenderService;
};

} // cx
#endif // CXFILEMANAGERSERVICEPLUGINACTIVATOR_H
