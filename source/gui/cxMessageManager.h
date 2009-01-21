#ifndef MESSAGEMANAGER_H_
#define MESSAGEMANAGER_H_

#include <QObject.h>

class QString;

namespace cx
{
/**
 * \class MessageManager
 * \brief This class handles delivering messages to a statusbar from all the other
 * classes.
 *
 * \author Janne Beate Lervik Bakeng, SINTEF Health Research
 * \date 16.10.2008
 */
class MessageManager : public QObject
{
  Q_OBJECT

public:
  /**
   * Returns a reference to the only MessageManager that exists.
   * \return The only MessageManager that exists.
   */
  static MessageManager &getInstance();
  /** Should be called by the object that made the MessageManager.*/
  static void destroyInstance();

  /** Sends information to the statusbar. Used to report successful operations.*/
  void sendInfo(std::string info);
  /** Sends a warning to the statusbar. The program does not need to terminate,
   * but the user might need to do something.*/
  void sendWarning(std::string warning);
  /** Sends a errormessage to the statusbar. The program (might) need to
   * terminate*/
  void sendError(std::string error);

signals:
  /** The signal the owner of a statusbar should listen to.*/
  void emittedMessage(const QString& message, int timeout);

private:
  /** Use getInstance().*/
  MessageManager(){};
  /** Use destroyInstance().*/
  ~MessageManager(){};
  /** Copycontructur.*/
  MessageManager(const MessageManager&){this->getInstance();};
  /** Assignment operator*/
  MessageManager& operator=(const MessageManager&){ return this->getInstance();};
  /** Emits the signal that actually sends the message.*/
  void sendMessage(QString &message, int timeout);

  /** The unique ToolManager.*/
  static MessageManager *mTheInstance;
};

} //End namespace cx

#endif /* MESSAGEMANAGER_H_ */
