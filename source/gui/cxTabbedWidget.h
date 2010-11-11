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
  TabbedWidget(QString objectName, QString windowTitle, QWidget* parent);
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
  RegistrationMethodsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~RegistrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class SegmentationMethodsWidget : public TabbedWidget
{
public:
  SegmentationMethodsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~SegmentationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class VisualizationMethodsWidget : public TabbedWidget
{
public:
  VisualizationMethodsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~VisualizationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class CalibrationMethodsWidget : public TabbedWidget
{
public:
  CalibrationMethodsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~CalibrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class LandmarkRegistrationsWidget : public TabbedWidget
{
public:
  LandmarkRegistrationsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~LandmarkRegistrationsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class FastRegistrationsWidget : public TabbedWidget
{
public:
  FastRegistrationsWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~FastRegistrationsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class Image2ImageRegistrationWidget : public TabbedWidget
{
public:
  Image2ImageRegistrationWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~Image2ImageRegistrationWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
class Image2PlateRegistrationWidget : public TabbedWidget
{
public:
  Image2PlateRegistrationWidget(QString objectName, QString windowTitle, QWidget* parent);
  virtual ~Image2PlateRegistrationWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------
}
#endif /* CXTABBEDWIDGET_H_ */
