/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#include "cxAngleMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"

#include "cxPatientModelService.h"

namespace cx
{

AngleMetric::AngleMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mUseSimpleVisualization = false;
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "point 0" << "point 1" << "point 2" << "point 3"));
	mArguments->setValidArgumentTypes(QStringList() << "pointMetric");
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SLOT(resetCachedValues()));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
}

AngleMetricPtr AngleMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return AngleMetricPtr(new AngleMetric(uid, name, dataManager, spaceProvider));
}

AngleMetric::~AngleMetric()
{
}

void AngleMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);
	mArguments->addXml(dataNode);
	dataNode.toElement().setAttribute("useSimpleVisualization", QString::number(mUseSimpleVisualization));
}

void AngleMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getDatas());

	mUseSimpleVisualization = dataNode.toElement().attribute("useSimpleVisualization", QString::number(mUseSimpleVisualization)).toInt();
	this->resetCachedValues();
}

void AngleMetric::resetCachedValues()
{
	mCachedEndPoints.reset();
}

bool AngleMetric::isValid() const
{
	return !this->getEndpoints().empty();
}

std::vector<Vector3D> AngleMetric::getEndpoints() const
{
	if (!mCachedEndPoints.isValid())
	{
		mCachedEndPoints.set(mArguments->getRefCoords());
	}
	return mCachedEndPoints.get();
}

Vector3D AngleMetric::getRefCoord() const
{
    return this->boundingBox().center();
}

double AngleMetric::getAngle() const
{
	std::vector<Vector3D> p = this->getEndpoints();

	if (p.empty())
		return -1;

	Vector3D a = (p[0] - p[1]).normalized();
	Vector3D b = (p[3] - p[2]).normalized();

	double angle = acos(dot(a, b) / a.length() / b.length());
	return angle;
}

QString AngleMetric::getValueAsString() const
{
	return QString("%1*").arg(this->getAngle() / M_PI * 180, 0, 'f', 1);
}

DoubleBoundingBox3D AngleMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(this->getEndpoints());
}

bool AngleMetric::getUseSimpleVisualization() const
{
	return mUseSimpleVisualization;
}

void AngleMetric::setUseSimpleVisualization(bool val)
{
	mUseSimpleVisualization = val;
	emit propertiesChanged();
}


}
