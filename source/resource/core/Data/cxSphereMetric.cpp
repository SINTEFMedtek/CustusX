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

#include "cxSphereMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxDataManager.h"

namespace cx
{

//DataPtr SphereMetricReader::load(const QString& uid, const QString& filename)
//{
//	return DataPtr(new SphereMetric(uid, filename));
//}

SphereMetric::SphereMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
	mRadius = 5;
}

SphereMetricPtr SphereMetric::create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return SphereMetricPtr(new SphereMetric(uid, name, dataManager, spaceProvider));
}

//SphereMetricPtr SphereMetric::create(QDomNode node)
//{
//	SphereMetricPtr retval = SphereMetric::create("");
//	retval->parseXml(node);
//	return retval;
//}

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

	mArguments->parseXml(dataNode, mDataManager->getData());
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

QString SphereMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(this->getRadius(), 0, 'f', 1);
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
