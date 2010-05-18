/*
 *  sscMainWindow.h
 */

#ifndef USTEST_MAINWINDOW_H_
#define USTEST_MAINWINDOW_H_

#include <QtGui>
#include "sscReconstructer.h"

#include <QtGui>
#include <vtkRenderer.h>
#include "sscReconstructionWidget.h"
#include "sscView.h"
#include "sscVolumetricRep.h"
#include "sscImage.h"
#include "sscMainWindow.h"


/**Main win for us reconstruction test app
 *
 */
class MainWindow : public QWidget
{
  Q_OBJECT
public:
  MainWindow(QWidget* parent=0);
  void automaticStart();

private slots:
  void showData();

private:
  ssc::ReconstructionWidget* mReconstructionWidget;
  ssc::View* mView;
};

#endif //USTEST_MAINWINDOW_H_
