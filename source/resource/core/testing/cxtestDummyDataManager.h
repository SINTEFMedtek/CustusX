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

#include "cxtestresource_export.h"

#include <QStringList>
#include "cxForwardDeclarations.h"
#include "cxVisServices.h"
class ctkPluginContext;

namespace cxtest
{

typedef boost::shared_ptr<class TestVisServices> TestVisServicesPtr;

/**
 * Convenience class combining all services used by visualization.
 *
 * \ingroup cx_resource_view
 *
 * \date Nov 14 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class CXTESTRESOURCE_EXPORT TestVisServices : public cx::VisServices
{
public:
	static TestVisServicesPtr create();
	TestVisServices();
	~TestVisServices();
//	static TestVisServices getNullObjects();

//	VisualizationServicePtr view() { return visualizationService;}

protected:
//	TestVisServices();
//	VisualizationServicePtr visualizationService;
};

//struct CXTESTRESOURCE_EXPORT TestServicesType
//{
//	cx::PatientModelServicePtr mPatientModelService;
//	cx::SpaceProviderPtr mSpaceProvider;
//	cx::TrackingServicePtr mTrackingService;
//};

//TestServicesType CXTESTRESOURCE_EXPORT createDummyCoreServices();
//void CXTESTRESOURCE_EXPORT destroyDummyCoreServices(TestServicesType& services);

//typedef boost::shared_ptr<class TestServices> TestServicesPtr;

///** A minimal set of services for test usage.
//  */
//class CXTESTRESOURCE_EXPORT TestServices : public TestServicesType
//{
//public:
//	static TestServicesPtr create();
//	~TestServices();

//	cx::PatientModelServicePtr patientModelService() { return mPatientModelService; }
//	cx::SpaceProviderPtr spaceProvider() { return mSpaceProvider; }
//	cx::TrackingServicePtr trackingService() { return mTrackingService; }

//private:
//	TestServices();
//};

} // namespace cx

#endif // CXTESTDUMMYDATAMANAGER_H
