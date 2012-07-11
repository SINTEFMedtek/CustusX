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
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef SSCVIDEOSOURCE_H_
#define SSCVIDEOSOURCE_H_

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace ssc
{

/**\brief video source interface.
 *
 * Synchronize data with source,
 * provide data as a vtkImageData.
 *
 *
 * \ingroup sscVideo
 */
class VideoSource : public QObject
{
	Q_OBJECT

public:
	virtual ~VideoSource() {}

	virtual QString getUid() = 0;
	virtual QString getName() = 0;
	virtual vtkImageDataPtr getVtkImageData() = 0;
	virtual double getTimestamp() = 0;

	virtual QString getInfoString() const = 0; ///< an information text intended to be visible along with the data stream at all times
	virtual QString getStatusString() const = 0; ///< status text describing the stream state, display instead of stream when the stream is invalid.

	virtual void start() = 0; ///< start streaming
	virtual void stop()  = 0; ///< stop streaming

	virtual bool validData() const = 0; ///< return true is data stream is ok to display. This is a heuristic based on the data rate.
	virtual bool isConnected() const = 0; ///< return true when a connection to the data source is established.
	virtual bool isStreaming() const = 0; ///< return true when the source is streaming data.

	/**
	 * Set the resolution of the incoming video in mm/pixel
	 */
	virtual void setResolution(double mmPerPixel) {}

signals:
	void streaming(bool on); ///< emitted when streaming started/stopped
	void connected(bool on); ///< emitted when source is connected/disconnected
	void newFrame();          ///< emitted when a new frame has arrived (getVtkImageData() returns something new). info/status/name/valid might also have changed
};
typedef boost::shared_ptr<VideoSource> VideoSourcePtr;

} // namespace ssc

#endif /* SSCVIDEOSOURCE_H_ */
