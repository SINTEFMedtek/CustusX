/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

} // namespace cx
