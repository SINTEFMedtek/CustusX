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

#include "cxCustomMetric.h"

#include "cxBoundingBox3D.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxTypeConversions.h"

namespace cx
{

CustomMetric::CustomMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
				DataMetric(uid, name, dataManager, spaceProvider)
{
	mArguments.reset(new MetricReferenceArgumentList(QStringList() << "position" << "direction"));
	connect(mArguments.get(), SIGNAL(argumentsChanged()), this, SIGNAL(transformChanged()));
	mRadius = 5;
	mThickness = 2;
	mHeight = 0;
	mFlat = true;
    mSTLFile = "";
}

CustomMetricPtr CustomMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
        return CustomMetricPtr(new CustomMetric(uid, name, dataManager, spaceProvider));
}

CustomMetric::~CustomMetric()
{
}

void CustomMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	mArguments->addXml(dataNode);
	dataNode.toElement().setAttribute("radius", mRadius);
	dataNode.toElement().setAttribute("thickness", mThickness);
	dataNode.toElement().setAttribute("height", mHeight);
	dataNode.toElement().setAttribute("flat", mFlat);
    dataNode.toElement().setAttribute("STLFile", mSTLFile);
}

void CustomMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	mArguments->parseXml(dataNode, mDataManager->getData());
	mRadius = dataNode.toElement().attribute("radius", qstring_cast(mRadius)).toDouble();
	mThickness = dataNode.toElement().attribute("thickness", qstring_cast(mThickness)).toDouble();
	mHeight = dataNode.toElement().attribute("height", qstring_cast(mHeight)).toDouble();
	mFlat = dataNode.toElement().attribute("flat", qstring_cast(mFlat)).toInt();
    mSTLFile = dataNode.toElement().attribute("STLFile", qstring_cast(mSTLFile));
}

bool CustomMetric::isValid() const
{
	return !mArguments->getRefCoords().empty();
}

Vector3D CustomMetric::getRefCoord() const
{
	return mArguments->getRefCoords().front();
}

DoubleBoundingBox3D CustomMetric::boundingBox() const
{
	return DoubleBoundingBox3D::fromCloud(mArguments->getRefCoords());
}

Vector3D CustomMetric::getPosition()
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.empty())
		return Vector3D::Zero();
	return coords[0];
}

Vector3D CustomMetric::getDirection()
{
	std::vector<Vector3D> coords = mArguments->getRefCoords();
	if (coords.size()<2)
		return Vector3D::UnitZ();
	Vector3D diff = (coords[1]-coords[0]);
	if (similar(diff.length(), 0.0))
		return Vector3D(0,1,0);
	return diff.normal();
}


QString CustomMetric::getAsSingleLineString() const
{
	return QString("%1 %2")
			.arg(this->getSingleLineHeader())
			.arg(qstring_cast(""));
}

void CustomMetric::setRadius(double val)
{
	mRadius = val;
	emit propertiesChanged();
}

double CustomMetric::getRadius() const
{
	return mRadius;
}

void CustomMetric::setThickness(double val)
{
	mThickness = val;
	emit propertiesChanged();
}

double CustomMetric::getThickness() const
{
	return mThickness;
}

void CustomMetric::setHeight(double val)
{
	mHeight = val;
    emit propertiesChanged();
}

double CustomMetric::getHeight() const
{
	return mHeight;
}

void CustomMetric::setFlat(bool val)
{
	mFlat = val;
	emit propertiesChanged();
}

bool CustomMetric::getFlat() const
{
	return mFlat;
}

void CustomMetric::setSTLFile(QString val)
{
    mSTLFile = val;
    emit propertiesChanged();
}

QString CustomMetric::getSTLFile() const
{
    return mSTLFile;
}

}
