/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXTESTDUMMYDATAMANAGER_H
#define CXTESTDUMMYDATAMANAGER_H

#include "cxForwardDeclarations.h"
#include <QStringList>
class ctkPluginContext;

namespace cxtest
{

struct TestServicesType
{
	cx::DataFactoryPtr mDataFactory;
	cx::DataServicePtr mDataService;
	cx::SpaceProviderPtr mSpaceProvider;
	cx::TrackingServiceOldPtr mTrackingService;
};

/** Create a dummy DataService that is not using a toolmanager internally
  *
  */
cx::DataServicePtr createDummyDataService();
TestServicesType createDummyCoreServices();
void destroyDummyCoreServices(TestServicesType& services);

typedef boost::shared_ptr<class TestServices> TestServicesPtr;

/** A minimal set of services for test usage.
  */
class TestServices : public TestServicesType
{
public:
	static TestServicesPtr create();
	~TestServices();

	cx::DataFactoryPtr dataFactory() { return mDataFactory; }
	cx::DataServicePtr dataService() { return mDataService; }
	cx::SpaceProviderPtr spaceProvider() { return mSpaceProvider; }
	cx::TrackingServiceOldPtr trackingService() { return mTrackingService; }

private:
	TestServices();
};

} // namespace cx

#endif // CXTESTDUMMYDATAMANAGER_H
