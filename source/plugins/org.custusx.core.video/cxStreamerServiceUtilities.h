/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTREAMERSERVICEUTILITIES_H_
#define CXSTREAMERSERVICEUTILITIES_H_

#include "org_custusx_core_video_Export.h"

#include <QString>
#include <ctkPluginContext.h>
#include "cxStreamerService.h"

namespace cx
{
/**
 * \brief 
 *
 * \date 21. aug. 2014
 * \author Janne Beate Bakeng, SINTEF
 */

class org_custusx_core_video_EXPORT StreamerServiceUtilities
{
public:
	static StreamerService* getStreamerServiceFromType(QString name, ctkPluginContext* context);
};

} /* namespace cx */

#endif /* CXSTREAMERSERVICEUTILITIES_H_ */
