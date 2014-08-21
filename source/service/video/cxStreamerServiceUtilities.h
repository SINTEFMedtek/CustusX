#ifndef CXSTREAMERSERVICEUTILITIES_H_
#define CXSTREAMERSERVICEUTILITIES_H_

#include <QString>
#include <ctkPluginContext.h>
#include "cxStreamerService.h"

namespace cx
{
/**
 * \brief 
 *
 * \date 21. aug. 2014
 * \author Janne Beate Bakeng, SINTEF
 */

class StreamerServiceUtilities
{
public:
	static StreamerService* getStreamerService(QString name, ctkPluginContext* context);
};

} /* namespace cx */

#endif /* CXSTREAMERSERVICEUTILITIES_H_ */
