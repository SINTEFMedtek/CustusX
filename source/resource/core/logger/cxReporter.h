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

#ifndef CXREPORTER_H_
#define CXREPORTER_H_

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

  QString getPrintableMessage() const; ///< Text containing  information appropriate to display
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
//  QString mSourceLocation; ///< file:line/function
  QString mChannel;

  QString mSourceFile;
  QString mSourceFunction;
  int mSourceLine;
};

/**\brief Logging service for SSC.
 *
 * Send info in different error levels, and route
 * them to file, or to a console via qt signals.
 * Use the class ConsoleWidget for this.
 * Reporter also captures cout and cerr.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \date 16.10.2008
 *
 * \addtogroup cx_resource_core_logger
 */
class cxResource_EXPORT Reporter : public QObject
{
  Q_OBJECT

public:
  static void initialize(); ///< initialize service. must be called before use.
  static void shutdown(); ///< shutdown service.

  static Reporter* getInstance(); ///< Returns a reference to the only Reporter that exists.

  void setLoggingFolder(QString absoluteLoggingFolderPath); // deprecated
//  bool setLogFile(QString filename); ///< set a file to write messages to.
  bool isEnabled() const;

  struct Format
  {
	  Format();
	  bool mShowBrackets;
	  bool mShowLevel;
	  bool mShowSourceLocation;
  };

  void setFormat(Format format); ///< fine-tune messaging format
  void setAudioSource(AudioPtr audioSource); ///< define sounds to go with the messages.
  bool hasAudioSource() const;

  //Text
  void sendInfo(QString info); ///< Used to report normal interesting activity, no sound associated
  void sendSuccess(QString success); ///< Used to report larger successful operations, default not muted
  void sendWarning(QString warning); ///< The program does not need to terminate, but the user might need to do something, default not muted
  void sendError(QString error); ///< The program (might) need to terminate, default not muted
  void sendDebug(QString debug); ///< Used to output debug info, no sound associated
  void sendVolatile(QString volatile_msg); ///< Used to output volatile info that changes rapidly, not suited for logging.
  void sendRaw(QString raw); ///< Used to output messages without adding anything to them, can be used as cout when mangling needs to be avoided

  void sendMessage(QString text, MESSAGE_LEVEL messageLevel=mlDEBUG, int timeout=-1, bool mute=false);
  void sendMessage(Message msg);

  //Audio
  void playStartSound(); ///< plays a sound signaling that something has started
  void playStopSound(); ///< plays a sound signaling that something has stopped
  void playCancelSound(); ///< plays a sound signaling that something has been canceled

  void playSuccessSound(); ///< automatically called by sendSuccess if not muted
  void playWarningSound(); ///< automatically called by sendWarning if not muted
  void playErrorSound(); ///< automatically called by sendError if not muted

  void playScreenShotSound(); ///< plays a sound signaling that a screen shot has been taken
  void playSampleSound(); ///< plays a sound signaling that something has been sampled

signals:
  void emittedMessage(Message message); ///< emitted for each new message, in addition to writing to file.

private:
  void initializeObject();
  Reporter();
  virtual ~Reporter();
  Reporter(const Reporter&);
  Reporter& operator=(const Reporter&);

  void setEnabled(bool enabled); ///< enable/disable messaging.

  bool appendToLogfile(QString filename, QString text);
  void playSound(MESSAGE_LEVEL messageLevel);
  QString formatMessage(Message msg);
  int getDefaultTimeout(MESSAGE_LEVEL messageLevel) const;

  bool initializeLogFile(QString filename);
  QString getFilenameForChannel(QString channel) const;

  typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
  SingleStreamerImplPtr mCout;
  SingleStreamerImplPtr mCerr;

  bool mEnabled;
  Format mFormat;
//  QString mLogFile;
  QString mLogPath;
  AudioPtr mAudioSource;

  static Reporter *mTheInstance; // global variable
};

/**Shortcut for accessing the message manager instance.
 */
cxResource_EXPORT Reporter* reporter();


} //namespace cx

typedef cx::Message Message;
Q_DECLARE_METATYPE(Message);

/**
 * @}
 */




#endif /* CXREPORTER_H_ */
