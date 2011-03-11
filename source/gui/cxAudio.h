#ifndef CXAUDIO_H_
#define CXAUDIO_H_

#include "sscAudio.h"

namespace cx {
/**
 * \class Audio
 *
 * \brief Class for playing audio.
 *
 * \date Mar 4, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class Audio : public ssc::Audio
{
public:
  Audio();
  virtual ~Audio();

  virtual void playStartSound();
  virtual void playStopSound();
  virtual void playCancelSound();

  virtual void playSuccessSound();
  virtual void playWarningSound();
  virtual void playErrorSound();

  virtual void playScreenShotSound();
};

}//namespace cx

#endif /* CXAUDIO_H_ */
