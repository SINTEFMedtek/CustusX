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
