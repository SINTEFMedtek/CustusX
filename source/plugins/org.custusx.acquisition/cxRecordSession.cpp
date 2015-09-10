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
#include "cxRecordSession.h"

#include <QDomDocument>
#include <QDomElement>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxTime.h"
#include "cxTool.h"
#include <QDateTime>
#include "cxTime.h"

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

RecordSession::RecordSession(int id, double startTime, double stopTime, QString description) :
	mStartTime(startTime),
	mStopTime(stopTime)
{
	QString timestamp = this->getTimestamp().toString(timestampSecondsFormat());

	mUid = QString("%1_%2")
			.arg(id, 2, 10, QChar('0'))
			.arg(timestamp);

	mDescription = QString("%1_%2")
			.arg(description)
			.arg(mUid);

	//	//  retval = qstring_cast(max + 1);
	//	retval = QString("%1").arg(max + 1, 2, 10, QChar('0'));
	//	retval += "_" + QDateTime::currentDateTime().toString(timestampSecondsFormat());
	//	return retval;
}


RecordSession::~RecordSession()
{}

QDateTime RecordSession::getTimestamp() const
{
	return QDateTime::fromMSecsSinceEpoch(mStartTime);
}

QString RecordSession::getCategory() const
{
	return mDescription.split("_"+mUid).first();
}

int RecordSession::getID() const
{
	return mUid.split("_").first().toInt();
}

QString RecordSession::getHumanDescription() const
{
	QString format("hh:mm");
//	QString format("yyyy-MM-dd hh:mm");
	QString ts = this->getTimestamp().toString(format);

	return QString("%1 %2 %3")
			.arg(this->getCategory())
			.arg(this->getID())
			.arg(ts);
}

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
	{
		reportWarning("RecordSession::parseXml() parentnode is null");
		return;
	}

	QDomElement base = parentNode.toElement();

	mUid = base.attribute("uid");
	bool ok;
	mStartTime = parentNode.namedItem("start").toElement().text().toDouble(&ok);
	mStopTime = parentNode.namedItem("stop").toElement().text().toDouble(&ok);
	mDescription = parentNode.namedItem("description").toElement().text();
}

TimedTransformMap RecordSession::getToolHistory_prMt(ToolPtr tool, RecordSessionPtr session)
{
//	if(!tool)
//		reportError("RecordSession::getToolHistory_prMt(): Tool missing.");

	TimedTransformMap retval;

	if(tool && session)
		retval = tool->getSessionHistory(session->getStartTime(), session->getStopTime());

	if(retval.empty() && session)
	{
		CX_LOG_ERROR() << QString("Could not find any tracking data for tool [%1] in session [%2]. ")
						  .arg(tool.get() ? tool->getName() : "NULL")
						  .arg(session.get() ? session->getHumanDescription() : "NULL");
//		reportError("RecordSession::getToolHistory_prMt(): Could not find any tracking data from session "+session->getUid()+".");
	}

	return retval;
}

}//namespace cx
