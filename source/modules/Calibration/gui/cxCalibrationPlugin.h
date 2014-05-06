/*
 * cxCalibrationPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#ifndef CXCALIBRATIONPLUGIN_H_
#define CXCALIBRATIONPLUGIN_H_

#include "cxGUIExtenderService.h"

/**
 * \defgroup cx_module_calibration Calibration Plugin
 * \ingroup cx_modules
 * \brief Calibration collection with widgets.
 *
 * See \ref cx::CalibrationPlugin.
 *
 */


namespace cx
{
typedef boost::shared_ptr<class CalibrationPlugin> CalibrationPluginPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

/**
 * \file
 * \addtogroup cx_module_calibration
 * @{
 */

/**
 *
 */
class CalibrationPlugin : public GUIExtenderService
{
	Q_OBJECT
public:
	CalibrationPlugin(AcquisitionDataPtr mAcquisitionData);
	virtual ~CalibrationPlugin();

//  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
	virtual std::vector<CategorizedWidget> createWidgets() const;

signals:

private slots:

private:
	AcquisitionDataPtr mAcquisitionData;
};

/**
 * @}
 */
}

#endif /* CXCALIBRATIONPLUGIN_H_ */
