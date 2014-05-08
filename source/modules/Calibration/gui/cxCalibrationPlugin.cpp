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

std::vector<GUIExtenderService::CategorizedWidget> CalibrationPlugin::createWidgets() const
{
	std::vector<CategorizedWidget> retval;

	retval.push_back(GUIExtenderService::CategorizedWidget(
			new CalibrationMethodsWidget(mAcquisitionData, NULL, "CalibrationMethodsWidget", "Calibration Methods"),
			"Algorithms"));

	return retval;
}

}
