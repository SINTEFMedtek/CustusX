/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREPORTER_H_
#define CXREPORTER_H_

#include "cxResourceExport.h"

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "cxDefinitions.h"
#include "cxAudio.h"
#include <sstream>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxLogMessage.h"
#include "cxLogFileWatcher.h"

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
typedef boost::shared_ptr<class Reporter> ReporterPtr;

/** \brief Logging service.
 *
 * Send info in different error levels, and route
 * them to file, or to a console via qt signals.
 * Use the class ConsoleWidget for this.
 * Reporter also captures cout and cerr.
 *
 * Use the MessageListener to catch emitted log messages.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \date 16.10.2008
 *
 * \addtogroup cx_resource_core_logger
 */
class cxResource_EXPORT Reporter : public Log
{
  Q_OBJECT

public:
	virtual ~Reporter();

  static void initialize(); ///< Initialize logging, static object is guaranteed to exist at least until shutdown.
  static void shutdown(); ///< shutdown service, destroy static object if none holds a reference.

  static ReporterPtr getInstance(); ///< Returns a reference to the only Reporter that exists.

  void setAudioSource(AudioPtr audioSource); ///< define sounds to go with the messages.

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

private slots:
  virtual void onEmittedMessage(Message message);

protected:
  virtual LogThreadPtr createWorker();

private:
  bool hasAudioSource() const;
  Reporter();
  Reporter(const Reporter&);
  Reporter& operator=(const Reporter&);

  void playSound(MESSAGE_LEVEL messageLevel);

  AudioPtr mAudioSource;

//  static Reporter *mTheInstance; // global variable
  static boost::weak_ptr<Reporter> mWeakInstance; // global variable
  static boost::shared_ptr<Reporter> mPersistentInstance; // global variable
};

/**Shortcut for accessing the message manager instance.
 */
cxResource_EXPORT ReporterPtr reporter();


} //namespace cx

/**
 * @}
 */


#endif /* CXREPORTER_H_ */
