/*
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */

#ifndef SSCRECONSTRUCTIONWIDGET_H_
#define SSCRECONSTRUCTIONWIDGET_H_

#include <QtGui>
#include "sscReconstructer.h"

namespace ssc
{
class ReconstructionWidget : public QWidget
{
  Q_OBJECT
public:
  ReconstructionWidget(QWidget* parent);
  ReconstructerPtr reconstructer() {  return mReconstructer; }

public slots:
  void reconstruct();
  void selectData();
  void currentDataComboIndexChanged(const QString& text);

private:
    ReconstructerPtr mReconstructer;

    QString mInputFile;

    QComboBox* mDataComboBox;
    QToolButton* mSelectDataButton;
    QPushButton* mReconstructButton;
    QAction* mSelectDataAction;

    QString getCurrentPath();
    void updateComboBox();
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
