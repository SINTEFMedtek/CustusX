#include <cxAudio.h>

#include <QSound>
#include "cxDataLocations.h"

namespace cx
{

Audio::Audio()
{
}

Audio::~Audio()
{}

void Audio::playStartSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Insert.wav");
}

void Audio::playStopSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Remove.wav");
}

void Audio::playCancelSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Hardware Fail.wav");
}

void Audio::playSuccessSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Print complete.wav");
}

void Audio::playWarningSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows xp Navigation.wav");
}

void Audio::playErrorSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"Vista_Sound_Pack/Windows XP Critical Stop.wav");
}

void Audio::playScreenShotSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"camera_shutter.wav");
}

}//namespace cx
