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
#include "cxLogger.h"
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
#include "cxDataLocations.h"
#include "cxMessageListener.h"

#include "internal/cxReporterThread.h"

namespace cx
{

// --------------------------------------------------------
Reporter* Reporter::mTheInstance = NULL;
// --------------------------------------------------------
Reporter* reporter()
{
	return Reporter::getInstance();
}

Reporter::Reporter()// :
//	mThread(NULL)
{	
//	connect(this, &Reporter::emittedMessage, this, &Reporter::onEmittedMessage);
}

Reporter::~Reporter()
{
	this->stopThread();
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
//	mListener = MessageListener::create();

	this->stopThread();
	this->startThread();
}

void Reporter::startThread()
{
	if (mThread)
		return;

	mThread.reset(new QThread());

	mWorker.reset(new ReporterThread());
	mWorker->moveToThread(mThread.get());
	if (!mLogPath.isEmpty())
		mWorker->setLoggingFolder(mLogPath);
	connect(mWorker.get(), &ReporterThread::emittedMessage, this, &Reporter::onEmittedMessage);

	mThread->start();
}

void Reporter::stopThread()
{
	if (!mThread)
		return;

	disconnect(mWorker.get(), &ReporterThread::emittedMessage, this, &Reporter::onEmittedMessage);
	boost::shared_ptr<class ReporterThread> tempWorker = mWorker;
	mWorker.reset();

	mThread->quit();
	mThread->wait(); // forever or until dead thread

	mThread.reset();
	tempWorker.reset();
}

void Reporter::shutdown()
{
  delete mTheInstance;
  mTheInstance = NULL;
}

void Reporter::setLoggingFolder(QString absoluteLoggingFolderPath)
{
	mLogPath = absoluteLoggingFolderPath;

	if (mThread)
	{
		this->stopThread();
		this->startThread();
	}
}

void Reporter::onEmittedMessage(Message message)
{
	if (!message.mMuted)
		this->playSound(message.getMessageLevel());
}

void Reporter::setAudioSource(AudioPtr audioSource)
{
  mAudioSource = audioSource;
}

bool Reporter::hasAudioSource() const
{
  return mAudioSource ? true : false;
}

void Reporter::sendInfo(QString info)
{
  this->sendMessage(info, mlINFO);
}

void Reporter::sendSuccess(QString success)
{
  this->sendMessage(success, mlSUCCESS);
}

void Reporter::sendWarning(QString warning)
{
  this->sendMessage(warning, mlWARNING);
}

void Reporter::sendError(QString error)
{
  this->sendMessage(error, mlERROR);
}

void Reporter::sendDebug(QString debug)
{
  this->sendMessage(debug, mlDEBUG);
}

void Reporter::sendVolatile(QString volatile_msg)
{
  this->sendMessage(volatile_msg, mlVOLATILE);
}

void Reporter::sendRaw(QString raw)
{
  this->sendMessage(raw, mlRAW);
}

void Reporter::sendMessage(QString text, MESSAGE_LEVEL messageLevel, int timeout, bool mute)
{
	Message message(text, messageLevel, timeout);
	message.mMuted = mute;
	this->sendMessage(message);
}


void Reporter::sendMessage(Message message)
{
	if (!mThread)
	{
		std::cout << message.getPrintableMessage() << std::endl;
		return;
	}

	mWorker->logMessage(message);
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

void Reporter::installObserver(MessageObserverPtr observer, bool resend)
{
	if (mWorker)
		mWorker->installObserver(observer, resend);
}

void Reporter::uninstallObserver(MessageObserverPtr observer)
{
	if (mWorker)
		mWorker->uninstallObserver(observer);
}



} //End namespace cx
