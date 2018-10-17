/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

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
