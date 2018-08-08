/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXSTREAMERINTERFACENULL_H
#define CXSTREAMERINTERFACENULL_H

#include "cxStreamerService.h"

namespace cx
{

/**
 * \brief Null implementation of the StreamerService.
 *
 * \ingroup cx_resource_core_video
 *
 * \date June 02, 2015
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT StreamerServiceNull : public StreamerService
{
public:
    StreamerServiceNull() {}
    virtual ~StreamerServiceNull(){}
    virtual QString getName();
    virtual QString getType() const;
		virtual void stop();

    virtual std::vector<PropertyPtr> getSettings(QDomElement root);
    virtual StreamerPtr createStreamer(QDomElement root);
};

} //end namespace cx

#endif // CXSTREAMERINTERFACENULL_H
