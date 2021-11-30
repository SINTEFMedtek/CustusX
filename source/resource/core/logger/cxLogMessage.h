/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLOGMESSAGE_H
#define CXLOGMESSAGE_H

#include "cxResourceExport.h"

//#define SSC_PRINT_CALLER_INFO

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include "boost/shared_ptr.hpp"
#include "cxDefinitions.h"
#include "cxAudio.h"
#include <sstream>
#include "cxTypeConversions.h"
#include "cxLogger.h"

class QString;
class QDomNode;
class QDomDocument;
class QFile;
class QTextStream;

/**
 * \file
 * \addtogroup cx_resource_core_logger
 * @{
 */

namespace cx
{

/**\brief A representation of a Reporter message.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF
 * \date 24.08.2010
 *
 * \sa Reporter
 * \addtogroup cx_resource_core_logger
 */
class cxResource_EXPORT Message
{
public:
	Message(QString text ="", MESSAGE_LEVEL messageLevel=mlDEBUG, int timeoutTime=-1);
	~Message();

	QString getPrintableMessage() const; ///< Text containing information appropriate to display
	MESSAGE_LEVEL getMessageLevel() const; ///< The category of the message
	QString getText() const; ///< The raw message.
	QDateTime getTimeStamp() const; ///< The time at which the message was created.
	int getTimeout() const; ///< Timout tells the statusbar how long it should be displayed, this depends on the message level
	QString getSourceLocation() const;

	//private:
	QString mText;
	MESSAGE_LEVEL mMessageLevel;
	int mTimeoutTime;
	QDateTime mTimeStamp;
	bool mMuted;
	//	QString mSourceLocation; ///< file:line/function
	QString mChannel;
	QString mThread;

	QString mSourceFile;
	QString mSourceFunction;
	int mSourceLine;
private:
	void setThreadName();
};

} //namespace cx

typedef cx::Message Message;
Q_DECLARE_METATYPE(Message);

/**
 * @}
 */

#endif // CXLOGMESSAGE_H
