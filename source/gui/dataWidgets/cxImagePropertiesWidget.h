#ifndef CXIMAGEPROPERTIESWIDGET_H_
#define CXIMAGEPROPERTIESWIDGET_H_

#include <vector>
#include <QtGui>
#include "cxTabbedWidget.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxGUI
 * @{
 */

/**
 * \class ImagePropertiesWidget
 *
 * \date 2010.04.12
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class ImagePropertiesWidget : public TabbedWidget
{
  Q_OBJECT

public:
  ImagePropertiesWidget(QWidget* parent);
  virtual ~ImagePropertiesWidget();
  virtual QString defaultWhatsThis() const;

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ImagePropertiesWidget();
};

/**
 * @}
 */
}//end namespace cx

#endif /* CXIMAGEPROPERTIESWIDGET_H_ */
