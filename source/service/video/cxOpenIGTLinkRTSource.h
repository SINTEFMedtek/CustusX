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

/*
 * cxOpenIGTLinkRTSource.h
 *
 *  \date Oct 31, 2010
 *      \author christiana
 */
#ifndef CXOPENIGTLINKRTSOURCE_H_
#define CXOPENIGTLINKRTSOURCE_H_

#include "sscVideoSource.h"
#include <boost/array.hpp>
#include "igtlImageMessage.h"
#include <vector>
#include "sscProbeData.h"
#include "cxProbe.h"
#include "cxIGTLinkUSStatusMessage.h"
#include "cxIGTLinkImageMessage.h"
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

typedef boost::shared_ptr<class IGTLinkClientBase> IGTLinkClientBasePtr;

/**\brief Implementation of ssc::VideoSource for the OpenIGTLink protocol.
 * \ingroup cxServiceVideo
 *
 * Synchronize data with source,
 * provide data as a vtkImageData.
 */
class OpenIGTLinkRTSource: public ssc::VideoSource
{
Q_OBJECT
public:
	OpenIGTLinkRTSource();
	virtual ~OpenIGTLinkRTSource();
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
//  virtual void pause();
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
	void updateImage(IGTLinkImageMessage::Pointer message); // called by receiving thread when new data arrives.
	void updateSonixStatus(IGTLinkUSStatusMessage::Pointer message);
	void setEmptyImage();
	std::vector<unsigned char> mTestData;
	void setTestImage();
	vtkImageDataPtr createFilterARGB2RGB(vtkImageDataPtr input);
	vtkImageDataPtr createFilterBGR2RGB(vtkImageDataPtr input);//temporary hack
  	vtkImageDataPtr createFilterRGBA2RGB(vtkImageDataPtr input);
	void updateImageImportFromIGTMessage(IGTLinkImageMessage::Pointer message);
	void updateSonix();
	ProbePtr getValidProbe();
	void runClient(IGTLinkClientBasePtr client);
	void stopClient();

	boost::array<unsigned char, 100> mZero;
	vtkImageImportPtr mImageImport;
	vtkImageDataPtr mFilter_IGTLink_to_RGB;
	vtkImageAlgorithmPtr mRedirecter;
	IGTLinkImageMessage::Pointer mImageMessage;
	IGTLinkClientBasePtr mClient;
	bool mConnected;
	QString mDeviceName;
	bool mTimeout;
	QTimer* mTimeoutTimer;
	double mFPS;
	double mLastTimestamp;
	bool updateSonixParameters;
	ssc::ProbeData mSonixProbeData;
};
typedef boost::shared_ptr<OpenIGTLinkRTSource> OpenIGTLinkRTSourcePtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXOPENIGTLINKRTSOURCE_H_ */
