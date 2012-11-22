/*
 * cxAlgorithmPlugin.cpp
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#include "cxAlgorithmPlugin.h"

#include <vector>
#include "cxVisualizationMethodsWidget.h"
#include "cxSegmentationMethodsWidget.h"
#include "cxFilterWidget.h"

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
    retval.push_back(PluginBase::PluginWidget(
            new FilterWidget(NULL),
            "Algorithms"));



	return retval;

}



}
