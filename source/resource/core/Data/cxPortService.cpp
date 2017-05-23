#include "cxPortService.h"
#include "cxPortServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

PortServicePtr PortService::getNullObject()
{
	static PortServicePtr mNull;
	if (!mNull)
		mNull.reset(new PortServiceNull, null_deleter());
	return mNull;
}
}
