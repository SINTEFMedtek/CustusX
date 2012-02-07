/*
 * cxSegmentationMethodsWidget.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXSEGMENTATIONMETHODSWIDGET_H_
#define CXSEGMENTATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

class SegmentationMethodsWidget : public TabbedWidget
{
public:
  SegmentationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~SegmentationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------


/**
 * @}
 */
}

#endif /* CXSEGMENTATIONMETHODSWIDGET_H_ */
