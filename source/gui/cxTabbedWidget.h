#ifndef CXTABBEDWIDGET_H_
#define CXTABBEDWIDGET_H_

#include <QString>
#include "cxWhatsThisWidget.h"

class QTabWidget;

namespace cx
{
/**
 * \class TabbedWidget
 *
 * \brief Interface for making widgets with a hierarchy of tabs
 *
 * \date 27. sep. 2010
 * \author: Janne Beate Bakeng
 */

class TabbedWidget : public WhatsThisWidget
{
public:
  TabbedWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~TabbedWidget();

  void addTab(WhatsThisWidget* newTab, QString newTabName);
  virtual QString defaultWhatsThis() const;

protected:
  QTabWidget* mTabWidget;

private:
  TabbedWidget();
};
//------------------------------------------------------------------------------
class RegistrationMethodsWidget : public TabbedWidget
{
public:
  RegistrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~RegistrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class SegmentationMethodsWidget : public TabbedWidget
{
public:
  SegmentationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~SegmentationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class VisualizationMethodsWidget : public TabbedWidget
{
public:
  VisualizationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~VisualizationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class CalibrationMethodsWidget : public TabbedWidget
{
public:
  CalibrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~CalibrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
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
}
#endif /* CXTABBEDWIDGET_H_ */
