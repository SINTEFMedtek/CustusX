/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODMANUALSERVICE_H_
#define CXREGISTRATIONMETHODMANUALSERVICE_H_

#include "cxRegistrationMethodService.h"

#include "org_custusx_registration_method_manual_Export.h"

namespace cx
{

/**
 * Registration method manual service interface.
 *
 * \ingroup org_custusx_registration_method_manual
 *
 * \date Sep 09, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_manual_EXPORT RegistrationMethodManualService : public RegistrationMethodService
{
	Q_OBJECT
public:
	RegistrationMethodManualService(RegServicesPtr services);
};

} /* namespace cx */

#endif /* CXREGISTRATIONMETHODMANUALSERVICE_H_ */

