#include "sscMessageManager.h"

#include <iostream>
#include <QString>

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
  this->sendMessage(qinfo, mlINFO, 1500);
}

void MessageManager::sendWarning(std::string warning)
{
  QString qwarning(warning.c_str());
  this->sendMessage(qwarning, mlWARNING, 3000);
}

void MessageManager::sendError(std::string error)
{
  QString qerror(error.c_str());
  this->sendMessage(qerror, mlERROR, 0);
}
  
void MessageManager::sendDebug(std::string text)
{
  QString qdebug(text.c_str());
  this->sendMessage(qdebug, mlDEBUG, 0);
}
  
void MessageManager::setCoutFlag(bool onlyCout)
{
  mOnlyCout = onlyCout;
}

void MessageManager::sendMessage(QString text, MESSAGE_LEVEL messageLevel, int timeout)
{
  Message message(text, messageLevel, timeout);
  emit emittedMessage(message);

  //this is for the old system
  this->sendMessage(message.getPrintableMessage(),timeout);
}

void MessageManager::sendMessage(QString message, int timeout)
{
  //old system
  if (mOnlyCout)
    std::cout << string_cast(message) << std::endl;
  else
    emit this->emittedMessage((const QString &)message, timeout);
}



} //End namespace ssc
