/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODCOMMANDLINESERVICE_H_
#define CXREGISTRATIONMETHODCOMMANDLINESERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_commandline_Export.h"

namespace cx
{

/**
 * Registration method: Commandline calls external program.
 * ElastiX is main target.
 *
 * \ingroup org_custusx_registration_method_commandline
 *
 * \date 2014-10-09
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_commandline_EXPORT RegistrationMethodCommandLineService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodCommandLineService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodCommandLineService() {}
	virtual QString getRegistrationType() {return QString("ImageToImage");}
	virtual QString getRegistrationMethod() {return QString("CommandLine");}
	virtual QString getWidgetName() {return QString("ElastiX");}
	virtual QWidget* createWidget();
};


} /* namespace cx */
#endif /* CXREGISTRATIONMETHODCOMMANDLINESERVICE_H_ */
