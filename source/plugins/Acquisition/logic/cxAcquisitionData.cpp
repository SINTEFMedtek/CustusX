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

#include "cxAcquisitionData.h"

//#include "cxAcquisitionManager.h"
#include <vector>
#include <QDomNode>
#include <QDateTime>
#include <QStringList>
#include "cxTime.h"
#include "cxReporter.h"
#include "cxToolManager.h"
#include "cxSettings.h"

namespace cx
{
AcquisitionData::AcquisitionData()
{}

AcquisitionData::AcquisitionData(ReconstructManagerPtr reconstructer) :
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
	while (it != mRecordSessions.end())
	{
		std::vector<RecordSessionPtr>::iterator current = it;
		++it;
		if((*current)->getUid() == session->getUid())
			mRecordSessions.erase(current);
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
	retval += "_" + QDateTime::currentDateTime().toString(timestampSecondsFormat());
	return retval;
}


///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Acquisition::Acquisition(AcquisitionDataPtr pluginData, QObject* parent) :
    QObject(parent), mPluginData(pluginData), mCurrentState(sNOT_RUNNING),
    mReady(true), mInfoText("")
{
}

Acquisition::~Acquisition()
{
}

void Acquisition::setReady(bool val, QString text)
{
	mReady = val;
	mInfoText = text;

	if (!mReady && this->getState()==sRUNNING)
		this->cancelRecord();

	emit readinessChanged();
}

void Acquisition::toggleRecord()
{
	if (this->getState()==sRUNNING)
		this->stopRecord();
	else
		this->startRecord();
}

void Acquisition::startRecord()
{
	if (this->getState()!=sNOT_RUNNING)
	{
		report("Already recording a session, stop before trying to start a new record session.");
		return;
	}

	double startTime = getMilliSecondsSinceEpoch();
	mLatestSession.reset(new cx::RecordSession(mPluginData->getNewUid(), startTime, startTime, settings()->value("Ultrasound/acquisitionName").toString()));
	reporter()->playStartSound();
	this->setState(sRUNNING);
}

void Acquisition::stopRecord()
{
	if (this->getState()!=sRUNNING)
	{
		return;
	}

	mLatestSession->setStopTime(getMilliSecondsSinceEpoch());
	mPluginData->addRecordSession(mLatestSession);
	toolManager()->saveToolsSlot(); //asks all the tools to save their transforms and timestamps
	reporter()->playStopSound();
	this->setState(sNOT_RUNNING);
}

void Acquisition::cancelRecord()
{
	if (this->getState()!=sRUNNING)
	{
		return;
	}
	reporter()->playCancelSound();
	mLatestSession.reset();
	this->setState(sNOT_RUNNING);
}

void Acquisition::startPostProcessing()
{
	this->setState(sPOST_PROCESSING);
}

void Acquisition::stopPostProcessing()
{
	this->setState(sNOT_RUNNING);
}

void Acquisition::setState(STATE newState)
{
	STATE lastState = mCurrentState;
	mCurrentState = newState;

	// emit some helper signals
	if (lastState!=sRUNNING && newState==sRUNNING)
		emit started();
	else if (lastState==sRUNNING && newState!=sRUNNING && mLatestSession)
		emit acquisitionStopped();
	else if (lastState==sRUNNING && newState!=sRUNNING && !mLatestSession)
		emit cancelled();

	emit stateChanged();
}


}
