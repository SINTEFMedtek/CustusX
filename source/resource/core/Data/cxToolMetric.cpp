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

#include "cxToolMetric.h"
#include "cxTool.h"
#include "cxTypeConversions.h"

namespace cx
{

//DataPtr ToolMetricReader::load(const QString& uid, const QString& filename)
//{
//	return DataPtr(new ToolMetric(uid, filename));
//}

ToolMetricPtr ToolMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return ToolMetricPtr(new ToolMetric(uid, name, dataManager, spaceProvider));
}

//ToolMetricPtr ToolMetric::create(QDomNode node)
//{
//	ToolMetricPtr retval = ToolMetric::create("");
//	retval->parseXml(node);
//	return retval;
//}

ToolMetric::ToolMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
		cx::FrameMetricBase(uid, name, dataManager, spaceProvider),
		mToolOffset(0.0)
{
}

ToolMetric::~ToolMetric()
{
}

double ToolMetric::getToolOffset() const
{
	return mToolOffset;
}

void ToolMetric::setToolOffset(double val)
{
	mToolOffset = val;
	emit propertiesChanged();
}

QString ToolMetric::getToolName() const
{
	return mToolName;
}

void ToolMetric::setToolName(const QString& val)
{
	mToolName = val;
	emit propertiesChanged();
}

void ToolMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("frame", qstring_cast(mFrame));

	dataNode.toElement().setAttribute("toolname", mToolName);
	dataNode.toElement().setAttribute("tooloffset", qstring_cast(mToolOffset));
}

void ToolMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	this->setSpace(CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setFrame(Transform3D::fromString(dataNode.toElement().attribute("frame", qstring_cast(mFrame))));
	this->setToolName(dataNode.toElement().attribute("toolname", mToolName));
	this->setToolOffset(dataNode.toElement().attribute("tooloffset", qstring_cast(mToolOffset)).toDouble());
}

QString ToolMetric::getAsSingleLineString() const
{
	return QString("%1 \"%2\" %3 \"%4\" %5")
			.arg(this->getSingleLineHeader())
			.arg(mSpace.toString())
			.arg(mToolName)
			.arg(mToolOffset)
			.arg(this->matrixAsSingleLineString());
}


} //namespace cx
