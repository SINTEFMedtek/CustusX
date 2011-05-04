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


}
#endif /* CXTABBEDWIDGET_H_ */
