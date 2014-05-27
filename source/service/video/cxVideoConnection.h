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
#include "cxForwardDeclarations.h"
#include "cxVideoSource.h"
#include "cxProbeData.h"
#include "cxImageStreamerInterface.h"

class QTimer;
typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_video
 * @{
 */

typedef boost::shared_ptr<class ImageReceiverThread> ImageReceiverThreadPtr;
typedef boost::shared_ptr<class BasicVideoSource> BasicVideoSourcePtr;
typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;

/** \brief Represent one video grabber connection.
 *
 * Connect to a video/scanner interface and receive
 * all video and probe data from it. Convert to video
 * streams, set all data in probe if available.
 * Video Streams are also available directly from this
 * object.
 *
 * Refactored from old class OpenIGTLinkRTSource.
 *
 *  \ingroup cx_service_video
 *  \date Oct 31, 2010
 *  \date Feb 26, 2013
 *  \author Christian Askeland, SINTEF
 */
class VideoConnection : public QObject
{
	Q_OBJECT

public:
	explicit VideoConnection(VideoServiceBackendPtr backend);
	virtual ~VideoConnection();
	virtual bool isConnected() const;

	void runDirectLinkClient(std::map<QString, QString> args);
	void runIGTLinkedClient(QString address, int port);
	void disconnectServer();

	std::vector<VideoSourcePtr> getVideoSources();
	ImageStreamerInterfacePtr getStreamerInterface();


signals:
	bool connected(bool);
	void fps(QString source, int fps);
	void videoSourcesChanged();

private slots:
	void clientFinishedSlot();
	void imageReceivedSlot();
	void statusReceivedSlot();
	void fpsSlot(QString, double fps);
	void connectedSlot(bool on);
	void connectVideoToProbe();
	void useUnusedProbeDataSlot();///< If no probe is available the ProbeData is saved and this slot is called when a probe becomes available

private:
	void updateImage(ImagePtr message); // called by receiving thread when new data arrives.
	void runClient(ImageReceiverThreadPtr client);
	void stopClient(); ///< Get rid of the mClient thread.
	void resetProbe();///< Tell probe it is no longer connected to a digital interface
	void updateStatus(ProbeDefinitionPtr message);
	void startAllSources();
	void removeSourceFromProbe(ToolPtr tool);
	QString getImageToStream();

	ImageReceiverThreadPtr mClient;
	bool mConnected;
	double mFPS;
	std::vector<ProbeDefinitionPtr> mUnsusedProbeDataVector;

	std::vector<BasicVideoSourcePtr> mSources;

	VideoServiceBackendPtr mBackend;

	ImageStreamerInterfacePtr mStreamerInterface;
};
typedef boost::shared_ptr<VideoConnection> VideoConnectionPtr;

/**
 * @}
 */
} // namespace cx

#endif /* CXOPENIGTLINKRTSOURCE_H_ */
