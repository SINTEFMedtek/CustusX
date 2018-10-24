/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImportExportGuiExtenderService.h"

#include "cxExportWidget.h"
#include "cxImportWidget.h"
#include "cxFileManagerServiceProxy.h"
#include "cxVisServices.h"

namespace cx
{

ImportExportGuiExtenderService::ImportExportGuiExtenderService(ctkPluginContext *context)
{
	mFileManager = FileManagerServiceProxy::create(context);
	mVisServices = VisServices::create(context);
}

std::vector<GUIExtenderService::CategorizedWidget> ImportExportGuiExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;
	retval.push_back(GUIExtenderService::CategorizedWidget( new ExportWidget(mFileManager, mVisServices), "File"));
	retval.push_back(GUIExtenderService::CategorizedWidget( new ImportWidget(mFileManager, mVisServices), "File"));
	return retval;
}
}
