/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomGUIExtenderService.h"
#include <QLabel>
#include "cxDicomWidget.h"
#include <QResource>

namespace cx
{


DicomGUIExtenderService::DicomGUIExtenderService(ctkPluginContext *context) : mContext(context)
{
	QResource::registerResource("./Resources/ctkDICOM.qrc");
}

DicomGUIExtenderService::~DicomGUIExtenderService()
{
}

std::vector<GUIExtenderService::CategorizedWidget> DicomGUIExtenderService::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new DicomWidget(mContext),
			"Utility"));

	return retval;
}


} /* namespace cx */
