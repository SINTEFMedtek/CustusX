/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
