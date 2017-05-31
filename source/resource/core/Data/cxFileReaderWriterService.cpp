#include "cxFileReaderWriterService.h"
#include "cxFileReaderWriterServiceNull.h"
#include "cxNullDeleter.h"

namespace cx
{

FileReaderWriterServicePtr FileReaderWriterService::getNullObject()
{
	static FileReaderWriterServicePtr mNull;
	if (!mNull)
		mNull.reset(new FileReaderWriterServiceNull, null_deleter());
	return mNull;
}
}
