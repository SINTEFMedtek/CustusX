/*
 * cxCalibrationPlugin.h
 *
 *  \date Jun 15, 2011
 *      \author christiana
 */

#ifndef CXCALIBRATIONPLUGIN_H_
#define CXCALIBRATIONPLUGIN_H_

#include "cxPluginBase.h"

/**
 * \defgroup cxPluginCalibration Calibration Plugin
 * \ingroup cxPlugins
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
 * \addtogroup cxPluginCalibration
 * @{
 */

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 */
class CalibrationPlugin : public PluginBase
{
	Q_OBJECT
public:
	CalibrationPlugin(AcquisitionDataPtr mAcquisitionData);
	virtual ~CalibrationPlugin();

//  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
	virtual std::vector<PluginWidget> createWidgets() const;

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
