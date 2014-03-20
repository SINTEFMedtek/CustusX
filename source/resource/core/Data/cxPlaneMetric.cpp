// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "cxPlaneMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTool.h"
#include "cxTypeConversions.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"

namespace cx
{

PlaneMetric::PlaneMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider) :
	DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
}

PlaneMetricPtr PlaneMetric::create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return PlaneMetricPtr(new PlaneMetric(uid, name, dataManager, spaceProvider));
}

PlaneMetric::~PlaneMetric()
{
}

Plane3D PlaneMetric::getRefPlane() const
{
	return Eigen::Hyperplane<double, 3>(this->getRefNormal(), this->getRefCoord());
}

Vector3D PlaneMetric::getRefCoord() const
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size()<2)
		return Vector3D::Zero();
	return coords[0];
}

Vector3D PlaneMetric::getRefNormal() const
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size()<2)
		return Vector3D::UnitZ();
	return (coords[1]-coords[0]).normal();
}


void PlaneMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);
	mArguments->addXml(dataNode);
}

void PlaneMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);
	mArguments->parseXml(dataNode, mDataManager->getData());
}

DoubleBoundingBox3D PlaneMetric::boundingBox() const
{
	Vector3D p0_r = this->getRefCoord();
	return DoubleBoundingBox3D(p0_r, p0_r);
}

QString PlaneMetric::getAsSingleLineString() const
{
	return QString("%1 %2 %3")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(this->getRefCoord()))
			.arg(qstring_cast(this->getRefNormal()));
}


}
