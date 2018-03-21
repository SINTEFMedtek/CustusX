/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGESTREAMEROPENCV_H_
#define CXIMAGESTREAMEROPENCV_H_

#include "cxGrabberExport.h"

#include <QObject> //needed for the mocer when OpenCv is not used...
#include "cxVideoServerConfig.h"

#include "boost/shared_ptr.hpp"
#include <QTcpSocket>
#include <QDateTime>
#include <QSize>
#include <QStringList>
#include "cxSender.h"
#include "cxStreamer.h"

class QDomElement;


class QTimer;

namespace cv
{
	class VideoCapture;
	class Mat;
}

namespace cx
{
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;

typedef boost::shared_ptr<cv::VideoCapture> VideoCapturePtr;

/**
 */
class cxGrabber_EXPORT ImageStreamerOpenCVArguments
{
public:
	QStringList getArgumentDescription();
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	StringMap convertToCommandLineArguments(QDomElement root);

	DoublePropertyBasePtr getVideoPortOption(QDomElement root);
	BoolPropertyBasePtr getPrintPropertiesOption(QDomElement root);
};

/**
 * An object sending images out on an ip port.
 * In order to operate within a nongui thread,
 * it must be created within the run() method
 * of a qthread.
 *
 * \ingroup cx_resource_videoserver
 * \author Christian Askeland, SINTEF
 * \date Jun 21, 2011
 *
 * This version uses openCV to grab images from a video file or camera
 */
class cxGrabber_EXPORT ImageStreamerOpenCV: public CommandLineStreamer
{
Q_OBJECT

public:
	ImageStreamerOpenCV();
	virtual ~ImageStreamerOpenCV();

	virtual void initialize(StringMap arguments);
	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();

	virtual QString getType();
	virtual QStringList getArgumentDescription();

private slots:
	void grab();
	void send();
	void continousGrabEvent();

private:
	void dumpProperties();
	ImagePtr getImageMessage();
	void dumpProperty(int val, QString name);

	void initialize_local();
	void deinitialize_local();
	vtkImageDataPtr convertTovtkImageData(cv::Mat& frame);

	QSize mRescaleSize;

	VideoCapturePtr mVideoCapture; // OpenCV video grabber
	QDateTime mLastGrabTime;
	bool mAvailableImage;
	bool mGrabbing;

};

}

#endif /* CXIMAGESTREAMEROPENCV_H_ */
