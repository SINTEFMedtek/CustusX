// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscVideoRecorder.h
 *
 *  Created on: Dec 17, 2010
 *      Author: christiana
 */

#ifndef SSCVIDEORECORDER_H_
#define SSCVIDEORECORDER_H_

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "sscVideoSource.h"
#include <map>

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/**\brief Recorder for a VideoSource.
 *
 * The class has an autosync feature that is useful if the realtimestream clock
 * is unsynced with the master clock. The first received frame is used to estimate
 * a shift between the master and source clocks. This shift is then added to the input
 * times in getRecording in order to retrieve data using the source clock.
 *
 * \ingroup sscVideo
 */
class VideoRecorder : public QObject
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

#endif /* SSCVIDEORECORDER_H_ */
