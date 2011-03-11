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
  QSound::play(DataLocations::getAudioConfigFilePath()+"double_beep_1_(start).wav");
}

void Audio::playStopSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"beep_1_(stop).wav");
}

void Audio::playCancelSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"cancel_1.wav");
}

void Audio::playSuccessSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"success_1.wav");
}

void Audio::playWarningSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"warning_1.wav");
}

void Audio::playErrorSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"error_beep_1.wav");
}

void Audio::playScreenShotSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"camera_shutter.wav");
}

}//namespace cx
