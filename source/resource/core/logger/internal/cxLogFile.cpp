/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogFile.h"

#include <iostream>
#include <QTextStream>
#include <QFileInfo>
#include "cxTime.h"
#include "cxEnumConversion.h"


namespace cx
{

LogFile::LogFile() :
	mFilePosition(0)
{

}

LogFile LogFile::fromChannel(QString path, QString channel)
{
	LogFile retval;
	retval.mPath = path;
	retval.mChannel = channel;
	return retval;
}

QString LogFile::getFilename() const
{
	return QString("%1/org.custusx.log.%2.txt").arg(mPath).arg(mChannel);
}

LogFile LogFile::fromFilename(QString filename)
{
	LogFile retval;
	retval.mPath = QFileInfo(filename).path();
	retval.mChannel = QFileInfo(filename).completeBaseName().split(".").back();
	return retval;
}

void LogFile::writeHeader()
{
	QString timestamp = QDateTime::currentDateTime().toString(timestampMilliSecondsFormatNice());
	QString formatInfo = "[timestamp][source info][severity][thread] <text> ";
	QString text = QString("-------> Logging initialized [%1], format: %2\n").arg(timestamp).arg(formatInfo);
	bool success = this->appendToLogfile(this->getFilename(), text);
//	return success;
}

void LogFile::write(Message message)
{
	QString text = this->formatMessage(message) + "\n";
	this->appendToLogfile(this->getFilename(), text);
}

bool LogFile::isWritable() const
{
	QString filename = this->getFilename();
	if (filename.isEmpty())
		return false;

	QFile file(filename);
	return file.open(QFile::WriteOnly | QFile::Append);
}


QString LogFile::timestampFormat() const
{
	return "hh:mm:ss.zzz";
}

QString LogFile::formatMessage(Message msg)
{
	QString retval;

	// timestamp in front
	retval += QString("[%1]").arg(msg.getTimeStamp().toString(this->timestampFormat()));

	retval += "\t";
	if (!msg.mThread.isEmpty())
		retval += QString("[%1]").arg(msg.mThread);

	retval += "\t";
	if (!msg.mSourceFile.isEmpty())
		retval += QString("[%1:%2]").arg(msg.mSourceFile).arg(msg.mSourceLine);

	retval += "\t";
	if (!msg.mSourceFunction.isEmpty())
		retval += QString("[%1]").arg(msg.mSourceFunction);

	retval += "\t" + QString("[%1]").arg(qstring_cast(msg.getMessageLevel()));

	// add message text at end.
	retval += " " + msg.getText();

	return retval;
}

/** Open the logfile and append the input text to it
 */
bool LogFile::appendToLogfile(QString filename, QString text)
{
	if (filename.isEmpty())
		return false;

	QFile file(filename);
	QTextStream stream;

	if (!file.open(QFile::WriteOnly | QFile::Append))
	{
		return false;
	}

	stream.setDevice(&file);
	stream << text;
	stream << flush;

	return true;
}

QRegExp LogFile::getRX_Timestamp() const
{
	return QRegExp("\\[(\\d\\d:\\d\\d:\\d\\d\\.\\d\\d\\d)\\]");
}

std::vector<Message> LogFile::readMessages()
{
	QString text = this->readFileTail();

//	text = this->removeEarlierSessionsAndSetStartTime(text);
	if (text.endsWith("\n"))
		text.chop(1); // remove endline at end of text, in order to get false linebreaks after each file

	QStringList lines = text.split("\n");

	QRegExp rx_ts = this->getRX_Timestamp();

	std::vector<Message> retval;

	for (int i=0; i<lines.size(); ++i)
	{
		QString line = lines[i];

		QDateTime timestamp = this->readTimestampFromSessionStartLine(lines[i]);
		if (timestamp.isValid())
		{
			mInitTimestamp = timestamp;
			Message msg(QString("Session initialized: %1").arg(mChannel), mlSUCCESS);
			msg.mChannel = mChannel;
			msg.mTimeStamp = timestamp;
			msg.mThread = "";
			retval.push_back(msg);
			continue;
		}

		if (line.count(rx_ts))
		{
			Message msg = this->readMessageFirstLine(lines[i]);
			msg.mChannel = mChannel;
			retval.push_back(msg);
		}
		else
		{
			if (!retval.empty())
				retval.back().mText += "\n"+line;
		}
	}

	return retval;
}

Message LogFile::readMessageFirstLine(QString line)
{
	MESSAGE_LEVEL level = this->readMessageLevel(line);
	if (level==mlCOUNT)
		return Message(line, mlINFO);
	QString levelSymbol = QString("[%1]").arg(enum2string<MESSAGE_LEVEL>(level));
	QStringList parts = line.split(levelSymbol);

	if (parts.size()<2)
		return Message(line, level);

	QString description = parts[1];
	QStringList fields = parts[0].split("\t");

	Message retval(description, level);

	this->parseTimestamp(this->getIndex(fields, 0), &retval);
	this->parseThread(this->getIndex(fields, 1), &retval);
	this->parseSourceFileLine(this->getIndex(fields, 2), &retval);
	this->parseSourceFunction(this->getIndex(fields, 3), &retval);

	return retval;
}

QString LogFile::getIndex(const QStringList& list, int index)
{
	if (0>index || index >= list.size())
		return "";
	QString field = list[index];
	if (field.startsWith("["))
		field.remove(0, 1);
	if (field.endsWith("]"))
		field.chop(1);
	return field;
}

void LogFile::parseTimestamp(QString text, Message* retval)
{
	if (text.isEmpty())
		return;

	retval->mTimeStamp = mInitTimestamp; // reuse date from init, as this is not part of each line
	QTime time = QTime::fromString(text, this->timestampFormat());
	retval->mTimeStamp.setTime(time);
}

void LogFile::parseThread(QString text, Message* retval)
{
	retval->mThread = text;
}

void LogFile::parseSourceFileLine(QString text, Message* retval)
{
	if (!text.count(":"))
		return;

	QStringList parts = text.split(":");
	bool ok = 0;
	retval->mSourceLine = parts.back().toInt(&ok);
	if (!ok)
		return;
	parts.removeLast();
	retval->mSourceFile = parts.join(":");
}

void LogFile::parseSourceFunction(QString text, Message* retval)
{
	if (!text.count("("))
		return;
	retval->mSourceFunction = text;
}

MESSAGE_LEVEL LogFile::readMessageLevel(QString line)
{
	QStringList levels;
	for (int i=0; i<mlCOUNT; ++i)
		levels << enum2string<MESSAGE_LEVEL>((MESSAGE_LEVEL)(i));
	QRegExp rx_level(QString("\\[(%1)\\]").arg(levels.join("|")));

	int pos = rx_level.indexIn(line);
	QStringList hits = rx_level.capturedTexts();

	if (hits.size()<2)
		return mlCOUNT;

	return string2enum<MESSAGE_LEVEL>(hits[1]);
}

QDateTime LogFile::readTimestampFromSessionStartLine(QString text)
{
	QString sessionStartSymbol("------->");
	if (!text.startsWith(sessionStartSymbol))
		return QDateTime();

	QRegExp rx_ts_start("\\[([^\\]]*)");
	if (text.indexOf(rx_ts_start) < 0)
		return QDateTime();

	QString rawTime = rx_ts_start.cap(1);
	QString format = timestampMilliSecondsFormatNice();
	QDateTime ts = QDateTime::fromString(rawTime, format);

	return ts;
}

QString LogFile::readFileTail()
{
	QFile file(this->getFilename());
	file.open(QIODevice::ReadOnly);

	file.seek(mFilePosition);
	QString text = file.readAll();
	mFilePosition = file.pos();

	return text;
}


} //End namespace cx
