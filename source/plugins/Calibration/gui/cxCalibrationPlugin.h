/*
 * cxCalibrationPlugin.h
 *
 *  Created on: Jun 15, 2011
 *      Author: christiana
 */

#ifndef CXCALIBRATIONPLUGIN_H_
#define CXCALIBRATIONPLUGIN_H_

#include "cxPluginBase.h"

namespace cx
{
typedef boost::shared_ptr<class CalibrationPlugin> CalibrationPluginPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

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

}

#endif /* CXCALIBRATIONPLUGIN_H_ */
