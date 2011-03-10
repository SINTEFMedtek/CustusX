#include <cxAudio.h>

#include <QSound>
#include "cxDataLocations.h"

#include "sscMessageManager.h"

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
  ssc::messageManager()->sendDebug("void Audio::playStartSound()");
}

void Audio::playStopSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"beep_1_(stop).wav");
  ssc::messageManager()->sendDebug("void Audio::playStopSound()");
}

void Audio::playCancelSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"cancel_1.wav");
  ssc::messageManager()->sendDebug("void Audio::playCancelSound()");
}

void Audio::playSuccessSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"success_1.wav");
  ssc::messageManager()->sendDebug("void Audio::playSuccessSound()");
}

void Audio::playWarningSound()
{
  //TODO
  //QSound::play(DataLocations::getAudioConfigFilePath()+"warning_1.wav");
  ssc::messageManager()->sendDebug("void Audio::playWarningSound()");
}

void Audio::playErrorSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"error_beep_1.wav");
  ssc::messageManager()->sendDebug("void Audio::playErrorSound()");
}

void Audio::playScreenShotSound()
{
  QSound::play(DataLocations::getAudioConfigFilePath()+"camera_shutter.wav");
  ssc::messageManager()->sendDebug("void Audio::playScreenShotSound()");
}

}//namespace cx
