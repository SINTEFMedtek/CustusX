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
#ifndef CXACCUSURFFILTER_H
#define CXACCUSURFFILTER_H

#include "org_custusx_filter_accusurf_Export.h"

#include "cxPatientModelService.h"
#include "cxFilterImpl.h"

class ctkPluginContext;

namespace cx
{

/** Filter to generate an ACCuSurf volume to be usen in navigated bronchoscopy.
 * The ACCuSurf is generated based on the standard thorax CT and a route-to-target centerline.
 * </p>
 * \ingroup cx_module_algorithm
 * \date Oct 26, 2016
 * \author Erlend Fagertun Hofstad
 */


typedef boost::shared_ptr<class Accusurf> AccusurfPtr;

class org_custusx_filter_accusurf_EXPORT AccusurfFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
	AccusurfFilter(VisServicesPtr services);
	virtual ~AccusurfFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;

	virtual bool execute();
    virtual bool postProcess();

    // extensions:
    DoublePropertyPtr getAccusurfThicknessUp(QDomElement root);
    DoublePropertyPtr getAccusurfThicknessDown(QDomElement root);


protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	AccusurfPtr mAccusurf;
	vtkPolyDataPtr mOutput;
    vtkImageDataPtr mAccusurfImage;
};
typedef boost::shared_ptr<class AccusurfFilter> AccusurfFilterPtr;


} // namespace cx



#endif // CXACCUSURFFILTER_H
