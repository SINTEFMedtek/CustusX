/*
 * cxPluginBaseServiceTrackerCustomizer.cpp
 *
 *  Created on: May 2, 2014
 *      Author: christiana
 */

#include "cxPluginBaseServiceTrackerCustomizer.h"
#include <iostream>

namespace cx
{

PluginBaseServiceTrackerCustomizer::~PluginBaseServiceTrackerCustomizer()
{
}

PluginBase* PluginBaseServiceTrackerCustomizer::addingService(const ctkServiceReference &reference)
{
//	std::cout << "adding service" << std::endl;
	PluginBase* service = reference.getPlugin()->getPluginContext()->getService<PluginBase>(reference);
	emit serviceAdded(service);
	return service;
}

void PluginBaseServiceTrackerCustomizer::modifiedService(const ctkServiceReference &reference, PluginBase* service)
{

}

void PluginBaseServiceTrackerCustomizer::removedService(const ctkServiceReference &reference, PluginBase* service)
{
//	std::cout << "removing service" << std::endl;
	emit serviceRemoved(service);
	reference.getPlugin()->getPluginContext()->ungetService(reference);
}


} /* namespace cx */
