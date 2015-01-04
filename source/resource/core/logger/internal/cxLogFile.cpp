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

#include "cxLogFile.h"

#include <iostream>
#include <QTextStream>
#include <QFileInfo>
#include "cxTime.h"
#include "cxEnumConverter.h"
#include "cxDefinitionStrings.h"


namespace cx
{

LogFile::LogFile() :
//	mFilename(filename),
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

//QString LogFile::getChannelName() const
//{
//	return QFileInfo(mFilename).completeBaseName();
//}

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

//	// show source location
//	if (!msg.getSourceLocation().isEmpty())
//		retval += " " + QString("[%1]").arg(msg.getSourceLocation());

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

	//note: writing to cout here causes recursion: disable cout redirection first.
	//	std::cout << "writing: " << text << " to " << mLogFile << std::endl;

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

	text = this->removeEarlierSessionsAndSetStartTime(text);

	QStringList lines = text.split("\n");

	QRegExp rx_ts = this->getRX_Timestamp();

	std::vector<Message> retval;

	for (int i=0; i<lines.size(); ++i)
	{
		QString line = lines[i];

		if (line.count(rx_ts))
		{
			Message msg = this->readMessageFirstLine(lines[i]);
			msg.mChannel = mChannel;
			retval.push_back(msg);
		}
		else
		{
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
	QRegExp rx_ts = this->getRX_Timestamp();

	if (!text.isEmpty() && rx_ts.indexIn(text)>=0)
	{
		QString format = QString("'['%1']'").arg(this->timestampFormat());

		retval->mTimeStamp = mInitTimestamp; // reuse date from init, as this is not part of each line
		QTime time = QTime::fromString(rx_ts.cap(1), this->timestampFormat());
		retval->mTimeStamp.setTime(time);
	}
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
//	std::cout << "LEVELS: " << levels.join("-") << std::endl;
	QRegExp rx_level(QString("\\[(%1)\\]").arg(levels.join("|")));

	int pos = rx_level.indexIn(line);
	QStringList hits = rx_level.capturedTexts();
//	std::cout << "HIT{\n"<<hits.join("\n")<<"\n}"<<std::endl;
//	QString levelSymbol = "\t"+hits[1]+"\t";

	if (hits.size()<2)
		return mlCOUNT;

	return string2enum<MESSAGE_LEVEL>(hits[1]);
}

QString LogFile::removeEarlierSessionsAndSetStartTime(QString text)
{
	QString sessionStartSymbol("------->");
	if (text.startsWith(sessionStartSymbol))
	{
		int startpos = text.lastIndexOf(sessionStartSymbol);
		int endpos = text.indexOf("\n", startpos); // pos of endline in last startline
//		QString sessionStartLine = text.s;
		QRegExp rx_ts_start("\\[([^\\]]*)");
		if (text.indexOf(rx_ts_start))
		{
			QString rawTime = rx_ts_start.cap(1);
			QString format = timestampMilliSecondsFormatNice();
			QDateTime ts = QDateTime::fromString(rawTime, format);
//			std::cout << "--------------TS-- " << mInitTimestamp.toString(timestampMilliSecondsFormatNice()) << std::endl;
			if (ts.isValid())
			mInitTimestamp = ts;
		}

		text.remove(0, endpos+1);
	}

//	std::cout << "READ{"<<text<<"}"<<std::endl;
	return text;
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
