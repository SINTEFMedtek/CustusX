/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxAcquisitionData.h"

//#include "cxAcquisitionManager.h"
#include <vector>
#include <QDomNode>
#include <QDateTime>
#include <QStringList>
#include "cxTime.h"
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxSettings.h"
#include "cxUsReconstructionService.h"
#include "cxReporter.h"
#include "cxVisServices.h"


namespace cx
{
AcquisitionData::AcquisitionData()
{}

AcquisitionData::AcquisitionData(VisServicesPtr services, UsReconstructionServicePtr reconstructer) :
	mServices(services),
	mReconstructer(reconstructer)
{

}

AcquisitionData::~AcquisitionData()
{
}

void AcquisitionData::clear()
{
	mRecordSessions.clear();
	emit recordedSessionsChanged();
}

void AcquisitionData::addRecordSession(RecordSessionPtr session)
{
	mRecordSessions.push_back(session);
	emit recordedSessionsChanged();
}

void AcquisitionData::removeRecordSession(RecordSessionPtr session)
{
	for(int i = 0; i < mRecordSessions.size(); ++i)
	{
		if(mRecordSessions[i]->getUid() == session->getUid())
			mRecordSessions.erase(mRecordSessions.begin() + i);
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
//		RecordSessionPtr session(new RecordSession("", 0,0,""));
		RecordSessionPtr session(new RecordSession());
		session->parseXml(recodesessionNode);
		this->addRecordSession(session);
	}
}

/**generate a unique uid for use with a recordsession
 *
 */
int AcquisitionData::getNewSessionId()
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

	return max+1;
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

Acquisition::Acquisition(AcquisitionDataPtr pluginData, QObject* parent) :
	QObject(parent), mPluginData(pluginData), mCurrentState(AcquisitionService::sNOT_RUNNING),
  mReady(true),
  mInfoText("")
{
	connect(this->getServices()->tracking().get(), &TrackingService::stateChanged, this, &Acquisition::checkIfReadySlot);
	connect(this->getServices()->tracking().get(), &TrackingService::activeToolChanged, this, &Acquisition::checkIfReadySlot);
	this->checkIfReadySlot();
}

Acquisition::~Acquisition()
{
}

bool Acquisition::isReady(AcquisitionService::TYPES context) const
{
			return true;
	if (!context.testFlag(AcquisitionService::tTRACKING))
		return true;
	return mReady;
}

QString Acquisition::getInfoText(AcquisitionService::TYPES context) const
{
	if (!context.testFlag(AcquisitionService::tTRACKING))
		return "";
	return mInfoText;
}

void Acquisition::checkIfReadySlot()
{
	bool tracking = this->getServices()->tracking()->getState()>=Tool::tsTRACKING;
	ToolPtr tool = this->getServices()->tracking()->getActiveTool();

	QString mWhatsMissing;
	mWhatsMissing.clear();

	if(tracking)
	{
		mWhatsMissing = "<font color=green>Ready to record!</font><br>";
		if (!tool || !tool->getVisible())
		{
			mWhatsMissing += "<font color=orange>Tool not visible</font><br>";
		}
	}
	else
	{
		mWhatsMissing.append("<font color=red>Need to start tracking.</font><br>");
	}

	// do not require tracking to be present in order to perform an acquisition.
	this->setReady(tracking, mWhatsMissing);
}

void Acquisition::setReady(bool val, QString text)
{
	mReady = val;
	mInfoText = text;

	emit readinessChanged();
}

void Acquisition::startRecord(AcquisitionService::TYPES context, QString category, RecordSessionPtr session)
{
	if (this->getState()!=AcquisitionService::sNOT_RUNNING)
	{
		report("Already recording a session, stop before trying to start a new record session.");
		return;
	}

	mCurrentContext = context;

	if (session)
		mLatestSession = session;
	else
		mLatestSession.reset(new cx::RecordSession(mPluginData->getNewSessionId(), category));

	mLatestSession->startNewInterval();

	reporter()->playStartSound();
	this->setState(AcquisitionService::sRUNNING);
	emit started();
}

void Acquisition::stopRecord()
{
	if (this->getState()!=AcquisitionService::sRUNNING)
	{
		return;
	}

	mLatestSession->stopLastInterval();
	if (!mPluginData->getRecordSession(mLatestSession->getUid()))
		mPluginData->addRecordSession(mLatestSession);
	reporter()->playStopSound();
	this->setState(AcquisitionService::sNOT_RUNNING);
	emit acquisitionStopped();
}

void Acquisition::cancelRecord()
{
	if (this->getState()!=AcquisitionService::sRUNNING)
	{
		return;
	}
	reporter()->playCancelSound();
	mLatestSession->cancelLastInterval();
	mLatestSession.reset();
	mCurrentContext = AcquisitionService::TYPES();
	this->setState(AcquisitionService::sNOT_RUNNING);
	emit cancelled();
}

void Acquisition::startPostProcessing()
{
	this->setState(AcquisitionService::sPOST_PROCESSING);
}

void Acquisition::stopPostProcessing()
{
	this->setState(AcquisitionService::sNOT_RUNNING);
}

void Acquisition::setState(AcquisitionService::STATE newState)
{
	AcquisitionService::STATE lastState = mCurrentState;
	mCurrentState = newState;

//	// emit some helper signals
//	if (lastState!=AcquisitionService::sRUNNING && newState==AcquisitionService::sRUNNING)
//		emit started();
//	else if (lastState==AcquisitionService::sRUNNING && newState!=AcquisitionService::sRUNNING && mLatestSession)
//		emit acquisitionStopped();
//	else if (lastState==AcquisitionService::sRUNNING && newState!=AcquisitionService::sRUNNING && !mLatestSession)
//		emit cancelled();

	emit stateChanged();
}

VisServicesPtr Acquisition::getServices()
{
	return this->getPluginData()->getServices();
}

}
