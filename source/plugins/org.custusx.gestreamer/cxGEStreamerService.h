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

#ifndef CXGESTREAMERSERVICE_H_
#define CXGESTREAMERSERVICE_H_

#include "cxStreamerService.h"
#include "org_custusx_gestreamer_Export.h"

namespace cx
{

/**
 * Implementation of streamer service.
 *
 * \ingroup org_custusx_gestreamer
 *
 * \date 2014-08-07
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_gestreamer_EXPORT GEStreamerService : public StreamerService
{
	Q_INTERFACES(cx::StreamerService)
public:
	GEStreamerService();
	virtual ~GEStreamerService();

	virtual QString getName();
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);

};
typedef boost::shared_ptr<GEStreamerService> GEStreamerServicePtr;

} /* namespace cx */

#endif /* CXGESTREAMERSERVICE_H_ */

