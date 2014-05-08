// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxDicomGUIExtenderService.h"
#include <QLabel>
#include "cxDicomWidget.h"
#include <QResource>

namespace cx
{


DicomGUIExtenderService::DicomGUIExtenderService()
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
			new DicomWidget(),
			"Plugins"));

	return retval;
}


} /* namespace cx */
