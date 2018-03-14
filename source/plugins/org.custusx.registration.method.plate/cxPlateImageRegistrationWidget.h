/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLATEIMAGEREGISTRATIONWIDGET_H
#define CXPLATEIMAGEREGISTRATIONWIDGET_H

#include "cxFastImageRegistrationWidget.h"

namespace cx
{

/**
 * Widget for performing plate (landmark based) image registration using only the
 * translation part of the matrix.
 *
 * \ingroup org_custusx_registration_method_plate
 *
 * \date 2014-10-09
 * \author Ole Vegard Solberg, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class PlateImageRegistrationWidget : public FastImageRegistrationWidget
{
//	Q_OBJECT
public:
	PlateImageRegistrationWidget(RegServicesPtr services, QWidget* parent);
	virtual ~PlateImageRegistrationWidget();

protected slots:
	virtual void editLandmarkButtonClickedSlot();

protected:
	virtual void performRegistration();
};

} //cx

#endif // CXPLATEIMAGEREGISTRATIONWIDGET_H
