/*
 * cxRegistrationMethodsWidget.h
 *
 *  \date May 2, 2011
 *      \author christiana
 */

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
