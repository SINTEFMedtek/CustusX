/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	FastImageRegistrationWidget(RegServices services, QWidget* parent, QString objectName, QString windowTitle, bool useRegistrationFixedPropertyInsteadOfActiveImage = false);
	virtual ~FastImageRegistrationWidget();

protected:
	virtual void performRegistration();
};

/**
 * @}
 */
}//namespace cx

#endif /* CXFASTIMAGEREGISTRATIONWIDGET_H_ */
