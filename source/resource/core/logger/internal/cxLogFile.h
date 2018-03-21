/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGFILE_H
#define CXLOGFILE_H

#include "cxResourceExport.h"
#include "cxLogMessage.h"

namespace cx
{

/**\brief Log file, format, read and write.
 *
 * \addtogroup cx_resource_core_logger
 */
class LogFile
{
public:
	explicit LogFile();
	static LogFile fromChannel(QString path, QString channel);
	static LogFile fromFilename(QString filename);
	virtual ~LogFile() {}

	void writeHeader();
	void write(Message message);
	bool isWritable() const;
	QString getFilename() const;

	std::vector<Message> readMessages();

private:
	QString mPath;
	QString mChannel;
	int mFilePosition;
	QDateTime mInitTimestamp;

	Message readMessageFirstLine(QString line);
	MESSAGE_LEVEL readMessageLevel(QString line);
	QRegExp getRX_Timestamp() const;
	QString formatMessage(Message msg);
	bool appendToLogfile(QString filename, QString text);
	QString readFileTail();
//	QString removeEarlierSessionsAndSetStartTime(QString text);
//	std::vector<std::pair<QDateTime, QString> > splitIntoSessions(QString text);
	QString timestampFormat() const;
	QDateTime readTimestampFromSessionStartLine(QString text);

	void parseTimestamp(QString text, Message* retval);
	void parseThread(QString text, Message* retval);
	void parseSourceFileLine(QString text, Message* retval);
	void parseSourceFunction(QString text, Message* retval);
	QString getIndex(const QStringList& list, int index);

};

} //namespace cx


#endif // CXLOGFILE_H
