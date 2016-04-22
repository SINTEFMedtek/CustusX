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
#include "cxXMLNodeWrapper.h"

namespace cx
{

RecordSession::RecordSession()
{

}

RecordSession::RecordSession(int id, QString category) :
	mId(id),
	mCategory(category)
{
	mTimestamp = QDateTime::currentDateTime();
}


RecordSession::~RecordSession()
{}

void RecordSession::startNewInterval()
{
	QDateTime time = QDateTime::currentDateTime();
	mIntervals.push_back(IntervalType(time, time));
}

void RecordSession::stopLastInterval()
{
	if (mIntervals.empty())
		return;
	QDateTime time = QDateTime::currentDateTime();
	mIntervals.back().second = time;
}

void RecordSession::cancelLastInterval()
{
	if (mIntervals.empty())
		return;
	mIntervals.pop_back();
}

QDateTime RecordSession::getTimestamp() const
{
	return mTimestamp;
}

QString RecordSession::getHumanDescription() const
{
	QString format("hh:mm");
	QString ts = this->getTimestamp().toString(format);

	return QString("%1 %2 %3")
			.arg(mCategory)
			.arg(mId)
			.arg(ts);
}

QString RecordSession::getUid() const
{
	QString timestamp = this->getTimestamp().toString(timestampSecondsFormat());

	return QString("%1_%2")
			.arg(mId, 2, 10, QChar('0'))
			.arg(timestamp);
}

QString RecordSession::getDescription() const
{
	QString timestamp = this->getTimestamp().toString(timestampSecondsFormat());
	return QString("%1_%2_%3")
			.arg(mCategory)
			.arg(mId)
			.arg(timestamp);
}

void RecordSession::addXml(QDomNode& node)
{
	XMLNodeAdder adder(node);
	adder.node().toElement().setAttribute("uid", this->getUid());
	adder.addTextToElement("category", mCategory);

	for (unsigned i=0; i<mIntervals.size(); ++i)
	{
		QDomElement interval = adder.addElement("interval");
		interval.setAttribute("start", this->datetime2timestamp(mIntervals[i].first));
		interval.setAttribute("stop", this->datetime2timestamp(mIntervals[i].second));
	}
}

void RecordSession::parseXml(QDomNode& node)
{
	if (node.isNull())
	{
		reportWarning("RecordSession::parseXml() node is null");
		return;
	}

	if (this->isOldStyleXmlFormat(node))
	{
		this->parseXml_oldstyle(node);
		return;
	}

	XMLNodeParser parser(node);

	QDomElement base = node.toElement();

	this->setIdAndTimestampFromUid(base.attribute("uid"));
	mCategory = parser.parseTextFromElement("category");

	std::vector<QDomElement> intervals = parser.getDuplicateElements("interval");
	for (unsigned i=0; i<intervals.size(); ++i)
	{
		QDomElement source = intervals[i].toElement();
		IntervalType interval;
		bool ok;
		interval.first = this->timestamp2datetime(source.attribute("start"));
		interval.second = this->timestamp2datetime(source.attribute("stop"));
		mIntervals.push_back(interval);
	}
}

QDateTime RecordSession::timestamp2datetime(QString in) const
{
	return QDateTime::fromString(in, timestampMilliSecondsFormat());
}

QString RecordSession::datetime2timestamp(QDateTime in) const
{
	return in.toString(timestampMilliSecondsFormat());
}

bool RecordSession::isOldStyleXmlFormat(QDomNode& node)
{
	return node.firstChildElement("description").isElement();
}

void RecordSession::parseXml_oldstyle(QDomNode& parentNode)
{
	if (parentNode.isNull())
	{
		reportWarning("RecordSession::parseXml() parentnode is null");
		return;
	}

	QDomElement base = parentNode.toElement();

	QString uid = base.attribute("uid");

	bool ok;
	double starttime = parentNode.namedItem("start").toElement().text().toDouble(&ok);
	double stoptime = parentNode.namedItem("stop").toElement().text().toDouble(&ok);
	QString description = parentNode.namedItem("description").toElement().text();

	mCategory = description.split("_"+uid).first();
	this->setIdAndTimestampFromUid(uid);
	mIntervals.push_back(std::make_pair(QDateTime::fromMSecsSinceEpoch(starttime),
										QDateTime::fromMSecsSinceEpoch(stoptime)));
}

void RecordSession::setIdAndTimestampFromUid(QString uid)
{
	mId = uid.split("_").first().toInt();
	mTimestamp = QDateTime::fromString(uid.split("_").last(), timestampSecondsFormat());
}

TimedTransformMap RecordSession::getToolHistory_prMt(ToolPtr tool, RecordSessionPtr session, bool verbose)
{
	TimedTransformMap retval;

	if(tool && session)
	{
		for (unsigned i=0; i<session->mIntervals.size(); ++i)
		{
			TimedTransformMap values;
			values = tool->getSessionHistory(session->mIntervals[i].first.toMSecsSinceEpoch(),
											 session->mIntervals[i].second.toMSecsSinceEpoch());
			retval.insert(values.begin(), values.end());
		}
	}

	if(retval.empty() && session && verbose)
	{
		CX_LOG_ERROR() << QString("Could not find any tracking data for tool [%1] in session [%2]. ")
						  .arg(tool.get() ? tool->getName() : "NULL")
						  .arg(session.get() ? session->getHumanDescription() : "NULL");
	}

	return retval;
}

std::map<double, cx::ToolPositionMetadata> RecordSession::getToolHistory_metadata(ToolPtr tool, RecordSessionPtr session, bool verbose)
{
	std::map<double, cx::ToolPositionMetadata> retval;

	if(tool && session)
	{
		for (unsigned i=0; i<session->mIntervals.size(); ++i)
		{
			double startTime = session->mIntervals[i].first.toMSecsSinceEpoch();
			double stopTime = session->mIntervals[i].second.toMSecsSinceEpoch();
			const std::map<double, cx::ToolPositionMetadata>& values = tool->getMetadataHistory();

			retval.insert(values.lower_bound(startTime),
						  values.upper_bound(stopTime));
		}
	}

	if(retval.empty() && session && verbose)
	{
		CX_LOG_ERROR() << QString("Could not find any tracking meta data for tool [%1] in session [%2]. ")
						  .arg(tool.get() ? tool->getName() : "NULL")
						  .arg(session.get() ? session->getHumanDescription() : "NULL");
	}

	return retval;
}

std::pair<QDateTime, QDateTime> RecordSession::getInterval(int i)
{
	return mIntervals[i];
}

unsigned RecordSession::getIntervalCount() const
{
	return mIntervals.size();
}


}//namespace cx
