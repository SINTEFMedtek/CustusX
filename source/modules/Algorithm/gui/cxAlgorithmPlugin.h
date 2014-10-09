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

#ifndef CXALGORITHMPLUGIN_H_
#define CXALGORITHMPLUGIN_H_

#include "cxPluginAlgorithmExport.h"

#include "cxGUIExtenderService.h"

namespace cx
{

/**
 * \defgroup cx_module_algorithm Algorithm Plugin
 * \ingroup cx_modules
 * \brief Algorithm collection with widgets.
 *
 * See \ref cx::AlgorithmPlugin.
 *
 */

/**
 * \file
 * \addtogroup cx_module_algorithm
 * @{
 */

/**
 * \class AlgorithmPlugin
 *
 * \date Jun 15, 2011
 * \author christiana
 */
class cxPluginAlgorithm_EXPORT AlgorithmPlugin : public GUIExtenderService
{
	Q_OBJECT
public:
	AlgorithmPlugin(VisualizationServicePtr visualizationService, PatientModelServicePtr patientModelService);
	virtual ~AlgorithmPlugin();

	virtual std::vector<CategorizedWidget> createWidgets() const;
private:
	PatientModelServicePtr mPatientModelService;
	VisualizationServicePtr mVisualizationService;
};
typedef boost::shared_ptr<class AlgorithmPlugin> AlgorithmPluginPtr;

/**
 * @}
 */
}

#endif /* CXALGORITHMPLUGIN_H_ */
