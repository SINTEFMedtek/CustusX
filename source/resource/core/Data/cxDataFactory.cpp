// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxDataFactory.h"

#include "sscImage.h"
#include "sscMesh.h"
#include "sscPointMetric.h"
#include "sscDistanceMetric.h"
#include "sscPlaneMetric.h"
#include "sscAngleMetric.h"
#include "cxShapedMetric.h"
#include "cxSphereMetric.h"
#include "cxFrameMetric.h"
#include "cxToolMetric.h"

namespace cx
{

DataFactory::DataFactory(DataManager* dataManager, SpaceProviderPtr spaceProvider) :
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

DataPtr DataFactory::create(QString type, QString uid, QString name)
{
	CREATE_IF_MATCH(type, Image);
	CREATE_IF_MATCH(type, Mesh);
	CREATE_METRIC_IF_MATCH(type, PointMetric);
	CREATE_METRIC_IF_MATCH(type, PlaneMetric);
	CREATE_METRIC_IF_MATCH(type, DistanceMetric);
	CREATE_METRIC_IF_MATCH(type, AngleMetric);
	CREATE_METRIC_IF_MATCH(type, FrameMetric);
	CREATE_METRIC_IF_MATCH(type, ToolMetric);
	CREATE_METRIC_IF_MATCH(type, DonutMetric);
	CREATE_METRIC_IF_MATCH(type, SphereMetric);
	return DataPtr ();
}

} // namespace cx

