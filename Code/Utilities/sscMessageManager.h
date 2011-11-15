#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include <QMetaType>
#include <QObject>
#include <QMutex>
#include <QDateTime>
#include <QFile>
#include "boost/shared_ptr.hpp"
#include "sscDefinitions.h"
#include "sscAudio.h"

class QString;
class QDomNode;
class QDomDocument;
class QFile;
class QTextStream;

namespace ssc
{
/**
 * \class Message
 *
 * \brief A representation of a MessageManager message.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF
 * \date 24.08.2010
 */
class Message
{
public:
  Message(QString text ="", MESSAGE_LEVEL messageLevel=mlDEBUG, int timeoutTime=0, QString sourceLocation="");
  ~Message();

  QString getPrintableMessage(); ///< Text containing  information appropriate to display
  MESSAGE_LEVEL getMessageLevel(); ///< The category of the message
  QString getText(); ///< The raw message.
  QDateTime* getTimeStamp(); ///< The time at which the message was created.
  int getTimeout(); ///< Timout tells the statusbar how long it should be displayed, this depends on the message level

  QString mText;
  MESSAGE_LEVEL mMessageLevel;
  int mTimeoutTime;
  QDateTime mTimeStamp;
  QString mSourceLocation; ///< file:line/function
};

/**
 * \class MessageManager
 *
 * Logging service for SSC.
 * Send info in different error levels, and route
 * them to file, or to a console via qt signals.
 * Use the class ConsoleWidget for this.
 * MessageManager also captures cout and cerr.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF
 * \author Christian Askeland, SINTEF
 * \date 16.10.2008
 *
 *TODO Sender should be added to the message
 *TODO Enable/disable info, warnings and errors
 *
 */
class MessageManager : public QObject
{
  Q_OBJECT

public:
  static void initialize(); ///< initialize service. must be called before use.
  static void shutdown(); ///< shutdown service.

  static MessageManager* getInstance(); ///< Returns a reference to the only MessageManager that exists.

  void setLoggingFolder(QString absoluteLoggingFolderPath); // deprecated
  bool setLogFile(QString filename); ///< set a file to write messages to.

  struct Format
  {
	  Format();
	  bool mShowBrackets;
	  bool mShowLevel;
	  bool mShowSourceLocation;
  };

  void setFormat(Format format); ///< fine-tune messaging format
  void setAudioSource(ssc::AudioPtr audioSource); ///< define sounds to go with the messages.
  bool hasAudioSource() const;
  void setEnabled(bool enabled); ///< enable/disable messaging.
  bool isEnabled() const;

  //Text
  void sendInfo(QString info); ///< Used to report normal interesting activity, no sound associated
  void sendSuccess(QString success, bool mute=false); ///< Used to report larger successful operations, default not muted
  void sendWarning(QString warning, bool mute=false); ///< The program does not need to terminate, but the user might need to do something, default not muted
  void sendError(QString error, bool mute=false); ///< The program (might) need to terminate, default not muted
  void sendDebug(QString debug); ///< Used to output debug info, no sound associated

  void sendMessage(QString text, MESSAGE_LEVEL messageLevel=mlDEBUG, int timeout=0, bool mute=false, QString sourceLocation="");

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
  MessageManager();
  virtual ~MessageManager();
  MessageManager(const MessageManager&);
  MessageManager& operator=(const MessageManager&);

  bool appendToLogfile(QString text);
  void playSound(MESSAGE_LEVEL messageLevel);
  QString formatMessage(Message msg);

  typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
  SingleStreamerImplPtr mCout;
  SingleStreamerImplPtr mCerr;

  bool mEnabled;
  Format mFormat;
  QString mLogFile;
  ssc::AudioPtr mAudioSource;

  static MessageManager *mTheInstance; ///< The unique MessageManager.
};

/**Shortcut for accessing the message manager instance.
 */
MessageManager* messageManager();

} //namespace ssc

typedef ssc::Message Message;
Q_DECLARE_METATYPE(Message);


#endif /* MESSAGEMANAGER_H_ */
