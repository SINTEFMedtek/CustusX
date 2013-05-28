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

#ifndef CXDirectlyLinkedImageReceiverThread_H_
#define CXDirectlyLinkedImageReceiverThread_H_

#include <vector>
#include "boost/shared_ptr.hpp"

#include "cxImageReceiverThread.h"

namespace cx
{
typedef boost::shared_ptr<class Streamer> StreamerPtr;
typedef boost::shared_ptr<class DirectlyLinkedSender> DirectlyLinkedSenderPtr;

/**
 * \file
 * \addtogroup cxServiceVideo
 * @{
 */
typedef boost::shared_ptr<class SimulatedImageStreamer> SimulatedImageStreamerPtr;
typedef boost::shared_ptr<class DirectlyLinkedImageReceiverThread> DirectlyLinkedImageReceiverThreadPtr;

/**\brief Client thread used for running streamers
 *
 * \ingroup cxServiceVideo
 *
 *  \date Oct 11, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Janne Beate Bakeng, SINTEF
 */
class DirectlyLinkedImageReceiverThread: public ImageReceiverThread
{
Q_OBJECT
public:
	DirectlyLinkedImageReceiverThread(std::map<QString, QString> args, QObject* parent = NULL);
	virtual QString hostDescription() const; ///< threadsafe

protected:
	virtual void run();

private slots:
	void addImageToQueueSlot();
	void addSonixStatusToQueueSlot();

private:
	SimulatedImageStreamerPtr createSimulatedImageStreamer();
	void printArguments(); ///< for debugging

	std::map<QString, QString> mArguments;
	StreamerPtr mImageStreamer;
	DirectlyLinkedSenderPtr mSender;
};

/**
 * @}
 */
} //end namespace cx

#endif /* CXDirectlyLinkedImageReceiverThread_H_ */
