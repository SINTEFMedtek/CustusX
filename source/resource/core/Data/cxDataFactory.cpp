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

#include "cxDataFactory.h"

#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"
#include "cxPointMetric.h"
#include "cxDistanceMetric.h"
#include "cxPlaneMetric.h"
#include "cxAngleMetric.h"
#include "cxShapedMetric.h"
#include "cxCustomMetric.h"
#include "cxSphereMetric.h"
#include "cxFrameMetric.h"
#include "cxToolMetric.h"
#include "cxRegionOfInterestMetric.h"

#include "cxPatientModelService.h"


namespace cx
{

DataFactory::DataFactory(PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
	mDataManager(dataManager),
	mSpaceProvider(spaceProvider)
{

}

#define CREATE_IF_MATCH(typeName, TYPE) \
{ \
	if (typeName==TYPE::getTypeName()) \
		return TYPE::create(uid, ""); \
}
#define CREATE_METRIC_IF_MATCH(typeName, TYPE) \
{ \
	if (typeName==TYPE::getTypeName()) \
		return TYPE::create(uid, "", mDataManager, mSpaceProvider); \
}

DataPtr DataFactory::createRaw(QString type, QString uid)
{
	CREATE_IF_MATCH(type, Image);
	CREATE_IF_MATCH(type, Mesh);
	CREATE_IF_MATCH(type, TrackedStream);
	CREATE_METRIC_IF_MATCH(type, PointMetric);
	CREATE_METRIC_IF_MATCH(type, PlaneMetric);
	CREATE_METRIC_IF_MATCH(type, DistanceMetric);
	CREATE_METRIC_IF_MATCH(type, AngleMetric);
	CREATE_METRIC_IF_MATCH(type, FrameMetric);
	CREATE_METRIC_IF_MATCH(type, ToolMetric);
	CREATE_METRIC_IF_MATCH(type, DonutMetric);
    CREATE_METRIC_IF_MATCH(type, CustomMetric);
	CREATE_METRIC_IF_MATCH(type, SphereMetric);
	CREATE_METRIC_IF_MATCH(type, RegionOfInterestMetric);
	return DataPtr ();
}

DataPtr DataFactory::create(QString type, QString uid, QString name)
{
//	if (mDataManager)
//		mDataManager->generateUidAndName(&uid, &name);

	DataPtr retval = this->createRaw(type, uid);
	if (name.isEmpty())
		name = uid;
	if (retval)
		retval->setName(name);
	return retval;
}

} // namespace cx

