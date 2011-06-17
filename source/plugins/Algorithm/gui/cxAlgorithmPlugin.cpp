/*
 * cxAlgorithmPlugin.cpp
 *
 *  Created on: Jun 15, 2011
 *      Author: christiana
 */

#include "cxAlgorithmPlugin.h"

#include <vector>
#include "cxVisualizationMethodsWidget.h"
#include "cxSegmentationMethodsWidget.h"

namespace cx
{

AlgorithmPlugin::AlgorithmPlugin()
{
}

AlgorithmPlugin::~AlgorithmPlugin()
{

}

std::vector<PluginBase::PluginWidget> AlgorithmPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
			new SegmentationMethodsWidget(NULL, "SegmentationMethodsWidget", "Segmentation Methods"),
			"Algorithms"));
	retval.push_back(PluginBase::PluginWidget(
			new VisualizationMethodsWidget(NULL, "VisualizationMethodsWidget", "Visualization Methods"),
			"Algorithms"));

	return retval;

}



}
