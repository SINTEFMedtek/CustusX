/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXCALIBRATIONMETHODSWIDGET_H_
#define CXCALIBRATIONMETHODSWIDGET_H_

#include "org_custusx_calibration_Export.h"

#include "cxTabbedWidget.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;

/**
 * \file
 * \addtogroup org_custusx_calibration
 * @{
 */

class org_custusx_calibration_EXPORT CalibrationMethodsWidget : public TabbedWidget
{
public:
  CalibrationMethodsWidget(VisServicesPtr services, AcquisitionServicePtr acquisitionService,
  		QWidget* parent, QString objectName, QString windowTitle);
  virtual ~CalibrationMethodsWidget() {}
};
//------------------------------------------------------------------------------

/**
 * @}
 */
}

#endif /* CXCALIBRATIONMETHODSWIDGET_H_ */
