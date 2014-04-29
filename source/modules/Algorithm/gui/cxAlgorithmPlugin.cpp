#include "cxAlgorithmPlugin.h"

#include <vector>
#include "cxFilterWidget.h"
#include "cxPipeline.h"
#include "cxPipelineWidget.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxAllFiltersWidget.h"
#include "cxDataLocations.h"

namespace cx
{

AlgorithmPlugin::AlgorithmPlugin()
{}

AlgorithmPlugin::~AlgorithmPlugin()
{}

std::vector<PluginBase::PluginWidget> AlgorithmPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
	                     new AllFiltersWidget(NULL),
	                     "Algorithms"));

	return retval;
}

}//namespace cx
