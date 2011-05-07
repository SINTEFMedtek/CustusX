/*
 * cxSegmentationMethodsWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXSEGMENTATIONMETHODSWIDGET_H_
#define CXSEGMENTATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

namespace cx
{

class SegmentationMethodsWidget : public TabbedWidget
{
public:
  SegmentationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~SegmentationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------


}

#endif /* CXSEGMENTATIONMETHODSWIDGET_H_ */
