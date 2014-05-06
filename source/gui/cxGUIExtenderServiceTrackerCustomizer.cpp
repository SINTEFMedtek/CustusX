/*
 * cxPluginBaseServiceTrackerCustomizer.cpp
 *
 *  Created on: May 2, 2014
 *      Author: christiana
 */

#include "cxGUIExtenderServiceTrackerCustomizer.h"
#include <iostream>

namespace cx
{

GUIExtenderServiceTrackerCustomizer::~GUIExtenderServiceTrackerCustomizer()
{
}

GUIExtenderService* GUIExtenderServiceTrackerCustomizer::addingService(const ctkServiceReference &reference)
{
//	std::cout << "adding service" << std::endl;
	GUIExtenderService* service = reference.getPlugin()->getPluginContext()->getService<GUIExtenderService>(reference);
	emit serviceAdded(service);
	return service;
}

void GUIExtenderServiceTrackerCustomizer::modifiedService(const ctkServiceReference &reference, GUIExtenderService* service)
{

}

void GUIExtenderServiceTrackerCustomizer::removedService(const ctkServiceReference &reference, GUIExtenderService* service)
{
//	std::cout << "removing service" << std::endl;
	emit serviceRemoved(service);
	reference.getPlugin()->getPluginContext()->ungetService(reference);
}


} /* namespace cx */
