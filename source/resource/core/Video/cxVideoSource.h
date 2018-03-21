/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



/*
 *  Created on: Oct 31, 2010
 *      Author: christiana
 */

#ifndef CXVIDEOSOURCE_H_
#define CXVIDEOSOURCE_H_

#include "cxResourceExport.h"

#include "vtkSmartPointer.h"
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include "cxData.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;

namespace cx
{

/** \brief video source interface.
 *
 * Synchronize data with source,
 * provide data as a vtkImageData.
 *
 *
 * \ingroup cx_resource_core_video
 */
class cxResource_EXPORT VideoSource : public QObject
{
	Q_OBJECT

public:
	virtual ~VideoSource() {}

	virtual QString getUid() = 0;
	virtual QString getName() = 0;
	virtual vtkImageDataPtr getVtkImageData() = 0;
	virtual double getTimestamp() = 0;
	virtual TimeInfo getAdvancedTimeInfo() = 0;

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

} // namespace cx

#endif /* CXVIDEOSOURCE_H_ */
