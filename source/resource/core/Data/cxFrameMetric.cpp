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

#include "cxFrameMetric.h"
#include "cxTool.h"
#include "cxTypeConversions.h"

namespace cx
{

FrameMetricPtr FrameMetric::create(QString uid, QString name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider)
{
	return FrameMetricPtr(new FrameMetric(uid, name, dataManager, spaceProvider));
}

FrameMetric::FrameMetric(const QString& uid, const QString& name, DataServicePtr dataManager, SpaceProviderPtr spaceProvider) :
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
