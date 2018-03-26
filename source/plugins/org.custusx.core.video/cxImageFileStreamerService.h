/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXIMAGEFILESTREAMERSERVICE_H
#define CXIMAGEFILESTREAMERSERVICE_H

#include "org_custusx_core_video_Export.h"
#include "cxStreamerService.h"
class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class StringPropertyBase> StringPropertyBasePtr;
typedef boost::shared_ptr<class DoublePropertyBase> DoublePropertyBasePtr;
typedef boost::shared_ptr<class BoolPropertyBase> BoolPropertyBasePtr;


/**
 * \ingroup org_custusx_core_video
 *
 * \date 2014-11-21
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_video_EXPORT ImageFileStreamerService : public StreamerService
{
public:
	ImageFileStreamerService(ctkPluginContext *context) {}
	virtual ~ImageFileStreamerService() {}
	virtual QString getName();
	virtual QString getType() const;
	virtual void stop() {}
	virtual std::vector<PropertyPtr> getSettings(QDomElement root);
	virtual StreamerPtr createStreamer(QDomElement root);
private:
};

} //end namespace cx

#endif // CXIMAGEFILESTREAMERSERVICE_H
