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
