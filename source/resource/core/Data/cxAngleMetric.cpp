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
