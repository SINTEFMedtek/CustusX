/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	SavingVideoRecorder(VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor, FileManagerServicePtr filemanagerservice);
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
