/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXMANUALIMAGETRANSFORMREGISTRATIONWIDGET_H
#define CXMANUALIMAGETRANSFORMREGISTRATIONWIDGET_H

#include "cxManualRegistrationWidget.h"

namespace cx
{

/** Direct setting of image registration
 *
 * Manipulate the image matrix rMd via a matrix
 * or xyz+angles. The writing of the matrix is handled
 * exactly like an image registration.
 *
 *  \ingroup org_custusx_registration_method_manual
 *  \date Feb 16, 2012
 *  \author Christian Askeland, SINTEF
 *  \author Ole Vegard Solberg, SINTEF
 */
class ManualImageTransformRegistrationWidget : public ManualImageRegistrationWidget
{
public:
	ManualImageTransformRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName);
	virtual QString getDescription();
	virtual Transform3D getMatrixFromBackend();
	virtual void setMatrixFromWidget(Transform3D M);
};
} //cx
#endif // CXMANUALIMAGETRANSFORMREGISTRATIONWIDGET_H
