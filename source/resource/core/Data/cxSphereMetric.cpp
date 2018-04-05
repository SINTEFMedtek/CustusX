/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSphereMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"

namespace cx
{

SphereMetric::SphereMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
	mRadius = 5;
}

SphereMetricPtr SphereMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return SphereMetricPtr(new SphereMetric(uid, name, dataManager, spaceProvider));
}

SphereMetric::~SphereMetric()
{
}

void SphereMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);
	mArguments->addXml(dataNode);
	dataNode.toElement().setAttribute("radius", mRadius);
}

void SphereMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getDatas());
	mRadius = dataNode.toElement().attribute("radius", qstring_cast(mRadius)).toDouble();
}

bool SphereMetric::isValid() const
{
	return !mArguments->getRefCoords().empty();
}

Vector3D SphereMetric::getRefCoord() const
{
	return this->boundingBox().center();
}

DoubleBoundingBox3D SphereMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(mArguments->getRefCoords());
}

void SphereMetric::setRadius(double val)
{
	mRadius = val;
	emit propertiesChanged();
}

double SphereMetric::getRadius() const
{
	return mRadius;
}

QString SphereMetric::getValueAsString() const
{
	return QString("%1").arg(this->getRadius(), 0, 'f', 1);
}

}
