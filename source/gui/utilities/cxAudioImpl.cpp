/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxAudioImpl.h"

#include <QSound>
#include <iostream>
#include "cxDataLocations.h"
#include <QFileInfo>
#include "cxTypeConversions.h"
#include "cxLogger.h"
#include "cxReporter.h"

namespace cx
{

AudioInternal::AudioInternal(QObject* parent) : QObject(parent), mLastPlayTimeMutex(QMutex::Recursive)
{
	mLastPlayTime = QDateTime::fromMSecsSinceEpoch(0);
	mMinTimeBetweenEachSound = 500;
	connect(this, SIGNAL(playSoundInternalSignal(QString)), this, SLOT(playSoundSlot(QString)));
}

void AudioInternal::playSound(QString file)
{
	emit playSoundInternalSignal(file);
}

bool AudioInternal::checkValidTime()
{
	QDateTime now = QDateTime::currentDateTime();

	QMutexLocker sentry(&mLastPlayTimeMutex);
	bool valid = mLastPlayTime.msecsTo(now) > mMinTimeBetweenEachSound;
	if (!valid)
		return false;
	mLastPlayTime = now;
	return true;
}

void AudioInternal::playSoundSlot(QString file)
{
	if (!this->checkValidTime())
		return;

	if (!QFileInfo(file).isAbsolute())
		file = QString("%1/%2").arg(DataLocations::findConfigFolder("/audio/")).arg(file);

	if (!QFileInfo(file).exists())
	{
		QString text = QString("Audio file %1 not found").arg(file);
		reporter()->sendMessage(text, mlWARNING, 3000, true);
		return;
	}

	QSound::play(file);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


AudioImpl::AudioImpl()
{
	mInternal.reset(new AudioInternal());
}

AudioImpl::~AudioImpl()
{}

void AudioImpl::playStartSound()
{
	mInternal->playSound("Windows XP Hardware Insert.wav");
}

void AudioImpl::playStopSound()
{
	mInternal->playSound("Windows XP Hardware Remove.wav");
}

void AudioImpl::playCancelSound()
{
	mInternal->playSound("Windows XP Hardware Fail.wav");
}

void AudioImpl::playSuccessSound()
{
	mInternal->playSound("Windows XP Print complete.wav");
}

void AudioImpl::playWarningSound()
{
	mInternal->playSound("Windows XP Navigation.wav");
}

void AudioImpl::playErrorSound()
{
	mInternal->playSound("Windows XP Critical Stop.wav");
}

void AudioImpl::playScreenShotSound()
{
	mInternal->playSound("camera_shutter.wav");
}

void AudioImpl::playSampleSound()
{
	mInternal->playSound("Windows XP Information Bar.wav");
}

}//namespace cx
