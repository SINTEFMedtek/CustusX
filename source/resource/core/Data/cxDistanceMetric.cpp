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


#include "cxDistanceMetric.h"
#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPlaneMetric.h"
#include "cxPointMetric.h"

#include "cxPatientModelService.h"

namespace cx
{

DistanceMetric::DistanceMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "line endpoint 0" << "line endpoint 1"));
	mArguments->setValidArgumentTypes(QStringList() << "pointMetric" << "planeMetric");

	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SLOT(resetCachedValues()));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
}

DistanceMetricPtr DistanceMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return DistanceMetricPtr(new DistanceMetric(uid, name, dataManager, spaceProvider));
}

DistanceMetric::~DistanceMetric()
{
}

Vector3D DistanceMetric::getRefCoord() const
{
    return this->boundingBox().center();
}

bool DistanceMetric::isValid() const
{
    return this->getEndpoints().size()==2;
}

void DistanceMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);
	mArguments->addXml(dataNode);
}

void DistanceMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getDatas());
	this->resetCachedValues();
}

void DistanceMetric::resetCachedValues()
{
	mCachedEndPoints.reset();
}

std::vector<Vector3D> DistanceMetric::getEndpoints() const
{
	this->updateCache();
	return mCachedEndPoints.get();
}

void DistanceMetric::updateCache() const
{
	if (!mCachedEndPoints.isValid())
	{
		std::vector<Vector3D> endpoints;
		Vector3D direction;
		this->getEndpointsUncached(&endpoints, &direction);
		mCachedEndPoints.set(endpoints);
		mCachedDirection.set(direction);
	}
}

void DistanceMetric::getEndpointsUncached(std::vector<Vector3D> *endpoints, Vector3D* direction) const
{
	DataPtr a0 = mArguments->get(0);
	DataPtr a1 = mArguments->get(1);

	if (!a0 || !a1)
	{
		endpoints->clear();
		return;
	}
	std::vector<Vector3D> retval(2);
	Vector3D dir;

	// case   I: point-point
	// case  II: point-plane
	// case III: plane-plane (not implemented)

	if ((a0->getType() == "pointMetric") && (a1->getType() == "pointMetric"))
	{
		retval[0] = boost::dynamic_pointer_cast<PointMetric>(a0)->getRefCoord();
		retval[1] = boost::dynamic_pointer_cast<PointMetric>(a1)->getRefCoord();
		dir = (retval[1] - retval[0]).normal();
	}
	else if ((a0->getType() == "planeMetric") && (a1->getType() == "pointMetric"))
	{
		Plane3D plane = boost::dynamic_pointer_cast<PlaneMetric>(a0)->getRefPlane();
		Vector3D p = boost::dynamic_pointer_cast<PointMetric>(a1)->getRefCoord();

		retval[0] = plane.projection(p);
		retval[1] = p;
		dir = plane.normal();
	}
	else if ((a0->getType() == "pointMetric") && (a1->getType() == "planeMetric"))
	{
		Plane3D plane = boost::dynamic_pointer_cast<PlaneMetric>(a1)->getRefPlane();
		Vector3D p = boost::dynamic_pointer_cast<PointMetric>(a0)->getRefCoord();

		retval[1] = plane.projection(p);
		retval[0] = p;
		dir = plane.normal();
	}
	else
	{
		endpoints->clear();
	}

	*endpoints = retval;
	*direction = dir;
}

double DistanceMetric::getDistance() const
{
	std::vector<Vector3D> endpoints = this->getEndpoints();
	if (endpoints.size() != 2)
		return -1;

	Vector3D dir = this->getDirection();

	return dot(endpoints[1] - endpoints[0], dir);
}

Vector3D DistanceMetric::getDirection() const
{
	this->updateCache();
	return mCachedDirection.get();
}

QString DistanceMetric::getValueAsString() const
{
	return QString("%1 mm").arg(this->getDistance(), 0, 'f', 1);
}

DoubleBoundingBox3D DistanceMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(this->getEndpoints());
}

QString DistanceMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(this->getDistance()));
}

}

bool cx::DistanceMetric::isEqual(cx::DataMetricPtr metric)
{
	return this->getSingleLineHeader() == metric->getSingleLineHeader();
}
