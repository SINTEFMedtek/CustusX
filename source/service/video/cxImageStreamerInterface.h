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
#ifndef CXIMAGESTREAMERINTERFACE_H
#define CXIMAGESTREAMERINTERFACE_H

#include "boost/shared_ptr.hpp"

namespace cx
{

typedef boost::shared_ptr<class ImageStreamerInterface> ImageStreamerInterfacePtr;
typedef boost::shared_ptr<class Streamer> StreamerPtr;

/**
 * \brief Abstract class. Interface to ImageStreamers
 *
 * \ingroup cx_service_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class ImageStreamerInterface
{
public:
	ImageStreamerInterface();
	virtual StreamerPtr createStreamer() = 0;
};

} //end namespace cx
#endif // CXIMAGESTREAMERINTERFACE_H
