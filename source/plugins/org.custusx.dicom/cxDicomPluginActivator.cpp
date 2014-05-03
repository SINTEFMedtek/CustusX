/*
 * DicomPluginActivator.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: christiana
 */

#include "cxDicomPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxDicomPlugin.h"

//DicomPluginActivator* DicomPluginActivator::instance = 0;

namespace cx
{

DicomPluginActivator::DicomPluginActivator()
  : context(0)
{
//	int* val = 0;
//	*val = 0;
	std::cout << "Created DicomPluginActivator" << std::endl;
}

DicomPluginActivator::~DicomPluginActivator()
{

}

void DicomPluginActivator::start(ctkPluginContext* context)
{
	std::cout << "Started DicomPluginActivator" << std::endl;
//  instance = this;
  this->context = context;

  mDicomPlugin.reset(new DicomPlugin);
  std::cout << "created dicomplugin service" << std::endl;
  try
  {
	  context->registerService(QStringList(PluginBase_iid), mDicomPlugin.get());
  }
  catch(ctkRuntimeException& e)
  {
	  std::cout << e.what() << std::endl;;
  }
  std::cout << "registered dicomplugin service" << std::endl;
//  mBackendInterface = new BackendInterfaceImpl2();
//  std::cout << "create BackendInterfaceImpl2 object " << mBackendInterface << std::endl;
//  context->registerService(QStringList("BackendInterface"), mBackendInterface);

}

void DicomPluginActivator::stop(ctkPluginContext* context)
{
	mDicomPlugin.reset();
//	delete mTestService;
	std::cout << "Stopped DicomPluginActivator" << std::endl;
  Q_UNUSED(context)
}

//DicomPluginActivator* DicomPluginActivator::getInstance()
//{
//	return instance;
//}
//
//ctkPluginContext* DicomPluginActivator::getPluginContext() const
//{
//	return context;
//}

} // namespace cx

//Q_EXPORT_PLUGIN2(org_mydomain_testplugin, DicomPluginActivator)
Q_EXPORT_PLUGIN2(DicomPluginActivator_irrelevant_string, cx::DicomPluginActivator)


