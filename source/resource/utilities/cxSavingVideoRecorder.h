// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXSAVINGVIDEORECORDER_H
#define CXSAVINGVIDEORECORDER_H

#include <QFile>
#include <QThread>
#include <QMutex>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <utility>
#include "boost/shared_ptr.hpp"
#include "sscVideoRecorder.h"
#include "sscTool.h"
#include "sscUSFrameData.h"
#include "cxImageDataContainer.h"

namespace cx
{

/** Class that saves vtkImageData continously to file.
  *
  * The data are saved as separate files in the saveSolder, using prefix
  * as a common filename component:
  *
  * A single file named <prefix>.fts containing N lines with timestamps
  * is written.
  * A sequence of N files named <prefix>_i.mhd (0<i<N) and corresponding .raw
  * files are written.
  *
  * If stop() is called, the thread will continue to write all remaining data,
  * then close files and return from run().
  *
  * Note: quit() will not work on this thread, use stop() instead.
  *
  * \date Dwc 2, 2012
  * \author christiana
  * \ingroup cxResourceUtilities
  */
class VideoRecorderSaveThread : public QThread
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
	QString addData(double timestamp, vtkImageDataPtr data);
	void stop();
	void cancel();

signals:
//	void dataSaved(QString filename); ///< emitted for each saved vtkImageData

protected:
	struct DataType
	{
		double mTimestamp;
		QString mImageFilename;
		vtkImageDataPtr mImage;
	};
	QString mSaveFolder;
	QString mPrefix;
	int mImageIndex;
//	typedef std::pair<double, vtkImageDataPtr> DataType; ///< timestamp + image data
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
};

/** \brief Recorder for a VideoSource.
 *
 * Record all frames from the input source and store them in an internal buffer.
 * Simultaneously save the data to disk continously using a VideoRecorderSaveThread.
 *
 * Replacement for the overly complicated class ssc::VideoRecorder
 *
  * \date Dwc 2, 2012
  * \author christiana
  * \ingroup cxResourceUtilities
 */
class SavingVideoRecorder : public QObject
{
	Q_OBJECT
public:
//	typedef std::map<double, vtkImageDataPtr> DataType; ///<  <timestamp, frame>
public:
	SavingVideoRecorder(ssc::VideoSourcePtr source, QString saveFolder, QString prefix, bool compressed, bool writeColor);
	virtual ~SavingVideoRecorder();

	virtual void startRecord();
	virtual void stopRecord();
	void cancel();

//	virtual DataType getRecording();
	CachedImageDataContainerPtr getImageData();
//	std::vector<QString> getImageData();
	std::vector<double> getTimestamps();

	/** Call to force complete the writing of data to disk.
	  */
	void completeSave();

	/**
	  * Delete all contents in folder created by this class
	  */
	static void deleteFolder(QString folder);

private slots:
	void newFrameSlot();
//	void dataSavedSlot(QString filename);
private:
//	std::vector<CachedImageDataPtr> mImages;
//	std::vector<QString> mImages;
	CachedImageDataContainerPtr mImages;
	std::vector<double> mTimestamps;
	/** Use to remove data in memory when the recording is large
	  */
//	void purgeCache();
//	int mLastPurgedImageIndex;
	QString mSaveFolder;
	ssc::VideoSourcePtr mSource;
	boost::shared_ptr<VideoRecorderSaveThread> mSaveThread;

};

typedef boost::shared_ptr<SavingVideoRecorder> SavingVideoRecorderPtr;


} // namespace cx


#endif // CXSAVINGVIDEORECORDER_H
