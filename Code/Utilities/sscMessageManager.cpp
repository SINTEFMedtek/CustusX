#include "sscMessageManager.h"

#include <iostream>
#include <QString>
#include "sscTypeConversions.h"

namespace ssc
{
MessageManager* MessageManager::mTheInstance = NULL;
MessageManager* messageManager() { return MessageManager::getInstance(); }

  
MessageManager::MessageManager():
  mOnlyCout(true) 
{}
  
MessageManager* MessageManager::getInstance()
{
  if(mTheInstance == NULL)
  {
    mTheInstance = new MessageManager();
  }
  return mTheInstance;
}

void MessageManager::destroyInstance()
{
  delete mTheInstance;
}

void MessageManager::sendInfo(std::string info)
{
  QString qinfo(info.c_str());
  QString message("INFO: ");
  message.append(&qinfo);
  this->sendMessage(message, 1500);
}

void MessageManager::sendWarning(std::string warning)
{
  QString qwarning(warning.c_str());
  QString message("WARNING: ");
  message.append(&qwarning);
  this->sendMessage(message, 3000);
}

void MessageManager::sendError(std::string error)
{
  QString qerror(error.c_str());
  QString message("ERROR: ");
  message.append(&qerror);
  this->sendMessage(message, 0);
}

void MessageManager::setCoutFlag(bool onlyCout)
{
  mOnlyCout = onlyCout;
}
  
void MessageManager::sendMessage(QString &message, int timeout)
{
  if (mOnlyCout)
    std::cout << string_cast(message) << std::endl;
  else
    emit this->emittedMessage((const QString &)message, timeout);
}



} //End namespace ssc
