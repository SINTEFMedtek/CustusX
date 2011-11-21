#include "sscMessageManager.h"

#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"
#include "sscTime.h"

namespace ssc
{


Message::Message(QString text, MESSAGE_LEVEL messageLevel, int timeoutTime, QString sourceLocation) :
  mText(text),
  mMessageLevel(messageLevel),
  mTimeoutTime(timeoutTime),
  mTimeStamp(QDateTime::currentDateTime()),
  mSourceLocation(sourceLocation)
{};

Message::~Message(){};


QString Message::getPrintableMessage()
{
	return QString("[%1] [%2] [%3] %4")
		.arg(mTimeStamp.toString("hh:mm:ss.zzz"))
		.arg(mSourceLocation)
		.arg(qstring_cast(mMessageLevel))
		.arg(mText);

//  QString message("");
//  message.append(QString("["));
//  message.append(mSourceLocation);
//  message.append(mTimeStamp.toString("hh:mm:ss:zzz"));
//  message.append(QString("] "));
//  message.append(QString("["));
//  message.append(qstring_cast(mMessageLevel));
//  message.append(QString("] "));
//  message.append(mText);
//
//  return message;
}

MESSAGE_LEVEL Message::getMessageLevel()
{
  return mMessageLevel;
}

QString Message::getText()
{
  return mText;
}

QDateTime* Message::getTimeStamp()
{
  return &mTimeStamp;
}
int Message::getTimeout()
{
  return mTimeoutTime;
}
// --------------------------------------------------------
// --------------------------------------------------------


namespace
{
// --------------------------------------------------------


/** streambuf subclass: used to override either cout or cerr and
 * reroute text to both messagemanager and original stream.
 *
 */
class MyStreamBuf: public std::basic_streambuf<char, std::char_traits<char> >
{
public:
  MyStreamBuf(MESSAGE_LEVEL level) :
    mEnabledRedirect(true), mOrig(NULL), mMessageLevel(level)
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
        QMutexLocker sentry(&mMutex);
        QString buffer = qstring_cast(mBuffer);
        mBuffer.clear();
        sentry.unlock();

        messageManager()->sendMessage(buffer, mMessageLevel, 0);
      }
      else
      {
        QMutexLocker sentry(&mMutex);
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
  QString mBuffer;
  std::streambuf* mOrig;
  MESSAGE_LEVEL mMessageLevel;
  QMutex mMutex;
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


MessageManager::MessageManager() :
	mEnabled(false)
{
  typedef ssc::Message Message;
  qRegisterMetaType<Message>("Message");
}

MessageManager::~MessageManager()
{
  mCout.reset();
  mCerr.reset();
}

MessageManager* MessageManager::getInstance()
{
  if(mTheInstance == NULL)
  {
    mTheInstance = new MessageManager();
  }
  return mTheInstance;
}

void MessageManager::initialize()
{
	MessageManager::getInstance()->initializeObject();
}

void MessageManager::initializeObject()
{
  mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
  mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));
  mEnabled = true;
}

void MessageManager::setFormat(Format format)
{
	mFormat = format;
}

void MessageManager::setEnabled(bool enabled)
{
	mEnabled = enabled;
}

bool MessageManager::isEnabled() const
{
	return mEnabled;
}


void MessageManager::shutdown()
{
  delete mTheInstance;
  mTheInstance = NULL;
}

bool MessageManager::setLogFile(QString filename)
{
	mEnabled = true;
	mLogFile = filename;
	QFileInfo(mLogFile).absoluteDir().mkpath(".");

	QString timestamp = QDateTime::currentDateTime().toString(timestampMilliSecondsFormatNice());
	QString text = QString("-------> Logging initialized %1\n").arg(timestamp);
	bool success = this->appendToLogfile(text);
	if (!success)
	{
		this->sendError("Failed to open log file " + mLogFile);
		mLogFile = "";
	}
	return success;
}

/** Backwards compatibility only
 *
 */
void MessageManager::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	this->setLogFile(absoluteLoggingFolderPath + "/ConsoleLog.txt");
}

void MessageManager::setAudioSource(ssc::AudioPtr audioSource)
{
  mAudioSource = audioSource;
}

bool MessageManager::hasAudioSource() const
{
  return mAudioSource;
}

