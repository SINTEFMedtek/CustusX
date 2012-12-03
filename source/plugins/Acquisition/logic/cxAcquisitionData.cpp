/*
 * cxAcquisitionData.cpp
 *
 *  \date Jun 9, 2011
 *      \author christiana
 */

#include <cxAcquisitionData.h>

//#include "cxAcquisitionManager.h"
#include <vector>
#include <QDomNode>
#include <QDateTime>
#include <QStringList>
#include "sscTime.h"

namespace cx
{

AcquisitionData::AcquisitionData(ssc::ReconstructManagerPtr reconstructer) :
	mReconstructer(reconstructer)
{

}

AcquisitionData::~AcquisitionData()
{
}

void AcquisitionData::addRecordSession(RecordSessionPtr session)
{
	mRecordSessions.push_back(session);
	emit recordedSessionsChanged();
}

void AcquisitionData::removeRecordSession(RecordSessionPtr session)
{
	std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
	for(; it != mRecordSessions.end(); ++it)
	{
		if((*it)->getUid() == session->getUid())
			mRecordSessions.erase(it);
	}
	emit recordedSessionsChanged();
}

std::vector<RecordSessionPtr> AcquisitionData::getRecordSessions()
{
	return mRecordSessions;
}

RecordSessionPtr AcquisitionData::getRecordSession(QString uid)
{
	RecordSessionPtr retval;
	std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
	for(; it != mRecordSessions.end(); ++it)
	{
		if((*it)->getUid() == uid)
			retval = (*it);
	}
	return retval;
}

void AcquisitionData::addXml(QDomNode& parentNode)
{
	QDomDocument doc = parentNode.ownerDocument();
	QDomElement base = doc.createElement("stateManager");
	parentNode.appendChild(base);

	QDomElement sessionsNode = doc.createElement("recordSessions");
	std::vector<RecordSessionPtr>::iterator it = mRecordSessions.begin();
	for(; it != mRecordSessions.end(); ++it)
	{
		QDomElement sessionNode = doc.createElement("recordSession");
		(*it)->addXml(sessionNode);
		sessionsNode.appendChild(sessionNode);
	}
	base.appendChild(sessionsNode);
}

void AcquisitionData::parseXml(QDomNode& dataNode)
{
	QDomNode recordsessionsNode = dataNode.namedItem("recordSessions");
	QDomElement recodesessionNode = recordsessionsNode.firstChildElement("recordSession");
	for (; !recodesessionNode.isNull(); recodesessionNode = recodesessionNode.nextSiblingElement("recordSession"))
	{
		RecordSessionPtr session(new RecordSession("", 0,0,""));
		session->parseXml(recodesessionNode);
		this->addRecordSession(session);
	}
}

/**generate a unique uid for use with a recordsession
 *
 */
QString AcquisitionData::getNewUid()
{
	QString retval;
	int max = 0;
	std::vector<RecordSessionPtr> recordsessions = this->getRecordSessions();
	std::vector<RecordSessionPtr>::iterator iter;
	for (iter = recordsessions.begin(); iter != recordsessions.end(); ++iter)
	{
		QString index = (*iter)->getUid().split("_").front();
		max = std::max(max, index.toInt());
	}

	//  retval = qstring_cast(max + 1);
	retval = QString("%1").arg(max + 1, 2, 10, QChar('0'));
	retval += "_" + QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	return retval;
}


}
