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

DataPtr AngleMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new AngleMetric(uid, filename));
}

AngleMetric::AngleMetric(const QString& uid, const QString& name) :
				DataMetric(uid, name)
{
}

AngleMetricPtr AngleMetric::create(QString uid, QString name)
{
    return AngleMetricPtr(new AngleMetric(uid, name));
}

AngleMetricPtr AngleMetric::create(QDomNode node)
{
    AngleMetricPtr retval = AngleMetric::create("");
    retval->parseXml(node);
    return retval;
}

AngleMetric::~AngleMetric()
{
}

void AngleMetric::setArgument(int index, DataPtr p)
{
	if (mArgument[index] == p)
		return;

	if (mArgument[index])
		disconnect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	mArgument[index] = p;

	if (mArgument[index])
		connect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));

	emit transformChanged();
}

DataPtr AngleMetric::getArgument(int index)
{
	return mArgument[index];
}

bool AngleMetric::validArgument(DataPtr p) const
{
	return p->getType() == "pointMetric"; // || p->getType()=="planeMetric";
}

void AngleMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		if (mArgument[i])
			dataNode.toElement().setAttribute(QString("p%1").arg(i), mArgument[i]->getUid());
	}
}

void AngleMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
		this->setArgument(i, dataManager()->getData(uid));
	}
}

bool AngleMetric::isValid() const
{
	return !this->getEndpoints().empty();
}

unsigned AngleMetric::getArgumentCount() const
{
	return (unsigned)mArgument.size();
}

std::vector<Vector3D> AngleMetric::getEndpoints() const
{
	std::vector<Vector3D> p(this->getArgumentCount());
	for (unsigned i = 0; i < p.size(); ++i)
	{
		if (!mArgument[i])
			return std::vector<Vector3D>();
		p[i] = boost::dynamic_pointer_cast<PointMetric>(mArgument[i])->getRefCoord();
	}
	return p;
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

}
