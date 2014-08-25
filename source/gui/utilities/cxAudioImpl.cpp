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

#include "cxAudioImpl.h"

#include <QSound>
#include "cxDataLocations.h"

namespace cx
{

AudioInternal::AudioInternal(QObject* parent) : QObject(parent)
{
	connect(this, SIGNAL(playSoundInternalSignal(QString)), this, SLOT(playSoundSlot(QString)));
}

void AudioInternal::playSound(QString file)
{
	emit playSoundInternalSignal(file);
}

void AudioInternal::playSoundSlot(QString file)
{
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
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Insert.wav");
}

void AudioImpl::playStopSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Remove.wav");
}

void AudioImpl::playCancelSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Fail.wav");
}

void AudioImpl::playSuccessSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Print complete.wav");
}

void AudioImpl::playWarningSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows xp Navigation.wav");
}

void AudioImpl::playErrorSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Critical Stop.wav");
}

void AudioImpl::playScreenShotSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"camera_shutter.wav");
}

void AudioImpl::playSampleSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Information Bar.wav");
}

}//namespace cx
