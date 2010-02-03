#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include <QObject.h>

class QString;
class QDomNode;
class QDomDocument;

namespace cx
{
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

signals:
  void emittedMessage(const QString& message, int timeout); ///< The signal the owner of a statusbar should listen to.

private:
  MessageManager(){}; ///< Use getInstance().
  ~MessageManager(){}; ///< Use destroyInstance().
  MessageManager(const MessageManager&){this->getInstance();}; ///< Copycontructur.
  MessageManager& operator=(const MessageManager&){return *this->getInstance();}; ///< Assignment operator.

  void sendMessage(QString &message, int timeout); ///< Emits the signal that actually sends the message.

  static MessageManager *mTheInstance; ///< The unique MessageManager.
};

} //namespace cx

#endif /* MESSAGEMANAGER_H_ */
