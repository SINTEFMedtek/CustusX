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
#include "sscAbstractInterface.h"
#include "sscHelperWidgets.h"

namespace ssc
{

/** Interface to the tool offset of the dominant tool
 */
class DoubleDataInterfaceMaxUSVolumeSize : public DoubleDataInterface
{
  Q_OBJECT
public:
  DoubleDataInterfaceMaxUSVolumeSize(ReconstructerPtr reconstructer);
  virtual ~DoubleDataInterfaceMaxUSVolumeSize() {}
  virtual QString getValueName() const { return "Max Volume Size (Mb)"; }
  virtual double convertInternal2Display(double internal) { return internal/mFactor; } ///< conversion from internal value to display value
  virtual double convertDisplay2Internal(double display) { return display*mFactor; } ///< conversion from internal value to display value
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual QString getValueID() const { return ""; }
  virtual void connectValueSignals(bool on) {}
  DoubleRange getValueRange() const;

private slots:
protected:
  const int mFactor;
  ReconstructerPtr mReconstructer;
};


class ReconstructionWidget : public QWidget
{
  Q_OBJECT
public:
  ReconstructionWidget(QWidget* parent);
  ReconstructerPtr reconstructer() {  return mReconstructer; }
  void selectData(QString inputfile);

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
    QLineEdit* mExtentLineEdit;
    ssc::SliderGroupWidget* mMaxVolSizeWidget;

    QString getCurrentPath();
    void updateComboBox();
    void setInputFile(const QString& inputFile);
};

}//namespace
#endif //SSCRECONSTRUCTIONWIDGET_H_
