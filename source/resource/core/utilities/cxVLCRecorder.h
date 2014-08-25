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

#ifndef CXVLCPROCESS_H_
#define CXVLCPROCESS_H_

#include <QString>
#include "cxProcessWrapper.h"

namespace cx
{

/**
 * \file
 * \addtogroup cx_resource_core_utilities
 * @{
 */

typedef boost::shared_ptr<class VLCRecorder> VLCRecorderPtr;

/**
 * \brief Lets you use the third party application VLC
 * to record a video of the screen.
 *
 * Is implemented as a singleton because there should only be
 * one instance of the application VLC running at all times.
 *
 * \date Oct 10, 2013
 * \author Janne Beate Bakeng, SINTEF
 */

class VLCRecorder: public QObject
{
	Q_OBJECT

public:
	static VLCRecorder* getInstance();
	  static void initialize();
	  static void shutdown();

	bool hasVLCApplication();
	void findVLCApplication(QStringList suggestedVLCLocations = QStringList());
	QString getVLCPath();

	bool isRecording();

	//for running without a event loop - used for testing
	bool waitForStarted(int msecs = 30000);
	bool waitForFinished(int msecs = 30000);

public slots:
	void startRecording(QString saveFile);
	void stopRecording();

private:
	VLCRecorder();
	virtual ~VLCRecorder();
	VLCRecorder(const VLCRecorder&);
	VLCRecorder& operator=(const VLCRecorder&);

	void setVLCPath(QString path);
	bool isValidVLC(QString vlcPath);
	QString getVLCDefaultLocation();
	QString getVLCDefaultRecorderArguments(QString saveFile);

	ProcessWrapperPtr mCommandLine;
	QString mVLCPath;

	static VLCRecorder* mTheInstance;
};

VLCRecorder* vlc(); ///< Shortcut for accessing the vlc recorder

/**
 * @}
 */

} /* namespace cx */

#endif /* CXVLCPROCESS_H_ */
