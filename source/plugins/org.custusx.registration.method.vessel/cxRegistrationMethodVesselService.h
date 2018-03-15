/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXREGISTRATIONMETHODVESSELSERVICE_H_
#define CXREGISTRATIONMETHODVESSELSERVICE_H_

#include "cxRegistrationMethodService.h"
#include "org_custusx_registration_method_vessel_Export.h"

namespace cx
{

/**
 * Registration method: Vessel image to image service implementation
 *
 * \ingroup org_custusx_registration_method_vessel
 *
 * \date 2014-10-01
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_registration_method_vessel_EXPORT RegistrationMethodVesselImageToImageService : public RegistrationMethodService
{
	Q_INTERFACES(cx::RegistrationMethodService)
public:
	RegistrationMethodVesselImageToImageService(RegServicesPtr services) :
		RegistrationMethodService(services) {}
	virtual ~RegistrationMethodVesselImageToImageService() {}
	virtual QString getRegistrationType() {return QString("ImageToImage");}
	virtual QString getRegistrationMethod() {return QString("Vessel");}
	virtual QString getWidgetName() {return QString("org_custusx_registration_method_vessel_widget");}
	virtual QWidget* createWidget();
};


} /* namespace cx */

#endif /* CXREGISTRATIONMETHODVESSELSERVICE_H_ */

