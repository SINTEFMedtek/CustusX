/*
 * cxCalibrationPlugin.cpp
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#include "cxCalibrationPlugin.h"

#include "cxCalibrationMethodsWidget.h"
#include "cxAcquisitionData.h"

namespace cx
{

CalibrationPlugin::CalibrationPlugin(AcquisitionDataPtr acquisitionData) :
		mAcquisitionData(acquisitionData)
{
}

CalibrationPlugin::~CalibrationPlugin()
{

}

std::vector<PluginBase::PluginWidget> CalibrationPlugin::createWidgets() const
{
	std::vector<PluginWidget> retval;

	retval.push_back(PluginBase::PluginWidget(
			new CalibrationMethodsWidget(mAcquisitionData, NULL, "CalibrationMethodsWidget", "Calibration Methods"),
			"Algorithms"));

	return retval;
}

}
