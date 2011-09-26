#include <cxAudio.h>

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


Audio::Audio()
{
	mInternal.reset(new AudioInternal());
}

Audio::~Audio()
{}

void Audio::playStartSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Insert.wav");
}

void Audio::playStopSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Remove.wav");
}

void Audio::playCancelSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Fail.wav");
}

void Audio::playSuccessSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Print complete.wav");
}

void Audio::playWarningSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows xp Navigation.wav");
}

void Audio::playErrorSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Critical Stop.wav");
}

void Audio::playScreenShotSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"camera_shutter.wav");
}

void Audio::playSampleSound()
{
	mInternal->playSound(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Information Bar.wav");
}

}//namespace cx
