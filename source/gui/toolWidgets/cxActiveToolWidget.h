/*
 * cxActiveToolWidget.h
 *
 *  Created on: May 4, 2011
 *      Author: christiana
 */

#ifndef CXACTIVETOOLWIDGET_H_
#define CXACTIVETOOLWIDGET_H_

#include <QWidget>

namespace cx
{

/** Widget that contains a select active tool combo box.
 */
class ActiveToolWidget : public QWidget
{
  Q_OBJECT
public:
  ActiveToolWidget(QWidget* parent);
  virtual ~ActiveToolWidget() {}
};

}

#endif /* CXACTIVETOOLWIDGET_H_ */
