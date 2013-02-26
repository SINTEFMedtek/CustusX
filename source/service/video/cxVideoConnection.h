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
#ifndef CXOPENIGTLINKRTSOURCE_H_
#define CXOPENIGTLINKRTSOURCE_H_

#include <vector>
#include <map>
#include <boost/array.hpp>
#include "sscForwardDeclarations.h"
#include "sscVideoSource.h"
#include "sscProbeData.h"

class QTimer;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;

namespace cx
{
/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */

typedef boost::shared_ptr<class GrabberReceiveThread> GrabberReceiveThreadPtr;

/** \brief Implementation of ssc::VideoSource for the OpenIGTLink protocol.
 *  \ingroup cxServiceVideo
 *
 * Synchronize data with source,
 * provide data as a vtkImageData.
 *
 *  \date Oct 31, 2010
 *  \author christiana
 */
class GrabberVideoSource: public ssc::VideoSource
{
Q_OBJECT
public:
	GrabberVideoSource();
	virtual ~GrabberVideoSource();
	virtual QString getUid()
	{
		return "us_openigtlink_source";
	}
	virtual QString getName();
	virtual vtkImageDataPtr getVtkImageData();
	virtual double getTimestamp();
	virtual bool isConnected() const;

	virtual QString getInfoString() const;
	virtual QString getStatusString() const;

	virtual void start();
	virtual void stop();

	virtual bool validData() const;
	virtual bool isStreaming() const;
	virtual void release() {}

	// non-inherited methods
	void directLink(std::map<QString, QString> args);
	void connectServer(QString address, int port);
	void disconnectServer();

signals:
	void fps(int fps);
private slots:
	void clientFinishedSlot();
	void imageReceivedSlot();
	void sonixStatusReceivedSlot();
	void timeout();
	void fpsSlot(double fps);
	void connectedSlot(bool on);

private:
	void updateImage(ssc::ImagePtr message); // called by receiving thread when new data arrives.
	void updateSonixStatus(ssc::ProbeData message);
	void runClient(GrabberReceiveThreadPtr client);
	void stopClient();

	ssc::ImagePtr mEmptyImage;
	ssc::ImagePtr mReceivedImage;
	vtkImageAlgorithmPtr mRedirecter;
	GrabberReceiveThreadPtr mClient;
	bool mConnected;
	bool mTimeout;
	QTimer* mTimeoutTimer;
	double mFPS;
};
typedef boost::shared_ptr<GrabberVideoSource> GrabberVideoSourcePtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXOPENIGTLINKRTSOURCE_H_ */
