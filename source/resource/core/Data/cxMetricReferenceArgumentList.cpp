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

#include "cxMetricReferenceArgumentList.h"
#include "cxData.h"
#include "cxPointMetric.h"
#include <QDomNode>
#include "cxTypeConversions.h"
#include "cxDataManager.h"

namespace cx
{

MetricReferenceArgumentList::MetricReferenceArgumentList(QStringList descriptions) :
	QObject(NULL)
{
	mArgument.resize(descriptions.size());
	mDescriptions = descriptions;
	this->setValidArgumentTypes(QStringList() << "pointMetric");
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

} // namespace cx
