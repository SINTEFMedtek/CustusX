/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscVideoRecorder.h
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#ifndef CXVIDEORECORDER_H_
#define CXVIDEORECORDER_H_

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "cxVideoSource.h"
#include <map>

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{

/** \brief Recorder for a VideoSource.
 *
 * The class has an autosync feature that is useful if the realtimestream clock
 * is unsynced with the master clock. The first received frame is used to estimate
 * a shift between the master and source clocks. This shift is then added to the input
 * times in getRecording in order to retrieve data using the source clock.
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoRecorder : public QObject
{
	Q_OBJECT
public:
	typedef std::map<double, vtkImageDataPtr> DataType; ///<  <timestamp, frame>
public:
	VideoRecorder(VideoSourcePtr source, bool sync = true);
	virtual ~VideoRecorder();

	virtual void startRecord();
	virtual void stopRecord();
	virtual DataType getRecording(double start, double stop);

	private slots:
	void newFrameSlot();
private:
	DataType mData;
	VideoSourcePtr mSource;

	bool mSynced;
	double mSyncShift;
};

typedef boost::shared_ptr<VideoRecorder> VideoRecorderPtr;

}

#endif /* CXVIDEORECORDER_H_ */
