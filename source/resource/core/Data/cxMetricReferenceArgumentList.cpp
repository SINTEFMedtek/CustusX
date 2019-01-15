/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMetricReferenceArgumentList.h"
#include "cxData.h"
#include "cxPointMetric.h"
#include <QDomNode>
#include "cxTypeConversions.h"

namespace cx
{

MetricReferenceArgumentList::MetricReferenceArgumentList(QStringList descriptions) :
	QObject(NULL)
{
	mArgument.resize(descriptions.size());
	mDescriptions = descriptions;
	this->setValidArgumentTypes(QStringList() << PointMetric::getTypeName());
}

void MetricReferenceArgumentList::setValidArgumentTypes(QStringList types)
{
	mValidTypes = types;
}

void MetricReferenceArgumentList::set(int index, DataPtr p)
{
	if (mArgument[index] == p)
		return;

	if (mArgument[index])
	{
		disconnect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(argumentsChanged()));
		disconnect(mArgument[index].get(), SIGNAL(propertiesChanged()), this, SIGNAL(argumentsChanged()));
	}

	mArgument[index] = p;

	if (mArgument[index])
	{
		connect(mArgument[index].get(), SIGNAL(transformChanged()), this, SIGNAL(argumentsChanged()));
		connect(mArgument[index].get(), SIGNAL(propertiesChanged()), this, SIGNAL(argumentsChanged()));
	}

	emit argumentsChanged();
}

DataPtr MetricReferenceArgumentList::get(int index)
{
	return mArgument[index];
}

bool MetricReferenceArgumentList::validArgument(DataPtr p) const
{
	if (!p)
		return false;
	return mValidTypes.contains(p->getType());
}

unsigned MetricReferenceArgumentList::getCount() const
{
	return (unsigned)mArgument.size();
}

QString MetricReferenceArgumentList::getDescription(int index) const
{
	return mDescriptions[index];
}

std::vector<Vector3D> MetricReferenceArgumentList::getRefCoords() const
{
	std::vector<Vector3D> p(this->getCount());
	for (unsigned i = 0; i < p.size(); ++i)
	{
		DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(mArgument[i]);
		if (!metric)
			return std::vector<Vector3D>();
		p[i] = metric->getRefCoord();
	}
	return p;
}

std::vector<Transform3D> MetricReferenceArgumentList::getRefFrames() const
{
    std::vector<Transform3D> p(this->getCount());
    for (unsigned i = 0; i < p.size(); ++i)
    {
        DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(mArgument[i]);
        if (!metric)
            return std::vector<Transform3D>();
        p[i] = metric->getRefFrame();
    }
    return p;
}

void MetricReferenceArgumentList::addXml(QDomNode& dataNode)
{
	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		if (mArgument[i])
			dataNode.toElement().setAttribute(QString("p%1").arg(i), mArgument[i]->getUid());
	}
}

void MetricReferenceArgumentList::parseXml(QDomNode& dataNode, const std::map<QString, DataPtr>& data)
{
	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		QString uid = dataNode.toElement().attribute(QString("p%1").arg(i), "");
		if (!data.count(uid))
			continue;
		this->set(i, data.find(uid)->second);
	}
}

QString MetricReferenceArgumentList::getAsSingleLineString() const
{
	QString retval;
	for (unsigned i = 0; i < mArgument.size(); ++i)
	{
		retval += mArgument[i]->getUid();
		retval += " ";
	}

	retval.chop(1);
	return retval;
}

} // namespace cx
