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

#include "cxToolMetric.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

DataPtr ToolMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new ToolMetric(uid, filename));
}

ToolMetricPtr ToolMetric::create(QString uid, QString name)
{
	return ToolMetricPtr(new ToolMetric(uid, name));
}

ToolMetricPtr ToolMetric::create(QDomNode node)
{
	ToolMetricPtr retval = ToolMetric::create("");
	retval->parseXml(node);
	return retval;
}

ToolMetric::ToolMetric(const QString& uid, const QString& name) :
		cx::FrameMetricBase(uid, name),
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
			.arg(mToolName)
			.arg(mToolOffset)
			.arg(mSpace.toString())
			.arg(this->matrixAsSingleLineString());
}

} //namespace cx
