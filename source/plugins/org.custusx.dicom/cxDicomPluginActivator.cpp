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

#include "cxDicomPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxDicomGUIExtenderService.h"
#include "cxReporter.h"

namespace cx
{

DicomPluginActivator::DicomPluginActivator()
	: mContext(0)
{
//	std::cout << "Created DicomPluginActivator" << std::endl;
}

DicomPluginActivator::~DicomPluginActivator()
{

}

void DicomPluginActivator::start(ctkPluginContext* context)
{
	//	std::cout << "Started DicomPluginActivator" << std::endl;
	this->mContext = context;

	mDicomPlugin.reset(new DicomGUIExtenderService);
	//  std::cout << "created dicomplugin service" << std::endl;
	try
	{
		context->registerService(QStringList(GUIExtenderService_iid), mDicomPlugin.get());
	}
	catch(ctkRuntimeException& e)
	{
		reportError(QString(e.what()));
		mDicomPlugin.reset();
	}
	//  std::cout << "registered dicomplugin service" << std::endl;
}

void DicomPluginActivator::stop(ctkPluginContext* context)
{
	mDicomPlugin.reset();
	//	std::cout << "Stopped DicomPluginActivator" << std::endl;
	Q_UNUSED(context)
}

} // namespace cx

Q_EXPORT_PLUGIN2(DicomPluginActivator_irrelevant_string, cx::DicomPluginActivator)


