/*
 * cxPluginBaseServiceTrackerCustomizer.h
 *
 *  Created on: May 2, 2014
 *      Author: christiana
 */

#ifndef CXPLUGINBASESERVICETRACKERCUSTOMIZER_H_
#define CXPLUGINBASESERVICETRACKERCUSTOMIZER_H_

#include "ctkServiceTracker.h"
#include "ctkServiceTrackerCustomizer.h"
#include "cxPluginBase.h"

namespace cx
{

typedef ctkServiceTracker<PluginBase*> PluginBaseServiceTracker;
typedef boost::shared_ptr<PluginBaseServiceTracker> PluginBaseServiceTrackerPtr;

/**Notify changes in PluginBase service lifecycle.
 *
 * Used inside a PluginBaseServiceTracker.
 *
 * \ingroup cx_gui
 * \date 2014-05-02
 * \author Christian Askeland
 */
class PluginBaseServiceTrackerCustomizer : public QObject, public ctkServiceTrackerCustomizer<PluginBase*>
{
	Q_OBJECT
public:
	virtual PluginBase* addingService(const ctkServiceReference &reference);
	virtual void modifiedService(const ctkServiceReference &reference, PluginBase* service);
	virtual void removedService(const ctkServiceReference &reference, PluginBase* service);
	virtual ~PluginBaseServiceTrackerCustomizer();

//	PluginBase* getPlugin(QString)
signals:
	void serviceAdded(PluginBase* reference);
	void serviceRemoved(PluginBase* reference);
private:
public:
//	std::map<QString, PluginBase*> mPlugins;

};
typedef boost::shared_ptr<PluginBaseServiceTrackerCustomizer> PluginBaseServiceTrackerCustomizerPtr;

} /* namespace cx */
#endif /* CXPLUGINBASESERVICETRACKERCUSTOMIZER_H_ */

