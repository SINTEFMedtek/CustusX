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
#ifndef CXSAVINGVIDEORECORDER_H
#define CXSAVINGVIDEORECORDER_H

#include "cxResourceExport.h"

#include <vector>
#include <QFile>
#include <QThread>
#include <QMutex>

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxData.h"

namespace cx
{
typedef boost::shared_ptr<class CachedImageDataContainer> CachedImageDataContainerPtr;

/** Class that saves vtkImageData continously to file.
  *
  * The data are saved as separate files in the saveSolder, using prefix
  * as a common filename component:
  *
  * A single file named \<prefix\>.fts containing N lines with timestamps
  * is written.
  * A sequence of N files named \<prefix\>_i.mhd (0<i<N) and corresponding .raw
  * files are written.
  *
  * If stop() is called, the thread will continue to write all remaining data,
  * then close files and return from run().
  *
  * Note: quit() will not work on this thread, use stop() instead.
  *
  * \date Dwc 2, 2012
  * \author christiana
  * \ingroup cx_resource_usreconstructiontypes
  */
class cxResource_EXPORT VideoRecorderSaveThread : public QThread
{
	Q_OBJECT
public:
	/**
	  * Create the thread object, set folder to save to.
	  */
	VideoRecorderSaveThread(QObject* parent, QString saveFolder, QString prefix, bool compressed, bool writeColor);
	virtual ~VideoRecorderSaveThread();
	/**
	  * Add data to be saved.
	  */
	QString addData(TimeInfo timestamp, vtkImageDataPtr data);
	void stop();
	void cancel();

protected:
	struct DataType
	{
		TimeInfo mTimestamp;
		QString mImageFilename;
		vtkImageDataPtr mImage;
	};
	QString mSaveFolder;
	QString mPrefix;
	int mImageIndex;
	std::list<DataType> mPendingData;
	QMutex mMutex; ///< protects the mPendingData
	bool mStop;
	bool mCancel;
	QFile mTimestampsFile;
	bool mCompressed;
	bool mWriteColor;
	/**
	  * Save the images to disk
	  */
	virtual void run();

	void writeQueue();
	bool openTimestampsFile();
	bool closeTimestampsFile();
	void write(DataType data);
	void writeTimeStampsFile(TimeInfo timeStamps);
};

/** \brief Recorder for a VideoSource.
 *
 * Record all frames from the input source and store them in an internal buffer.
 * Simultaneously save the data to disk continously using a VideoRecorderSaveThread.
 *
 * Replacement for the overly complicated class VideoRecorder
 *
  * \date Dwc 2, 2012
  * \author christiana
  * \ingroup cx_resource_usreconstructiontypes
 */
class cxResource_EXPORT SavingVideoRecorder : public QObject
{
	Q_OBJECT

public:
	SavingVideoRecorder(VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor);
	virtual ~SavingVideoRecorder();

	virtual void startRecord();
	virtual void stopRecord();
	void cancel();

	CachedImageDataContainerPtr getImageData();
	std::vector<TimeInfo> getTimestamps();
	QString getSaveFolder() { return mSaveFolder; }

	/** Call to force complete the writing of data to disk.
	  */
	void completeSave();

	VideoSourcePtr getSource() { return mSource; }

private slots:
	void newFrameSlot();
private:
	/**
	  * Delete all contents in folder created by this class
	  */
	void deleteFolder(QString folder);
	CachedImageDataContainerPtr mImages;
	std::vector<TimeInfo> mTimestamps;
	QString mSaveFolder;
	QString mPrefix;
	VideoSourcePtr mSource;
	boost::shared_ptr<VideoRecorderSaveThread> mSaveThread;

};

typedef boost::shared_ptr<SavingVideoRecorder> SavingVideoRecorderPtr;


} // namespace cx


#endif // CXSAVINGVIDEORECORDER_H
