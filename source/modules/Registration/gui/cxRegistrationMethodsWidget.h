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
#ifndef CXREGISTRATIONMETHODSWIDGET_H_
#define CXREGISTRATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"
#include "cxRegistrationBaseWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_module_registration
 * @{
 */

class LandmarkRegistrationsWidget : public TabbedWidget
{
public:
  LandmarkRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~LandmarkRegistrationsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class FastRegistrationsWidget : public TabbedWidget
{
public:
  FastRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~FastRegistrationsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class Image2ImageRegistrationWidget : public TabbedWidget
{
public:
  Image2ImageRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~Image2ImageRegistrationWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class Image2PlateRegistrationWidget : public TabbedWidget
{
public:
  Image2PlateRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~Image2PlateRegistrationWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------

class ManualRegistrationsWidget : public TabbedWidget
{
public:
	ManualRegistrationsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~ManualRegistrationsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------

class RegistrationMethodsWidget : public TabbedWidget
{
	Q_OBJECT
public:
  RegistrationMethodsWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle);
  virtual ~RegistrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
private slots:
  void tabChangedSlot(int value);
};
//------------------------------------------------------------------------------

/**
 * @}
 */
}

#endif /* CXREGISTRATIONMETHODSWIDGET_H_ */
