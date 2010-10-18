/*
 * cxControlPanel.h
 *
 *  Created on: Oct 18, 2010
 *      Author: christiana
 */
#ifndef CXCONTROLPANEL_H_
#define CXCONTROLPANEL_H_

#include <QMainWindow>

namespace cx
{

class ControlPanel : public QMainWindow
{
  Q_OBJECT

public:
  ControlPanel(QWidget* parent);
  ~ControlPanel();
private:
  void addAsDockWidget(QWidget* widget);
};

} // namespace cx

#endif /* CXCONTROLPANEL_H_ */
