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
#ifndef CXSIMULATEDIMAGESTREAMERINTERFACE_H
#define CXSIMULATEDIMAGESTREAMERINTERFACE_H

#include "cxImageStreamerInterface.h"

#include "cxVideoServiceBackend.h"
#include "cxSimulatedImageStreamer.h"

namespace cx
{

typedef boost::shared_ptr<class SimulatedImageStreamerInterface> SimulatedImageStreamerInterfacePtr;

/**
 * \brief Interface to image streamer simulator
 *
 * \ingroup cx_service_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class SimulatedImageStreamerInterface : public ImageStreamerInterface
{
public:
	SimulatedImageStreamerInterface();

	void setBackend(VideoServiceBackendPtr backend);
	void setImageToStream(QString imageUid);
	void setGain(double gain);

	virtual StreamerPtr createStreamer();

private:
	VideoServiceBackendPtr mBackend;
	QString mImageUidToSimulate;
	SimulatedImageStreamerPtr mStreamer;//Access must be mutexed, as it can be accessed from several threads
};

} //end namespace cx
#endif // CXSIMULATEDIMAGESTREAMERINTERFACE_H
