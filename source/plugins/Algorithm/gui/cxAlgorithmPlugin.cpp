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
#include "cxPipeline.h"
#include "cxPipelineWidget.h"
#include "cxBinaryThresholdImageFilter.h"
#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxDummyFilter.h"
#include "cxDataLocations.h"

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
	                     new AllFiltersWidget(NULL),
	                     "Algorithms"));

// test code for pipeline:

//	PipelinePtr pipeline(new Pipeline());

//	ssc::XmlOptionFile options = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("dummypipeline");
//	FilterGroupPtr filters(new FilterGroup(options));
//	filters->append(FilterPtr(new DummyFilter()));
//	filters->append(FilterPtr(new BinaryThresholdImageFilter()));
//	filters->append(FilterPtr(new BinaryThinningImageFilter3DFilter()));

//	pipeline->initialize(filters);

//	retval.push_back(PluginBase::PluginWidget(
//	                     new PipelineWidget(NULL, pipeline),
//	                     "Algorithms"));

	return retval;

}



}
