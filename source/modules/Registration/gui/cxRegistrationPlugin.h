/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXREGISTRATIONPLUGIN_H_
#define CXREGISTRATIONPLUGIN_H_

#include "cxPluginRegistrationExport.h"

#include "cxGUIExtenderService.h"
#include <ctkPluginContext.h>

/**
 * \defgroup cx_module_registration Registration Plugin
 * \ingroup cx_modules
 * \brief Registration collection with widgets.
 *
 * See \ref cx::RegistrationPlugin.
 *
 */

namespace cx
{
typedef boost::shared_ptr<class RegistrationPlugin> RegistrationPluginPtr;
typedef boost::shared_ptr<class RegistrationManager> RegistrationManagerPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;

/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

/**
 *
 *  \date Jun 16, 2011
 *  \author christiana
 */
class cxPluginRegistration_EXPORT RegistrationPlugin : public GUIExtenderService
{
	Q_OBJECT
public:
	RegistrationPlugin(AcquisitionDataPtr acquisitionData, ctkPluginContext *pluginContext);
	virtual ~RegistrationPlugin() {}

	virtual std::vector<CategorizedWidget> createWidgets() const;
signals:

private slots:

private:
	RegistrationManagerPtr mRegistrationManager;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONPLUGIN_H_ */
