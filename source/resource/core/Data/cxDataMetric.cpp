/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxDataMetric.h"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"

namespace cx
{

DataMetric::DataMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider) :
	Data(uid, name)
{
	mDataManager = dataManager;
	mSpaceProvider = spaceProvider;

	mColor = QColor(240, 170, 255, 255);
	m_rMd_History = RegistrationHistory::getNullObject();
}

DataMetric::~DataMetric()
{
}

QString DataMetric::getSpace()
{
    return "";
}

void DataMetric::setColor(const QColor& color)
{
	mColor = color;
	emit propertiesChanged();
}

QColor DataMetric::getColor()
{
	return mColor;
}


void DataMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);
	QDomDocument doc = dataNode.ownerDocument();

	QDomElement colorNode = doc.createElement("color");
	colorNode.appendChild(doc.createTextNode(color2string(mColor)));
	dataNode.appendChild(colorNode);
}

void DataMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	if (dataNode.isNull())
		return;

	QDomNode colorNode = dataNode.namedItem("color");
	if (!colorNode.isNull())
	{
		mColor = string2color(colorNode.toElement().text(), mColor);
	}

	emit propertiesChanged();
}


}
