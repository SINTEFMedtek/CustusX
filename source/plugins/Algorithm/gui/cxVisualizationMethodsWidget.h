/*
 * cxVisualizationMethodsWidget.h
 *
 *  \date May 4, 2011
 *      \author christiana
 */

#ifndef CXVISUALIZATIONMETHODSWIDGET_H_
#define CXVISUALIZATIONMETHODSWIDGET_H_

#include "cxTabbedWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
 * @{
 */

class VisualizationMethodsWidget : public TabbedWidget
{
public:
  VisualizationMethodsWidget(QWidget* parent, QString objectName, QString windowTitle);
  virtual ~VisualizationMethodsWidget(){};
  virtual QString defaultWhatsThis() const;
};
//------------------------------------------------------------------------------


/**
 * @}
 */
}

#endif /* CXVISUALIZATIONMETHODSWIDGET_H_ */
