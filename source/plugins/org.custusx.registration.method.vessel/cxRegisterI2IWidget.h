/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXREGISTERI2IWIDGET_H_
#define CXREGISTERI2IWIDGET_H_

#include <vector>
#include <QtWidgets>

#include "cxDoubleProperty.h"
#include "cxDoubleWidgets.h"
#include "cxRegistrationBaseWidget.h"

namespace cx
{
typedef boost::shared_ptr<class StringPropertySelectImage> StringPropertySelectImagePtr;
class SeansVesselRegistrationWidget;

/**
 * \class RegisterI2IWidget
 *
 * \brief Widget for performing the registration between two vessel segments.
 *
 * \ingroup org_custusx_registration_method_vessel
 * \date 13. okt. 2010
 * \author Janne Beate Bakeng
 */
class RegisterI2IWidget : public RegistrationBaseWidget
{
  Q_OBJECT

public:
	RegisterI2IWidget(RegServicesPtr services, QWidget* parent);
  ~RegisterI2IWidget();

private:
  RegisterI2IWidget();

  SeansVesselRegistrationWidget* mSeansVesselRegsitrationWidget;
};

}

#endif /* CXREGISTERI2IWIDGET_H_ */
