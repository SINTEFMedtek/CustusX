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

#include "cxReporter.h"

#include <QtGlobal>
#include <iostream>
#include "boost/shared_ptr.hpp"
#include <QString>
#include <QMutex>
#include <QSound>
#include <QDir>
#include <QTextStream>
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxTime.h"

namespace cx
{


void convertQtMessagesToCxMessages(QtMsgType type, const QMessageLogContext &, const QString &msg)
//void convertQtMessagesToCxMessages(QtMsgType type, const char *msg)
{
	switch (type)
	{
	case QtDebugMsg:
		reportDebug(msg);
		break;
	case QtWarningMsg:
		reportWarning(msg);
		break;
	case QtCriticalMsg:
		reportError(msg);
		break;
	case QtFatalMsg:
		reportError(msg);
		//abort(); here we hope for the best instead of aborting...
	}
}

Message::Message(QString text, MESSAGE_LEVEL messageLevel, int timeoutTime, QString sourceLocation) :
  mText(text),
  mMessageLevel(messageLevel),
  mTimeoutTime(timeoutTime),
  mTimeStamp(QDateTime::currentDateTime()),
  mSourceLocation(sourceLocation)
{};

Message::~Message(){};


QString Message::getPrintableMessage() const
{
	QString source = QString("");
	if (!mSourceLocation.isEmpty())
		source = QString("[%1]").arg(mSourceLocation);

	QString printableMessage;
	if(mMessageLevel == mlRAW)
		printableMessage = mText;
	else
		printableMessage = QString("[%1]%2[%3] %4")
							.arg(mTimeStamp.toString("hh:mm:ss.zzz"))
							.arg(source)
							.arg(qstring_cast(mMessageLevel))
							.arg(mText);

	return printableMessage;
}

MESSAGE_LEVEL Message::getMessageLevel() const
{
  return mMessageLevel;
}

QString Message::getText() const
{
  return mText;
}

QDateTime Message::getTimeStamp() const
{
  return mTimeStamp;
}
int Message::getTimeout() const
{
  return mTimeoutTime;
}
// --------------------------------------------------------
// --------------------------------------------------------

namespace
{
// --------------------------------------------------------


/** streambuf subclass: used to override either cout or cerr and
 * reroute text to both Reporter and original stream.
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
    {
      QMutexLocker sentry(&mOrigMutex);
      mOrig->sputc(single);
    }

    if (mEnabledRedirect)
    {
      if (single == '\n')
      {
        QMutexLocker sentry(&mMutex);
        QString buffer = qstring_cast(mBuffer);
        mBuffer.clear();
        sentry.unlock();

        reporter()->sendMessage(buffer, mMessageLevel, 0);
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

  //this is threadsafe fix...
  void sendUnredirected(const QString& sequence)
  {
	  QMutexLocker sentry(&mOrigMutex);
	  mOrig->sputn(sequence.toLatin1(), sequence.size());
  }

private:
  bool mEnabledRedirect;
  QString mBuffer;
  std::streambuf* mOrig;
  MESSAGE_LEVEL mMessageLevel;
  QMutex mMutex;
  QMutex mOrigMutex;
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

  void sendUnredirected(const QString& sequence)
  {
	  StreamBuf->sendUnredirected(sequence);
  }

};

Reporter* Reporter::mTheInstance = NULL;
Reporter* reporter() { return Reporter::getInstance(); }


Reporter::Reporter() :
	mEnabled(false)
{
  qInstallMessageHandler(convertQtMessagesToCxMessages);
  qRegisterMetaType<Message>("Message");
}

Reporter::~Reporter()
{
  mCout.reset();
  mCerr.reset();
}

Reporter* Reporter::getInstance()
{
  if(mTheInstance == NULL)
  {
    mTheInstance = new Reporter();
  }
  return mTheInstance;
}

void Reporter::initialize()
{
	Reporter::getInstance()->initializeObject();
}

void Reporter::initializeObject()
{
	// must clear both before reinit (in case of nested initializing)
	// otherwise we get a segfault.
	mCout.reset();
	mCerr.reset();

	mCout.reset(new SingleStreamerImpl(std::cout, mlCOUT));
	mCerr.reset(new SingleStreamerImpl(std::cerr, mlCERR));
	mEnabled = true;
}

void Reporter::setFormat(Format format)
{
	mFormat = format;
}

void Reporter::setEnabled(bool enabled)
{
	mEnabled = enabled;
}

bool Reporter::isEnabled() const
{
	return mEnabled;
}


void Reporter::shutdown()
{
  delete mTheInstance;
  mTheInstance = NULL;
}

bool Reporter::setLogFile(QString filename)
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
void Reporter::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	this->setLogFile(absoluteLoggingFolderPath + "/ConsoleLog.txt");
}

void Reporter::setAudioSource(AudioPtr audioSource)
{
  mAudioSource = audioSource;
}

bool Reporter::hasAudioSource() const
{
  return mAudioSource ? true : false;
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendInfo(QString info)
#else
void Reporter::sendInfoRedefined(QString info)
#endif
{
  this->sendMessage(info, mlINFO, 1500);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendSuccess(QString success)
#else
void Reporter::sendSuccessRedefined(QString success)
#endif
{
  this->sendMessage(success, mlSUCCESS, 1500);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendWarning(QString warning)
#else
void Reporter::sendWarningRedefined(QString warning)
#endif
{
  this->sendMessage(warning, mlWARNING, 3000);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendError(QString error)
#else
void Reporter::sendErrorRedefined(QString error)
#endif
{
  this->sendMessage(error, mlERROR, 0);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendDebug(QString debug)
#else
void Reporter::sendDebugRedefined(QString debug)
#endif
{
  this->sendMessage(debug, mlDEBUG, 0);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendVolatile(QString volatile_msg)
#else
void Reporter::sendVolatileRedefined(QString volatile_msg)
#endif
{
  this->sendMessage(volatile_msg, mlVOLATILE, 5000);
}

#ifndef SSC_PRINT_CALLER_INFO
void Reporter::sendRaw(QString raw)
#else
void Reporter::sendRawRedefined(QString raw)
#endif
{
  this->sendMessage(raw, mlRAW, 0);
}

#ifdef SSC_PRINT_CALLER_INFO
void Reporter::sendCallerInformation(const std::string &caller, const std::string &file, int line)
{
	printf("\t\t[FUNCTION] %s\n",caller.c_str());
	printf("\t\t[FILE] %s: %i\n",file.c_str(), line);
}

void Reporter::sendInfoWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendInfoRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}

void Reporter::sendSuccessWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendSuccessRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}

void Reporter::sendWarningWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendWarningRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}

void Reporter::sendErrorWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendErrorRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}

void Reporter::sendDebugWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendDebugRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}

void Reporter::sendVolatileWithCallerInfo(QString info, const std::string &caller, const std::string &file, int line)
{
	this->sendVolatileRedefined(info);
	this->sendCallerInformation(caller, file, line);
	printf("\n");
}
#endif

void Reporter::sendMessage(QString text, MESSAGE_LEVEL messageLevel, int timeout, bool mute, QString sourceLocation)
{
	if (!this->isEnabled())
		return;

	Message message(text, messageLevel, timeout, sourceLocation);

	if (messageLevel!=mlVOLATILE)
	{
		if (mCout)
		{
			if (messageLevel != mlCOUT && messageLevel != mlCERR)
				mCout->sendUnredirected(message.getPrintableMessage()+"\n");
		}

		this->appendToLogfile(this->formatMessage(message) + "\n");
	}

	if (!mute)
		this->playSound(messageLevel);

	emit emittedMessage(message);
}

void Reporter::playSound(MESSAGE_LEVEL messageLevel)
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

void Reporter::playStartSound()
{
  if(this->hasAudioSource())
    mAudioSource->playStartSound();
}

void Reporter::playStopSound()
{
  if(this->hasAudioSource())
    mAudioSource->playStopSound();
}

void Reporter::playCancelSound()
{
  if(this->hasAudioSource())
    mAudioSource->playCancelSound();
}

void Reporter::playSuccessSound()
{
  if(this->hasAudioSource())
    mAudioSource->playSuccessSound();
}

void Reporter::playWarningSound()
{
  if(this->hasAudioSource())
    mAudioSource->playWarningSound();
}

void Reporter::playErrorSound()
{
  if(this->hasAudioSource())
    mAudioSource->playErrorSound();
}

void Reporter::playScreenShotSound()
{
  if(this->hasAudioSource())
    mAudioSource->playScreenShotSound();
}

void Reporter::playSampleSound()
{
  if(this->hasAudioSource())
    mAudioSource->playSampleSound();
}

QString Reporter::formatMessage(Message msg)
{
	QString retval;

	QString bra = (mFormat.mShowBrackets ? "[" : "");
	QString ket = (mFormat.mShowBrackets ? "]" : "");

	// timestamp in front
	retval += bra + msg.getTimeStamp().toString("hh:mm:ss.zzz") + ket;

	// show source location
	if (!msg.getSourceLocation().isEmpty())
		retval += " " + bra + msg.getSourceLocation() + ket;

	// show level if set, or anyway if one of error/warning/success
	if (mFormat.mShowLevel
			|| msg.getMessageLevel() == mlERROR
			|| msg.getMessageLevel() == mlWARNING
			|| msg.getMessageLevel() == mlSUCCESS)
		retval += " " + bra + qstring_cast(msg.getMessageLevel()) + ket;

	// add message text at end.
	retval += " " + msg.getText();

	return retval;
}

/** Open the logfile and append the input text to it
 */
bool Reporter::appendToLogfile(QString text)
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

Reporter::Format::Format() :
	mShowBrackets(true),
	mShowLevel(true),
	mShowSourceLocation(true)
{}


} //End namespace cx
