/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXAIRWAYSFROMCENTERLINEFILTER_H
#define CXAIRWAYSFROMCENTERLINEFILTER_H

#include "org_custusx_filter_airwaysfromcenterline_Export.h"

#include "cxPatientModelService.h"
#include "cxFilterImpl.h"

class ctkPluginContext;

namespace cx
{

/** </p>
 * \ingroup org_custusx_filter_airwaysfromcenterline
 *
 * \date 2018-05-28
 * \author Erlend Fagertun Hofstad, SINTEF
 */


typedef boost::shared_ptr<class AirwaysFromCenterline> AirwaysFromCenterlinePtr;

class org_custusx_filter_airwaysfromcenterline_EXPORT AirwaysFromCenterlineFilter : public FilterImpl
{
	Q_OBJECT
	Q_INTERFACES(cx::Filter)

public:
        AirwaysFromCenterlineFilter(VisServicesPtr services);
        virtual ~AirwaysFromCenterlineFilter() {}

	virtual QString getType() const;
	virtual QString getName() const;
	virtual QString getHelp() const;
    static QString getNameSuffix();
    static QString getNameSuffixCenterline();


	virtual bool execute();
	virtual bool postProcess();

protected:
	virtual void createOptions();
	virtual void createInputTypes();
	virtual void createOutputTypes();

private slots:

private:
    AirwaysFromCenterlinePtr mAirwaysFromCenterline;
    vtkPolyDataPtr mOutputAirwayMesh;
    vtkPolyDataPtr mOutputSmoothedCenterline;
};
typedef boost::shared_ptr<class AirwaysFromCenterlineFilter> AirwaysFromCenterlineFilterPtr;


} // namespace cx



#endif // CXAIRWAYSFROMCENTERLINEFILTER_H
