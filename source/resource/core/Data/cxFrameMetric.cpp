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
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

DataPtr FrameMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new FrameMetric(uid, filename));
}

FrameMetricPtr FrameMetric::create(QString uid, QString name)
{
    return FrameMetricPtr(new FrameMetric(uid, name));
}

FrameMetricPtr FrameMetric::create(QDomNode node)
{
    FrameMetricPtr retval = FrameMetric::create("");
    retval->parseXml(node);
    return retval;
}

FrameMetric::FrameMetric(const QString& uid, const QString& name) :
		cx::FrameMetricBase(uid, name)
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
