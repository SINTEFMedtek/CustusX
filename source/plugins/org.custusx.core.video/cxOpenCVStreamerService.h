/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXOPENCVSTREAMERSERVICE_H
#define CXOPENCVSTREAMERSERVICE_H

#include "org_custusx_core_video_Export.h"
#include "cxStreamerService.h"
#include "cxStreamer.h"

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class Property> PropertyPtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;

/**
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT OpenCVStreamerService : public StreamerService
{
public:
	OpenCVStreamerService(ctkPluginContext *context) {}
	virtual ~OpenCVStreamerService() {}
	virtual QString getName();
	virtual QString getType() const;
	virtual void stop() {}
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);
private:
};

} //end namespace cx


#endif // CXOPENCVSTREAMERSERVICE_H
