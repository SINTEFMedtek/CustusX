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

#ifndef CXAUDIOIMPL_H_
#define CXAUDIOIMPL_H_

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

#endif /* CXAUDIOIMPL_H_ */
