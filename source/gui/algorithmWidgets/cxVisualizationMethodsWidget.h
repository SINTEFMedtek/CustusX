/*
 * cxVisualizationMethodsWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXVISUALIZATIONMETHODSWIDGET_H_
#define CXVISUALIZATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

namespace cx
{

class VisualizationMethodsWidget : public TabbedWidget
{
public:
  VisualizationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~VisualizationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------


}

#endif /* CXVISUALIZATIONMETHODSWIDGET_H_ */
