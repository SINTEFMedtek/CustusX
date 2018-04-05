/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXFASTIMAGEREGISTRATIONWIDGET_H_
#define CXFASTIMAGEREGISTRATIONWIDGET_H_

#include "org_custusx_registration_method_landmark_Export.h"

#include "cxImageLandmarksWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_landmark
 * @{
 */

/**
 * \class FastImageRegistrationWidget
 *
 * \brief Widget for performing landmark based image registration using only the
 * translation part of the matrix.
 *
 * \date 27. sep. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_registration_method_landmark_EXPORT FastImageRegistrationWidget : public ImageLandmarksWidget
{
	Q_OBJECT
public:
	FastImageRegistrationWidget(RegServicesPtr services, QWidget* parent, QString objectName, QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage = false);
	virtual ~FastImageRegistrationWidget();

protected:
	virtual void performRegistration();
};

/**
 * @}
 */
}//namespace cx

#endif /* CXFASTIMAGEREGISTRATIONWIDGET_H_ */
