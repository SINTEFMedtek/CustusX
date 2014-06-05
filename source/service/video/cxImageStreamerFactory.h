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
#ifndef CXIMAGESTREAMERFACTORY_H
#define CXIMAGESTREAMERFACTORY_H

#include <map>
#include <QString>
#include "cxStreamerService.h"

namespace cx
{

typedef boost::shared_ptr<class VideoServiceBackend> VideoServiceBackendPtr;
/**
 * \brief Create ImageStreamerInterfaces that can be used to create ImageStreamers (often in another thread)
 *
 * \ingroup cx_service_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class ImageStreamerFactory
{
public:
	ImageStreamerFactory();
	void setArguments(std::map<QString, QString> arguments);
	StreamerServicePtr getCommandlineStreamerInterface();

private:
	std::map<QString, QString> mArguments;
};

} //namespace cx

#endif // CXIMAGESTREAMERFACTORY_H
