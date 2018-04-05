/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODMANUALIMAGETOIMAGESERVICE_H
#define CXREGISTRATIONMETHODMANUALIMAGETOIMAGESERVICE_H

#include "cxRegistrationMethodManualService.h"

namespace cx
{
/**
 * Implementation of registration method manual service: Image to image
 *
 * \ingroup org_custusx_registration_method_manual
 *
 * \date Sep 08, 2014
 * \author Ole Vegard Solberg, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */
class org_custusx_registration_method_manual_EXPORT RegistrationMethodManualImageToImageService : public RegistrationMethodManualService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodManualImageToImageService(RegServicesPtr services);
	virtual ~RegistrationMethodManualImageToImageService() {}
	virtual QString getRegistrationType() {return QString("ImageToImage");}
	virtual QString getRegistrationMethod() {return QString("Manual");}
	virtual QWidget* createWidget();
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_manual_image2image");}
};
} //cx
#endif // CXREGISTRATIONMETHODMANUALIMAGETOIMAGESERVICE_H
