#ifndef CXTESTFILEMANAGERSERVICEMOCK_H
#define CXTESTFILEMANAGERSERVICEMOCK_H

#include "cxFileManagerServiceBase.h"
#include "cxFileReaderWriterService.h"
#include "cxtestresource_export.h"

namespace cxtest
{

class CXTESTRESOURCE_EXPORT FileManagerServiceMock : public cx::FileManagerServiceBase
{
public:
	FileManagerServiceMock();

};

}//cxtest

#endif // CXTESTFILEMANAGERSERVICEMOCK_H