void MessageManager::sendInfo(QString info)
{
  this->sendMessage(info, mlINFO, 1500);
}

void MessageManager::sendSuccess(QString success, bool mute)
{
  this->sendMessage(success, mlSUCCESS, 1500);
}

void MessageManager::sendWarning(QString warning, bool mute)
{
  this->sendMessage(warning, mlWARNING, 3000);
}

void MessageManager::sendError(QString error, bool mute)
{
  this->sendMessage(error, mlERROR, 0);
}
  
void MessageManager::sendDebug(QString debug)
{
  this->sendMessage(debug, mlDEBUG, 0);
}

void MessageManager::sendMessage(QString text, MESSAGE_LEVEL messageLevel, int timeout, bool mute, QString sourceLocation)
{
	if (!this->isEnabled())
		return;

	Message message(text, messageLevel, timeout, sourceLocation);

	if (mCout)
	{
		// send text to cout if it not already comes from that stream (or cerr)
		mCout->setEnableRedirect(false);
		if (messageLevel != mlCOUT && messageLevel != mlCERR)
			std::cout << message.getPrintableMessage() << std::endl;
		mCout->setEnableRedirect(true);
	}

	this->appendToLogfile(this->formatMessage(message) + "\n");

	if (!mute)
		this->playSound(messageLevel);

	emit emittedMessage(message);
}

void MessageManager::playSound(MESSAGE_LEVEL messageLevel)
{
	switch (messageLevel)
	{
	case mlSUCCESS:
		this->playSuccessSound();
		break;
	case mlWARNING:
		this->playWarningSound();
		break;
	case mlERROR:
		this->playErrorSound();
		break;
	default:
		break;
	}
}

void MessageManager::playStartSound()
{
  if(this->hasAudioSource())
    mAudioSource->playStartSound();
}

void MessageManager::playStopSound()
{
  if(this->hasAudioSource())
    mAudioSource->playStopSound();
}

void MessageManager::playCancelSound()
{
  if(this->hasAudioSource())
    mAudioSource->playCancelSound();
}

void MessageManager::playSuccessSound()
{
  if(this->hasAudioSource())
    mAudioSource->playSuccessSound();
}

void MessageManager::playWarningSound()
{
  if(this->hasAudioSource())
    mAudioSource->playWarningSound();
}

void MessageManager::playErrorSound()
{
  if(this->hasAudioSource())
    mAudioSource->playErrorSound();
}

void MessageManager::playScreenShotSound()
{
  if(this->hasAudioSource())
    mAudioSource->playScreenShotSound();
}

void MessageManager::playSampleSound()
{
  if(this->hasAudioSource())
    mAudioSource->playSampleSound();
}

QString MessageManager::formatMessage(Message msg)
{
	QString retval;

	QString bra = (mFormat.mShowBrackets ? "[" : "");
	QString ket = (mFormat.mShowBrackets ? "]" : "");

	// timestamp in front
	retval += bra + msg.mTimeStamp.toString("hh:mm:ss.zzz") + ket;

	// show source location
	if (!msg.mSourceLocation.isEmpty())
		retval += " " + bra + msg.mSourceLocation + ket;

	// show level if set, or anyway if one of error/warning/success
	if (mFormat.mShowLevel
		|| msg.mMessageLevel == mlERROR
		|| msg.mMessageLevel == mlWARNING
		|| msg.mMessageLevel == mlSUCCESS)
		retval += " " + bra + qstring_cast(msg.mMessageLevel) + ket;

	// add message text at end.
	retval += " " + msg.mText;

	return retval;
}

/** Open the logfile and append the input text to it
 */
bool MessageManager::appendToLogfile(QString text)
{
	if (mLogFile.isEmpty())
		return false;

	QFile file(mLogFile);
	QTextStream stream;

	if (!file.open(QFile::WriteOnly | QFile::Append))
	{
		return false;
	}

	//note: writing to cout here causes recursion: disable cout redirection first.
//	std::cout << "writing: " << text << " to " << mLogFile << std::endl;

	stream.setDevice(&file);
	stream << text;
	stream << flush;

	return true;
}

MessageManager::Format::Format() :
	mShowBrackets(true),
	mShowLevel(true),
	mShowSourceLocation(true)
{
}


} //End namespace ssc
