/*
 * cxSecondaryMainWindow.h
 *
 *  Created on: Oct 18, 2010
 *      Author: christiana
 */
#ifndef CXSECONDARYMAINWINDOW_H_
#define CXSECONDARYMAINWINDOW_H_

#include <QMainWindow>

namespace cx
{

class SecondaryMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  SecondaryMainWindow(QWidget* parent);
  ~SecondaryMainWindow();
private:
  void addAsDockWidget(QWidget* widget, bool visible);
};

} // namespace cx

#endif /* CXSECONDARYMAINWINDOW_H_ */
