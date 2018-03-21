/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTABBEDWIDGET_H_
#define CXTABBEDWIDGET_H_

#include "cxResourceWidgetsExport.h"

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
 * \ingroup cx_resource_widgets
 * \date 27. sep. 2010
 * \\author Janne Beate Bakeng
 */

class cxResourceWidgets_EXPORT TabbedWidget : public BaseWidget
{
	Q_OBJECT
public:
  TabbedWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~TabbedWidget();

  void addTab(BaseWidget* newTab, QString newTabName);
  void insertWidgetAtTop(QWidget* newWidget);
  void setDefaultWidget(QWidget* widget);

private slots:
  void tabChanged(const int &index);

protected:
  QTabWidget* mTabWidget;
  QVBoxLayout* mLayout;

private:
  TabbedWidget();
};
//------------------------------------------------------------------------------


}
#endif /* CXTABBEDWIDGET_H_ */
