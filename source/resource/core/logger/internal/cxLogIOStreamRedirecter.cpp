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
//		if (isValidMessage(buffer))
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

  bool isValidMessage(QString message)
  {
	  //Some tests fail when something is written in std::err, and VKT writes warnings here.
	  //Temporary fix of failing tests:
	  //Remove VTK Warnings about deprecated classes vtkVolumeTextureMapper3D and vtkOpenGLVolumeTextureMapper3D
	  //VTK writes several lines for each warning
	  if (message.contains("vtkVolumeTextureMapper3D") || message.contains("vtkOpenGLVolumeTextureMapper3D"))
	  {
//		  std::cout << "Found VTK deprecated message. Removing this for now." << std::endl;
		  return false;
	  }
	  else if (message.isEmpty())
		  return false;

	  return true;
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
