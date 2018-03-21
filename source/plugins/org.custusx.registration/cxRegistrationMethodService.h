/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODSERVICE_H
#define CXREGISTRATIONMETHODSERVICE_H

#include <QObject>

#include <boost/shared_ptr.hpp>
#include "cxRegServices.h"
#include "org_custusx_registration_Export.h"

#define RegistrationMethodService_iid "cx::RegistrationMethodService"

namespace cx
{


/** \brief Registration Method services
 *
 * This class defines the common interface towards the registration methods plugins.
 *
 *  \ingroup cx_resource_core_registration
 *  \date 2014-09-02
 *  \author Ole Vegard Solberg, SINTEF
 *  \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_EXPORT RegistrationMethodService : public QObject
{
	Q_OBJECT
public:
	RegistrationMethodService(RegServicesPtr services) :
	mServices(services) {}
    virtual ~RegistrationMethodService() {}

	virtual QWidget* createWidget() = 0;
	virtual QString getWidgetName() = 0;
	virtual QString getRegistrationType() = 0;
	virtual QString getRegistrationMethod() = 0;

protected:
	RegServicesPtr mServices;
};

} //namespace cx
Q_DECLARE_INTERFACE(cx::RegistrationMethodService, RegistrationMethodService_iid)


#endif // CXREGISTRATIONMETHODSERVICE_H
