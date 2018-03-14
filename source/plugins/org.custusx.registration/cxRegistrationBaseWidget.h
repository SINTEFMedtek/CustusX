/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREGISTRATIONBASEWIDGET_H_
#define CXREGISTRATIONBASEWIDGET_H_

#include "cxBaseWidget.h"
#include "cxResourceWidgetsExport.h"

#include "cxRegistrationMethodService.h"

namespace cx
{

/**
 * \file
 * \addtogroup org_custusx_registration
 * @{
 */

class org_custusx_registration_EXPORT RegistrationBaseWidget : public BaseWidget
{
public:
	RegistrationBaseWidget(RegServicesPtr services, QWidget *parent, QString objectName, QString windowTitle);
  virtual ~RegistrationBaseWidget() {}

protected:
	RegServicesPtr mServices;
};

/**
 * @}
 */
}

#endif /* CXREGISTRATIONBASEWIDGET_H_ */
