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
//#include "sscAbstractInterface.h"
#include "sscDoubleWidgets.h"
#include "sscXmlOptionItem.h"
#include "sscLabeledComboBoxWidget.h"

namespace ssc
{


class ReconstructionWidget : public QWidget
{
  Q_OBJECT
public:
  ReconstructionWidget(QWidget* parent, ReconstructerPtr reconstructer);
  ReconstructerPtr reconstructer() {  return mReconstructer; }
  void selectData(QString inputfile);

public slots:
  void reconstruct();
  void selectData();
  void reload();
  void currentDataComboIndexChanged(const QString& text);
  void paramsChangedSlot();
private slots:
  void inputDataSelected(QString mhdFileName);

private:
  ReconstructerPtr mReconstructer;
  
  QComboBox* mDataComboBox;
  QToolButton* mSelectDataButton;
  QPushButton* mReconstructButton;
  QPushButton* mReloadButton;
  QAction* mSelectDataAction;
  QLineEdit* mExtentLineEdit;
  QLineEdit* mInputSpacingLineEdit;
  ssc::SpinBoxGroupWidget* mMaxVolSizeWidget;
  ssc::SpinBoxGroupWidget* mSpacingWidget;
  ssc::SpinBoxGroupWidget* mDimXWidget;
  ssc::SpinBoxGroupWidget* mDimYWidget;
  ssc::SpinBoxGroupWidget* mDimZWidget;
//  ssc::CheckBoxWidget* mAlignTimestampsWidget;
//  ssc::SpinBoxGroupWidget* mTimeCalibrationWidget;
  
  QGroupBox* mAlgorithmGroup;
  QGridLayout* mAlgoLayout;
  std::vector<QWidget*> mAlgoWidgets;
  
  //    ssc::StringDataAdapterPtr generateStringDataAdapter(QString uid);
  QString getCurrentPath();
  void updateComboBox();
  void setInputFile(const QString& inputFile);
  void repopulateAlgorithmGroup();
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
