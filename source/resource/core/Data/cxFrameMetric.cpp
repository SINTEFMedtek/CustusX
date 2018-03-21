/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


} //namespace cx
