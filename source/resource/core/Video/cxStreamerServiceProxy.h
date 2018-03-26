/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTREAMERINTERFACEPROXY_H
#define CXSTREAMERINTERFACEPROXY_H

#include "cxStreamerService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{
/**
 * \brief Proxy for StreamerServices.
 *
 * \ingroup cx_resource_core_video
 *
 * \date June 02, 2015
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT StreamerServiceProxy : public StreamerService
{
public:

    StreamerServiceProxy(ctkPluginContext *context, QString name);
    virtual ~StreamerServiceProxy() {}
    virtual QString getName();
    virtual QString getType() const;
		virtual void stop();

    virtual std::vector<PropertyPtr> getSettings(QDomElement root);
    virtual StreamerPtr createStreamer(QDomElement root);
private:
    void initServiceListener();
    void onServiceAdded(StreamerService *service);
    void onServiceRemoved(StreamerService *service);

    ctkPluginContext *mPluginContext;
    QString mServiceName;
    StreamerServicePtr mStreamerService;
    boost::shared_ptr<ServiceTrackerListener<StreamerService> > mServiceListener;
};

} //end namespace cx

#endif // CXSTREAMERINTERFACEPROXY_H
