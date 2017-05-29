#include "cxFileManagerService.h"
#include "cxFileManagerServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

FileManagerServicePtr FileManagerService::getNullObject()
{
	static FileManagerServicePtr mNull;
	if (!mNull)
		mNull.reset(new FileManagerServiceNull, null_deleter());
	return mNull;
}


}
