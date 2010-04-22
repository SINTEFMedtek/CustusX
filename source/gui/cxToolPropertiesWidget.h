/*
 * cxToolWidget.h
 *
 *  Created on: Apr 22, 2010
 *      Author: christiana
 */
#ifndef CXTOOLPROPERTIESWIDGET_H_
#define CXTOOLPROPERTIESWIDGET_H_

#include <vector>
#include <QtGUI>

namespace cx
{

class SliderGroupWidget;

/**
 * \class ToolPropertiesWidget
 *
 * \date 2010.04.22
 * \author: Christian Askeland, SINTEF
 */
class ToolPropertiesWidget : public QWidget
{
  Q_OBJECT

public:
  ToolPropertiesWidget(QWidget* parent);
  virtual ~ToolPropertiesWidget();

signals:

protected slots:
  void updateSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QCloseEvent* event); ///<disconnects stuff

private:
  ToolPropertiesWidget();

  SliderGroupWidget* mToolOffsetWidget;
  QLabel* mToolNameLabel;
};

}//end namespace cx


#endif /* CXTOOLPROPERTIESWIDGET_H_ */
