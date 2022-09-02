/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
#include "cxTime.h"
#include "cxMessageListener.h"

#include "internal/cxReporterThread.h"
#include "internal/cxLogFileWatcherThread.h"

namespace cx
{

// --------------------------------------------------------
boost::weak_ptr<Reporter> Reporter::mWeakInstance;
boost::shared_ptr<Reporter> Reporter::mPersistentInstance;
// --------------------------------------------------------

ReporterPtr reporter()
{
	return Reporter::getInstance();
}

Reporter::Reporter()
{	
}

Reporter::~Reporter()
{
}

ReporterPtr Reporter::getInstance()
{
	ReporterPtr retval = mWeakInstance.lock();
	if (!retval)
	{
		retval.reset(new Reporter());
		mWeakInstance = retval;
	}
	return retval;
}

void Reporter::initialize()
{
	ReporterPtr object = Reporter::getInstance();

	mPersistentInstance = object;
	object->initializeObject();
}

LogThreadPtr Reporter::createWorker()
{
	return LogThreadPtr(new ReporterThread());
}

void Reporter::shutdown()
{
	mPersistentInstance.reset();
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

	if (mWorker)
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

} //End namespace cx
