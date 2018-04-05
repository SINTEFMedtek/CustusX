/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
