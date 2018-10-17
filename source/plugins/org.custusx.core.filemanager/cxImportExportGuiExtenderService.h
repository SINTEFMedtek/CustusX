/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMPORTEXPORTGUIEXTENDERSERVICE_H
#define CXIMPORTEXPORTGUIEXTENDERSERVICE_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxGUIExtenderService.h"
#include "cxForwardDeclarations.h"
class ctkPluginContext;

namespace cx
{

class org_custusx_core_filemanager_EXPORT ImportExportGuiExtenderService  : public GUIExtenderService
{
public:
	Q_INTERFACES(cx::GUIExtenderService)

	ImportExportGuiExtenderService(ctkPluginContext *context);

	std::vector<CategorizedWidget> createWidgets() const;

private:
	FileManagerServicePtr mFileManager;
	VisServicesPtr mVisServices;
};

}

#endif // CXIMPORTEXPORTGUIEXTENDERSERVICE_H
