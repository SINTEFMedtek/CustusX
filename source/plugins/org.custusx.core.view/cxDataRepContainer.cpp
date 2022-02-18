/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDataRepContainer.h"

#include "cxView.h"
#include "cxSliceProxy.h"
#include "cxGeometricRep2D.h"
#include "cxSettings.h"
#include "cxData.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxPointMetricRep2D.h"
#include "cxSphereMetricRep2D.h"
#include "cxSphereMetric.h"
#include "cxDistanceMetricRep2D.h"
#include "cxLogger.h"

namespace cx
{

void DataRepContainer::setSliceProxy(SliceProxyPtr sliceProxy)
{
	mSliceProxy = sliceProxy;
}

void DataRepContainer::setView(ViewPtr view)
{
	mView = view;
}

void DataRepContainer::updateSettings()
{
	for (RepMap::iterator iter=mDataReps.begin(); iter!=mDataReps.end(); ++iter)
	{
		this->updateSettings(iter->second);
	}
}

void DataRepContainer::addData(DataPtr data)
{
	if (boost::dynamic_pointer_cast<Mesh>(data))
	{
		this->meshAdded(boost::dynamic_pointer_cast<Mesh>(data));
	}
	else if (boost::dynamic_pointer_cast<PointMetric>(data))
	{
		this->pointMetricAdded(boost::dynamic_pointer_cast<PointMetric>(data));
	}
	else if (boost::dynamic_pointer_cast<SphereMetric>(data))
	{
		this->sphereMetricAdded(boost::dynamic_pointer_cast<SphereMetric>(data));
	}
	else if (boost::dynamic_pointer_cast<DistanceMetric>(data))
	{
		this->distanceMetricAdded(boost::dynamic_pointer_cast<DistanceMetric>(data));
	}
}

void DataRepContainer::removeData(QString uid)
{
	if (!mDataReps.count(uid))
		return;

	mView->removeRep(mDataReps[uid]);
	mDataReps.erase(uid);
}

void DataRepContainer::meshAdded(MeshPtr mesh)
{
	if (!mesh)
		return;
	if (mDataReps.count(mesh->getUid()))
		return;

	GeometricRep2DPtr rep = GeometricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
	rep->setMesh(mesh);
	mView->addRep(rep);
	mDataReps[mesh->getUid()] = rep;
}

void DataRepContainer::pointMetricAdded(PointMetricPtr mesh)
{
	if (!mesh)
		return;
	if (mDataReps.count(mesh->getUid()))
		return;

	PointMetricRep2DPtr rep = PointMetricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
	rep->setDataMetric(mesh);
	rep->setDynamicSize(true);
	mView->addRep(rep);
	mDataReps[mesh->getUid()] = rep;
	this->updateSettings(rep);
}

void DataRepContainer::distanceMetricAdded(DistanceMetricPtr mesh)
{
	if (!mesh)
		return;
	if (mDataReps.count(mesh->getUid()))
		return;

	DistanceMetricRep2DPtr rep = DistanceMetricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
	rep->setDataMetric(mesh);
	rep->setDynamicSize(true);
	mView->addRep(rep);
	mDataReps[mesh->getUid()] = rep;
	this->updateSettings(rep);
}

void DataRepContainer::sphereMetricAdded(SphereMetricPtr mesh)
{
	if (!mesh)
		return;
	if (mDataReps.count(mesh->getUid()))
		return;

	SphereMetricRep2DPtr rep = SphereMetricRep2D::New(mesh->getUid() + "_rep2D");
	rep->setSliceProxy(mSliceProxy);
	rep->setDataMetric(mesh);
	mView->addRep(rep);
	mDataReps[mesh->getUid()] = rep;
	this->updateSettings(rep);
}

void DataRepContainer::updateSettings(RepPtr rep)
{
	DataMetricRepPtr val = boost::dynamic_pointer_cast<DataMetricRep>(rep);
	if (!val)
		return;

	val->setGraphicsSize(settings()->value("View3D/sphereRadius").toDouble());
	val->setShowLabel(settings()->value("View/showLabels").toBool());
	val->setLabelSize(settings()->value("View3D/labelSize").toDouble());
	val->setShowAnnotation(!settings()->value("View/showMetricNamesInCorner").toBool());
}


} // namespace cx
