#include "cxStreamerServiceUtilities.h"

#include <QList>
#include <ctkServiceTracker.h>

namespace cx
{
StreamerService* StreamerServiceUtilities::getStreamerService(QString name, ctkPluginContext* context)
{
	ctkServiceTracker<StreamerService*> tracker(context);
	tracker.open();

	QList<StreamerService*> serviceList = tracker.getServices();

	for(int i = 0; i < serviceList.size(); ++i)
	{
		StreamerService* service = serviceList.at(i);
		if (service->getName() == name)
		return service;
	}

	return NULL;
}
} /* namespace cx */
