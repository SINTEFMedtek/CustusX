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

//USAcqRecordSession::USAcqRecordSession(QString uid, double startTime, double stopTime, QString description) :
//    RecordSession(uid, startTime, stopTime, description)
//{
//}

//USAcqRecordSession::~USAcqRecordSession()
//{}

}//namespace cx
