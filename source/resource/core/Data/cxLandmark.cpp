/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include "cxLandmark.h"

#include <QDomNode>

#include "cxTypeConversions.h"
#include "cxTime.h"

namespace cx
{

Landmark::Landmark(QString uid, Vector3D coord) :
	mUid(uid), mCoord(coord)
{
	mTimestamp = QDateTime::currentDateTime();
}

Landmark::~Landmark()
{
}

QString Landmark::getUid() const
{
	return mUid;
}

Vector3D Landmark::getCoord() const
{
	return mCoord;
}
QDateTime Landmark::getTimestamp() const
{
	return mTimestamp;
}

void Landmark::addXml(QDomNode& dataNode) const
{
	QDomDocument doc = dataNode.ownerDocument();

	dataNode.toElement().setAttribute("uid", qstring_cast(mUid));

	QDomElement coordNode = doc.createElement("coord");
	coordNode.appendChild(doc.createTextNode(qstring_cast(mCoord)));
	dataNode.appendChild(coordNode);

	QDomElement timestampNode = doc.createElement("timestamp");
	timestampNode.appendChild(doc.createTextNode(mTimestamp.toString(timestampSecondsFormat())));
	dataNode.appendChild(timestampNode);
}
void Landmark::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	QDomElement base = dataNode.toElement();

	mUid = base.attribute("uid");
	mCoord = Vector3D::fromString(dataNode.namedItem("coord").toElement().text());
	mTimestamp = QDateTime::fromString(dataNode.namedItem("timestamp").toElement().text(), timestampSecondsFormat());
}

bool operator<(const Landmark& lhs, const Landmark& rhs)
{
	// attempts an integer comparison; otherwise revert to lexiographical
	bool ok = true;
	int i_lhs = lhs.getUid().toInt(&ok);
	int i_rhs = rhs.getUid().toInt(&ok);
	if (ok)
		return i_lhs < i_rhs;

	return lhs.getUid() < rhs.getUid();
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Landmarks::Landmarks() : QObject(NULL)
{

}

LandmarksPtr Landmarks::create()
{
	return LandmarksPtr(new Landmarks());
}

LandmarkMap Landmarks::getLandmarks()
{
	return mLandmarks;
}

void Landmarks::setLandmark(Landmark landmark)
{
	mLandmarks[landmark.getUid()] = landmark;
	emit landmarkAdded(landmark.getUid());
}

void Landmarks::removeLandmark(QString uid)
{
	mLandmarks.erase(uid);
	emit landmarkRemoved(uid);
}

void Landmarks::clear()
{
	while (!mLandmarks.empty())
		this->removeLandmark(mLandmarks.begin()->first);
}

void Landmarks::addXml(QDomNode dataNode) const
{
	QDomElement landmarksNode = dataNode.toElement();
	QDomDocument doc = dataNode.ownerDocument();

	LandmarkMap::const_iterator it = mLandmarks.begin();
	for (; it != mLandmarks.end(); ++it)
	{
		QDomElement landmarkNode = doc.createElement("landmark");
		it->second.addXml(landmarkNode);
		landmarksNode.appendChild(landmarkNode);
	}
}

void Landmarks::parseXml(QDomNode dataNode)
{
	QDomElement landmarksNode = dataNode.toElement();

	if (dataNode.isNull())
		return;

	QDomElement landmarkNode = landmarksNode.firstChildElement("landmark");
	for (; !landmarkNode.isNull(); landmarkNode = landmarkNode.nextSiblingElement("landmark"))
	{
		Landmark landmark;
		landmark.parseXml(landmarkNode);
		this->setLandmark(landmark);
	}
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

LandmarkProperty::LandmarkProperty(const QString& uid, const QString& name, bool active) :
	mUid(uid), mName(name), mActive(active)
{
	if (mName.isEmpty())
		mName = mUid;
}

void LandmarkProperty::setName(const QString& name)
{
	mName = name;
}

void LandmarkProperty::setActive(bool active)
{
	mActive = active;
}

QString LandmarkProperty::getUid() const
{
	return mUid;
}

bool LandmarkProperty::getActive() const
{
	return mActive;
}

QString LandmarkProperty::getName() const
{
	return mName;
}

void LandmarkProperty::addXml(QDomNode& dataNode)
{
	dataNode.toElement().setAttribute("uid", qstring_cast(mUid));
	dataNode.toElement().setAttribute("active", qstring_cast(mActive));
	dataNode.toElement().setAttribute("name", qstring_cast(mName));
}
void LandmarkProperty::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;

	QDomElement base = dataNode.toElement();
	mUid = base.attribute("uid");
	mActive = base.attribute("active").toInt();
	mName = base.attribute("name");
}

} // namespace cx
