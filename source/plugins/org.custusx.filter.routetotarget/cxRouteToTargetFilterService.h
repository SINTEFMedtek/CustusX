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
#ifndef CXROUTETOTARGETFILTER_H
#define CXROUTETOTARGETFILTER_H

#include "org_custusx_filter_routetotarget_Export.h"

#include "cxPatientModelService.h"
#include "cxFilterImpl.h"

class ctkPluginContext;

namespace cx
{

/** Filter to calculates the route to a selected target in navigated bronchocopy.
 * The rout starts at the top of trachea and ends at the most adjacent airway centerline
 *  from the target.</p>
 * \ingroup cx_module_algorithm
 * \date Jan 29, 2015
 * \author Erlend Fagertun Hofstad
 */


typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;

class org_custusx_filter_routetotarget_EXPORT RouteToTargetFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
	RouteToTargetFilter(VisServicesPtr services);
	virtual ~RouteToTargetFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
    static QString getNameSuffix();
    static QString getNameSuffixExtension();

	virtual bool execute();
	virtual bool postProcess();
	virtual void setTargetName(QString name);

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
	RouteToTargetPtr mRouteToTarget;
	vtkPolyDataPtr mOutput;
    vtkPolyDataPtr mExtendedRoute;
	QString mTargetName;
};
typedef boost::shared_ptr<class RouteToTargetFilter> RouteToTargetFilterPtr;


} // namespace cx



#endif // CXROUTETOTARGETFILTER_H
