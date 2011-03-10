#ifndef CXAUDIO_H_
#define CXAUDIO_H_

namespace cx {
/**
 * \class Audio
 *
 * \brief Class for playing audio.
 *
 * \date Mar 4, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class Audio
{
public:
  Audio();
  virtual ~Audio();

  static void playStartSound();
  static void playStopSound();
  static void playCancelSound();

  static void playSuccessSound();
  static void playWarningSound();
  static void playErrorSound();

  static void playScreenShotSound();
};

}//namespace cx

#endif /* CXAUDIO_H_ */
