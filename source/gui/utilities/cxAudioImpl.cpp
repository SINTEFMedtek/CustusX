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
