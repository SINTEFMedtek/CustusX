/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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


} //namespace cx
