/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxPlaneMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTool.h"
#include "cxTypeConversions.h"
#include "cxSpaceListener.h"
#include "cxSpaceProvider.h"
#include "cxPatientModelService.h"

namespace cx
{

PlaneMetric::PlaneMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
	DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
}

PlaneMetricPtr PlaneMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
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
	mArguments->parseXml(dataNode, mDataManager->getDatas());
}

DoubleBoundingBox3D PlaneMetric::boundingBox() const
{
	Vector3D p0_r = this->getRefCoord();
	return DoubleBoundingBox3D(p0_r, p0_r);
}


}
