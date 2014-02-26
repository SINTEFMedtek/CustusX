// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include <sscDataMetric.h>
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"

namespace cx
{

DataMetric::DataMetric(const QString& uid, const QString& name, DataManager* dataManager, SpaceProviderPtr spaceProvider) :
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

QString DataMetric::getSingleLineHeader() const
{
	return QString("%1 \"%2\"")
			.arg(this->getType())
			.arg(mName);
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
