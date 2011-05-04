/*
 * cxCalibrationMethodsWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXCALIBRATIONMETHODSWIDGET_H_
#define CXCALIBRATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

namespace cx
{

class CalibrationMethodsWidget : public TabbedWidget
{
public:
  CalibrationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~CalibrationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------

}

#endif /* CXCALIBRATIONMETHODSWIDGET_H_ */
