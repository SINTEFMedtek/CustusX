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

#include "cxShapedMetric.h"

#include "sscBoundingBox3D.h"
#include "sscTypeConversions.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"

namespace cx
{

DonutMetric::DonutMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
	mRadius = 5;
	mThickness = 2;
	mHeight = 0;
	mFlat = true;
}

DonutMetricPtr DonutMetric::create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return DonutMetricPtr(new DonutMetric(uid, name, dataManager, spaceProvider));
}

DonutMetric::~DonutMetric()
{
}

void DonutMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	mArguments->addXml(dataNode);
	dataNode.toElement().setAttribute("radius", mRadius);
	dataNode.toElement().setAttribute("thickness", mThickness);
	dataNode.toElement().setAttribute("height", mHeight);
	dataNode.toElement().setAttribute("flat", mFlat);
}

void DonutMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getData());
	mRadius = dataNode.toElement().attribute("radius", qstring_cast(mRadius)).toDouble();
	mThickness = dataNode.toElement().attribute("thickness", qstring_cast(mThickness)).toDouble();
	mHeight = dataNode.toElement().attribute("height", qstring_cast(mHeight)).toDouble();
	mFlat = dataNode.toElement().attribute("flat", qstring_cast(mFlat)).toInt();
}

bool DonutMetric::isValid() const
{
	return !mArguments->getRefCoords().empty();
}

Vector3D DonutMetric::getRefCoord() const
{
	return mArguments->getRefCoords().front();
}

DoubleBoundingBox3D DonutMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(mArguments->getRefCoords());
}

Vector3D DonutMetric::getPosition()
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.empty())
		return Vector3D::Zero();
	return coords[0];
}

Vector3D DonutMetric::getDirection()
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size()<2)
		return Vector3D::UnitZ();
	Vector3D diff = (coords[1]-coords[0]);
	if (similar(diff.length(), 0.0))
		return Vector3D(0,1,0);
	return diff.normal();
}


QString DonutMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(""));
}

void DonutMetric::setRadius(double val)
{
	mRadius = val;
	emit propertiesChanged();
}

double DonutMetric::getRadius() const
{
	return mRadius;
}

void DonutMetric::setThickness(double val)
{
	mThickness = val;
	emit propertiesChanged();
}

double DonutMetric::getThickness() const
{
	return mThickness;
}

void DonutMetric::setHeight(double val)
{
	mHeight = val;
}

double DonutMetric::getHeight() const
{
	return mHeight;
}

void DonutMetric::setFlat(bool val)
{
	mFlat = val;
	emit propertiesChanged();
}

bool DonutMetric::getFlat() const
{
	return mFlat;
}

}
