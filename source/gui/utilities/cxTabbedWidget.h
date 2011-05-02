#ifndef CXTABBEDWIDGET_H_
#define CXTABBEDWIDGET_H_

#include <QString>
#include "cxBaseWidget.h"

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

class TabbedWidget : public BaseWidget
{
public:
  TabbedWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~TabbedWidget();

  void addTab(BaseWidget* newTab, QString newTabName);
  virtual QString defaultWhatsThis() const;

protected:
  QTabWidget* mTabWidget;

private:
  TabbedWidget();
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
}
#endif /* CXTABBEDWIDGET_H_ */
