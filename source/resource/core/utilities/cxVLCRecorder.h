/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVLCPROCESS_H_
#define CXVLCPROCESS_H_

#include "cxResourceExport.h"

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

class cxResource_EXPORT VLCRecorder: public QObject
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

signals:
	void stateChanged();

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

cxResource_EXPORT VLCRecorder* vlc(); ///< Shortcut for accessing the vlc recorder

/**
 * @}
 */

} /* namespace cx */

#endif /* CXVLCPROCESS_H_ */
