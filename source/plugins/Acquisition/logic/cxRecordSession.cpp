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
#include "cxRecordSession.h"

#include <QDomDocument>
#include <QDomElement>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscTime.h"

namespace cx
{
RecordSession::RecordSession(QString uid, double startTime, double stopTime, QString description) :
    mStartTime(startTime),
    mStopTime(stopTime),
    mDescription(description)
{
	mUid = uid;
	mDescription.append("_"+mUid+"");
}

RecordSession::~RecordSession()
{}

QString RecordSession::getUid()
{
	return mUid;
}

QString RecordSession::getDescription()
{
	return mDescription;
}

double RecordSession::getStartTime()
{
	return mStartTime;
}

double RecordSession::getStopTime()
{
	return mStopTime;
}

void RecordSession::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();

	parentNode.toElement().setAttribute("uid", mUid);

	QDomElement startNode = doc.createElement("start");
	startNode.appendChild(doc.createTextNode(qstring_cast(mStartTime)));
	parentNode.appendChild(startNode);

	QDomElement stopNode = doc.createElement("stop");
	stopNode.appendChild(doc.createTextNode(qstring_cast(mStopTime)));
	parentNode.appendChild(stopNode);

	QDomElement descriptionNode = doc.createElement("description");
	descriptionNode.appendChild(doc.createTextNode(mDescription));
	parentNode.appendChild(descriptionNode);
}

void RecordSession::parseXml(QDomNode& parentNode)
{
	if (parentNode.isNull())
		return;

	QDomElement base = parentNode.toElement();

	mUid = base.attribute("uid");
	bool ok;
	mStartTime = parentNode.namedItem("start").toElement().text().toInt(&ok);
	mStopTime = parentNode.namedItem("stop").toElement().text().toInt(&ok);
	mDescription = parentNode.namedItem("description").toElement().text();
}

}//namespace cx
