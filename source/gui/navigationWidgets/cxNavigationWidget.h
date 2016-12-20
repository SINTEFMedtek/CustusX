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
