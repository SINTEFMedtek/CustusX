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


#include "sscAngleMetric.h"

#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"

#include "sscDataManager.h"

namespace cx
{

//DataPtr AngleMetricReader::load(const QString& uid, const QString& filename)
//{
//	return DataPtr(new AngleMetric(uid, filename));
//}

AngleMetric::AngleMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mUseSimpleVisualization = false;
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "point 0" << "point 1" << "point 2" << "point 3"));
	mArguments->setValidArgumentTypes(QStringList() << "pointMetric");
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SLOT(resetCachedValues()));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
}

AngleMetricPtr AngleMetric::create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return AngleMetricPtr(new AngleMetric(uid, name, dataManager, spaceProvider));
}

//AngleMetricPtr AngleMetric::create(QDomNode node)
//{
//    AngleMetricPtr retval = AngleMetric::create("");
//    retval->parseXml(node);
//    return retval;
//}

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

	mArguments->parseXml(dataNode, mDataManager->getData());

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

QString AngleMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(this->getAngle()));
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
