#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include <QObject>
#include <QDateTime>

#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"
#include "sscTime.h"
#include "boost/shared_ptr.hpp"

class QString;
class QDomNode;
class QDomDocument;

namespace ssc
{

class Message
{
public:
  Message(QString text, MESSAGE_LEVEL messageLevel=mlDEBUG, int timeoutTime=0) :
    mText(text),
    mMessageLevel(messageLevel),
    mTimeoutTime(timeoutTime),
    mTimeStamp(QDateTime::currentDateTime())
  {};

  ~Message(){};

  QString getPrintableMessage()
  {
    QString message;
    message.append(QString("["));
    message.append(mTimeStamp.toString("hh:mm:ss:zzz"));
    message.append(QString("] "));
    message.append(QString("["));
    message.append(qstring_cast(mMessageLevel));
    message.append(QString("] "));
    message.append(mText);
    return message;
  }

  MESSAGE_LEVEL getMessageLevel()
  {
    return mMessageLevel;
  }

  QString getText()
  {
    return mText;
  }

  QDateTime* getTimeStamp()
  {
    return &mTimeStamp;
  }

private:
  QString mText;
  MESSAGE_LEVEL mMessageLevel;
  int mTimeoutTime;
  QDateTime mTimeStamp;
};

/**
 * \class MessageManager
 *
 * \brief This class handles delivering messages to a statusbar from all the other
 * classes.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF Health Research
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
  static MessageManager* getInstance(); ///< Returns a reference to the only MessageManager that exists.
  static void destroyInstance(); ///< Should be called by the object that made the MessageManager.

  void sendInfo(std::string info); ///< Used to report successful operations.
  void sendWarning(std::string warning); ///< The program does not need to terminate, but the user might need to do something.
  void sendError(std::string error); ///< The program (might) need to terminate
  void sendDebug(std::string text); ///< Used to output debug info
  void sendMessage(QString text, MESSAGE_LEVEL messageLevel=mlDEBUG, int timeout=0);

  void setCoutFlag(bool onlyCout);///< Tell the MessageManager if messages only should be dumped to cout. Set to false to allow MessageManager to emit messages.

signals:
  void emittedMessage(const QString& text, int timeout); ///< The signal the owner of a statusbar should listen to. DEPRECATED!
  void emittedMessage(Message message); ///< The signal the user should listen to!

private:
  void initialize();
  bool mOnlyCout;///< Tells the MessageManager if the message only should be dumped to cout
  typedef boost::shared_ptr<class SingleStreamerImpl> SingleStreamerImplPtr;
  SingleStreamerImplPtr mCout;
  SingleStreamerImplPtr mCerr;
  MessageManager(); ///< Use getInstance().
  ~MessageManager(){}; ///< Use destroyInstance().
  MessageManager(const MessageManager&){this->getInstance();}; ///< Copycontructur.
  MessageManager& operator=(const MessageManager&){return *this->getInstance();}; ///< Assignment operator.

  void sendMessage(QString text, int timeout); ///< Emits the signal that actually sends the message.

  static MessageManager *mTheInstance; ///< The unique MessageManager.
};

/**Shortcut for accessing the message manager instance.
 */
MessageManager* messageManager();

} //namespace ssc

#endif /* MESSAGEMANAGER_H_ */
