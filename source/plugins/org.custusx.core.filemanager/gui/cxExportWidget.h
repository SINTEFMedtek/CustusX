/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXEXPORTWIDGET_H
#define CXEXPORTWIDGET_H

#include "cxBaseWidget.h"
#include "cxFileManagerService.h"
#include "org_custusx_core_filemanager_Export.h"

namespace cx
{

class org_custusx_core_filemanager_EXPORT ExportWidget  : public BaseWidget
{
public:
	ExportWidget(FileManagerServicePtr filemanager, VisServicesPtr services);

private:
	FileManagerServicePtr mFileManager;
	VisServicesPtr mVisServices;

};

}

#endif // CXEXPORTWIDGET_H
