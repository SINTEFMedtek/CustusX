/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLUGINFRAMEWORKUTILITIES_H_
#define CXPLUGINFRAMEWORKUTILITIES_H_

#include "cxLogicManagerExport.h"

#include <ctkPlugin.h>

namespace cx
{


cxLogicManager_EXPORT QString getStringForctkPluginState(const ctkPlugin::State state);
cxLogicManager_EXPORT ctkPlugin::State getctkPluginStateForString(QString text);

} /* namespace cx */
#endif /* CXPLUGINFRAMEWORKUTILITIES_H_ */
