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

#include "cxFrameMetric.h"
#include "cxTool.h"
#include "cxTypeConversions.h"

namespace cx
{

FrameMetricPtr FrameMetric::create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return FrameMetricPtr(new FrameMetric(uid, name, dataManager, spaceProvider));
}

FrameMetric::FrameMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
		cx::FrameMetricBase(uid, name, dataManager, spaceProvider)
{
}

FrameMetric::~FrameMetric()
{
}

void FrameMetric::addXml(QDomNode& dataNode)
{
	DataMetric::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("frame", qstring_cast(mFrame));
}

void FrameMetric::parseXml(QDomNode& dataNode)
{
	DataMetric::parseXml(dataNode);

	this->setSpace(CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setFrame(Transform3D::fromString(dataNode.toElement().attribute("frame", qstring_cast(mFrame))));
}

QString FrameMetric::getAsSingleLineString() const
{
	return QString("%1 \"%2\" %3")
			.arg(this->getSingleLineHeader())
			.arg(mSpace.toString())
			.arg(this->matrixAsSingleLineString());
}

} //namespace cx
