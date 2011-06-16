/*
 * cxRegistrationPlugin.h
 *
 *  Created on: Jun 16, 2011
 *      Author: christiana
 */

#ifndef CXREGISTRATIONPLUGIN_H_
#define CXREGISTRATIONPLUGIN_H_

#include "cxPluginBase.h"

namespace cx
{
typedef boost::shared_ptr<class RegistrationPlugin> RegistrationPluginPtr;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

/**Rewrite to be a descendant of PluginBase, with generic API for creating plugins...
 *
 */
class RegistrationPlugin : public PluginBase
{
	Q_OBJECT
public:
	RegistrationPlugin(AcquisitionDataPtr acquisitionData);
	virtual ~RegistrationPlugin() {}

//  AcquisitionDataPtr getAcquisitionData() { return mAcquisitionData; }
	virtual std::vector<PluginWidget> createWidgets() const;

signals:

private slots:

private:
	RegistrationManagerPtr mRegistrationManager;
};

}

#endif /* CXREGISTRATIONPLUGIN_H_ */
