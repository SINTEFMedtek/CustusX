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

#include "cxStreamerService.h"

#include "cxSimulatedImageStreamer.h"

#include "org_custusx_ussimulator_Export.h"

namespace cx
{

typedef boost::shared_ptr<class SimulatedImageStreamerService> SimulatedImageStreamerServicePtr;

/**
 * \brief Interface to image streamer simulator
 *
 * \ingroup org_custusx_ussimulator
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_ussimulator_EXPORT SimulatedImageStreamerService : public StreamerService
{
	Q_INTERFACES(cx::StreamerService)
public:
	SimulatedImageStreamerService();

	void setImageToStream(QString imageUid);
	void setGain(double gain);

	virtual StreamerPtr createStreamer();
	virtual QWidget* createWidget();
	virtual QString getName();

private:
//	VideoServiceBackendPtr mBackend;
	QString mImageUidToSimulate;
	SimulatedImageStreamerPtr mStreamer;//Access must be mutexed, as it can be accessed from several threads
};

} //end namespace cx
#endif // CXSIMULATEDIMAGESTREAMERINTERFACE_H
