/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXNAVIGATIONWIDGET_H_
#define CXNAVIGATIONWIDGET_H_

#include "cxGuiExport.h"

#include "cxBaseWidget.h"

class QGroupBox;
class QRadioButton;
class QVBoxLayout;
class QLabel;
class QSlider;

namespace cx
{

/**
 * \class NavigationWidget
 *
 * \brief Widget for controlling camera follow style.
 * \ingroup cx_gui
 *
 * \date 22. feb. 2010
 * \\author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT NavigationWidget : public BaseWidget
{
  Q_OBJECT

public:
  NavigationWidget(ViewServicePtr viewService, TrackingServicePtr trackingService, QWidget* parent);
  ~NavigationWidget();

protected:
  QVBoxLayout*  mVerticalLayout; ///< vertical layout is used

  //camera settings
  QGroupBox*    mCameraGroupBox; ///< widget to put all camera specific settings
  QVBoxLayout*  mCameraGroupLayout; ///< layout for the cameragroupbox
};
}
#endif /* CXNAVIGATIONWIDGET_H_ */
