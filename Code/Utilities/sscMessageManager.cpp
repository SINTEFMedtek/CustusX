#include "sscMessageManager.h"

#include <iostream>
//#include <streambuf>
#include "boost/shared_ptr.hpp"
#include <QString>

namespace ssc
{

namespace
{

/** streambuf subclass: used to override either cout or cerr and
 * reroute text to both messagemanager and original stream.
 *
 */
class MyStreamBuf: public std::basic_streambuf<char, std::char_traits<char> >
{
public:
  MyStreamBuf(MESSAGE_LEVEL level) :
    mOrig(NULL), mEnabledRedirect(true), mMessageLevel(level)
  {
  }
  void setOriginal(std::streambuf* orig)
  {
    mOrig = orig;
  }
  virtual int_type overflow(int_type meta = traits_type::eof())
  {
    char single = traits_type::to_char_type(meta);
    if (mOrig) // send to original stream as well
      mOrig->sputc(single);

    if (mEnabledRedirect)
    {
      if (single == '\n')
      {
        messageManager()->sendMessage(qstring_cast(mBuffer), mMessageLevel, 0);
        mBuffer.clear();
      }
      else
      {
        mBuffer += single;
      }
    }
    return traits_type::not_eof(meta);
  }
  void setEnableRedirect(bool on)
  {
    mEnabledRedirect = on;
  }

  //    virtual int_type overflow(int_type meta=traits_type::eof());
private:
  bool mEnabledRedirect;
  std::string mBuffer;
  std::streambuf* mOrig;
  MESSAGE_LEVEL mMessageLevel;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
}


class SingleStreamerImpl
{
private:
  boost::shared_ptr<class MyStreamBuf> StreamBuf;
  std::streambuf *OrigBuf;
  std::ostream& mStream;
public:
  SingleStreamerImpl(std::ostream& str, MESSAGE_LEVEL level) :
    mStream(str)
  {
    StreamBuf.reset(new MyStreamBuf(level));
    OrigBuf = mStream.rdbuf(StreamBuf.get());
    StreamBuf->setOriginal(OrigBuf);
  }
  ~SingleStreamerImpl()
  {
    mStream.rdbuf(OrigBuf);
  }
  void setEnableRedirect(bool on)
  {
    StreamBuf->setEnableRedirect(on);
  }

};

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
    mTheInstance->initialize();
  }
  return mTheInstance;
}

void MessageManager::initialize()
{
  mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
  mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));
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

  // send text to cout if it not already comes from that stream (or cerr)
  mCout->setEnableRedirect(false);
  if (messageLevel!=mlCOUT && messageLevel!=mlCERR)
    std::cout << message.getPrintableMessage() << std::endl;
  mCout->setEnableRedirect(true);

  if (mOnlyCout)
    return;

  emit emittedMessage(message);
  //this is for the old system
  this->sendMessage(message.getPrintableMessage(),timeout);
}

void MessageManager::sendMessage(QString message, int timeout)
{
  //old system
  //if (mOnlyCout)
  //  std::cout << string_cast(message) << std::endl;
  //else
    emit this->emittedMessage((const QString &)message, timeout);
}



} //End namespace ssc
