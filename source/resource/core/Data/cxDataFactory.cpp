/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#define CREATE_WITH_SERVICES_IF_MATCH(typeName, TYPE) \
{ \
	if (typeName==TYPE::getTypeName()) \
	return TYPE::create(uid, "", mDataManager, mSpaceProvider); \
}

DataPtr DataFactory::createRaw(QString type, QString uid)
{
	CREATE_IF_MATCH(type, Image);
	CREATE_IF_MATCH(type, TrackedStream);
	CREATE_WITH_SERVICES_IF_MATCH(type, Mesh);
	CREATE_WITH_SERVICES_IF_MATCH(type, PointMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, PlaneMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, DistanceMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, AngleMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, FrameMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, ToolMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, DonutMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, CustomMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, SphereMetric);
	CREATE_WITH_SERVICES_IF_MATCH(type, RegionOfInterestMetric);
	return DataPtr ();
}

DataPtr DataFactory::create(QString type, QString uid, QString name)
{
	DataPtr retval = this->createRaw(type, uid);
	if (name.isEmpty())
		name = uid;
	if (retval)
		retval->setName(name);
	return retval;
}

} // namespace cx

