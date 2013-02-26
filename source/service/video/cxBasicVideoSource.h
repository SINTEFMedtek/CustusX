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
#ifndef CXBASICVIDEOSOURCE_H
#define CXBASICVIDEOSOURCE_H

#include "vtkSmartPointer.h"
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDateTime>
#include <boost/array.hpp>
class QTimer;

#include "sscVideoSource.h"
#include "sscForwardDeclarations.h"

typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageChangeInformation> vtkImageChangeInformationPtr;

namespace cx
{

/**
 * \brief VideoSource controlled by a vtkImageData
 *
 * Primitive implementation of the VideoSource interface.
 * Use the interface extensions to control the input:
 * Set a vtkImageData and
 *
 * \ingroup cxServiceVideo
 * \date April 26, 2013
 * \author Christian Askeland, SINTEF
 */
class BasicVideoSource : public ssc::VideoSource
{
	Q_OBJECT

public:
	BasicVideoSource();
	virtual ~BasicVideoSource();

	virtual QString getUid();
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();

	virtual QString getInfoString() const { return mInfo; }
	virtual QString getStatusString() const { return mStatus; }

	virtual void start();
	virtual void stop();

	virtual bool validData() const;
	virtual bool isConnected() const;
	virtual bool isStreaming() const;

	virtual void setResolution(double resolution);

	// extensions:
	void setInput(ssc::ImagePtr input);
	void setInfoString(QString text) { mInfo = text; }
	void setStatusString(QString text) { mStatus = text; }

private slots:
	void timeout();
private:
	bool mStreaming;
	QString mInfo;
	QString mStatus;

	ssc::ImagePtr mEmptyImage;
	ssc::ImagePtr mReceivedImage;
	vtkImageChangeInformationPtr mRedirecter;
	bool mTimeout;
	QTimer* mTimeoutTimer;
};

typedef boost::shared_ptr<BasicVideoSource> BasicVideoSourcePtr;

} // namespace cx

#endif // CXBASICVIDEOSOURCE_H
