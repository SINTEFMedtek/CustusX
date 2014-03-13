#ifndef CXAUDIO_H_
#define CXAUDIO_H_

#include "sscAudio.h"
#include <QObject>
#include <QString>

namespace cx {

/**Helper class for playing sounds in the main thread even if calls are made from other threads.
 * \ingroup cx_gui
 *
 */
class AudioInternal : public QObject
{
	Q_OBJECT
public:
	AudioInternal(QObject* parent=NULL);
	void playSound(QString file);
signals:
	void playSoundInternalSignal(QString file);
private slots:
	void playSoundSlot(QString file);
};


/**
 * \class Audio
 *
 * \brief Class for playing audio.
 *
 * \date Mar 4, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class AudioImpl : public Audio
{
public:
  AudioImpl();
  virtual ~AudioImpl();

  virtual void playStartSound();
  virtual void playStopSound();
  virtual void playCancelSound();

  virtual void playSuccessSound();
  virtual void playWarningSound();
  virtual void playErrorSound();

  virtual void playScreenShotSound();
  virtual void playSampleSound();

private:
  boost::shared_ptr<AudioInternal> mInternal;
};

}//namespace cx

#endif /* CXAUDIO_H_ */
