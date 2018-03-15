/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXAUDIOIMPL_H_
#define CXAUDIOIMPL_H_

#include "cxGuiExport.h"

#include "cxAudio.h"
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMutex>

namespace cx {

/**Helper class for playing sounds in the main thread even if calls are made from other threads.
 *
 * Play requests too close to the last play is ignored. mMinTimeBetweenEachSound limits this.
 *
 * \ingroup cx_gui
 *
 */
class cxGui_EXPORT AudioInternal : public QObject
{
	Q_OBJECT
public:
	AudioInternal(QObject* parent=NULL);
	void playSound(QString file);
signals:
	void playSoundInternalSignal(QString file);
private slots:
	void playSoundSlot(QString file);
private:
	QMutex mLastPlayTimeMutex;
	int mMinTimeBetweenEachSound;
	QDateTime mLastPlayTime;
	bool checkValidTime();
};


/**
 * \class Audio
 *
 * \brief Class for playing audio.
 *
 * \date Mar 4, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT AudioImpl : public Audio
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

#endif /* CXAUDIOIMPL_H_ */
