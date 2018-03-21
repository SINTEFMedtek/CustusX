/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLogIOStreamRedirecter.h"
#include "cxReporter.h"

#include <iostream>

namespace cx
{


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

		Message msg(buffer, mMessageLevel);
//		msg.mChannel = qstring_cast(mMessageLevel);
		msg.mChannel = "stdout";
		reporter()->sendMessage(msg);
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



SingleStreamerImpl::SingleStreamerImpl(std::ostream& str, MESSAGE_LEVEL level) :
	mStream(str)
{
	StreamBuf.reset(new MyStreamBuf(level));
	OrigBuf = mStream.rdbuf(StreamBuf.get());
	StreamBuf->setOriginal(OrigBuf);
}

SingleStreamerImpl::~SingleStreamerImpl()
{
	mStream.rdbuf(OrigBuf);
}

void SingleStreamerImpl::sendUnredirected(const QString& sequence)
{
	StreamBuf->sendUnredirected(sequence);
}




} //End namespace cx
