/*
 * cxDoubleSpanSlider.h
 *
 *  \date Aug 26, 2010
 *      \author christiana
 */
#ifndef CXDOUBLESPANSLIDER_H_
#define CXDOUBLESPANSLIDER_H_

#include <QWidget>
#include "sscDoubleRange.h"
#include "qxtspanslider.h"
#include "sscDoubleWidgets.h"
class QDoubleSpinBox;

namespace cx
{

/**\brief Custom widget for display of double-valued data.
 * \ingroup cxGUI
 *
 * Use the double-named methods instead of qslider's int-based ones.
 */
class DoubleSpanSlider : public QxtSpanSlider
{
  Q_OBJECT

public:
  virtual ~DoubleSpanSlider() {}
  DoubleSpanSlider(QWidget* parent = 0) : QxtSpanSlider(parent)
  {
    connect(this, SIGNAL(spanChanged(int,int)), this, SLOT(spanChangedSlot(int,int)));
//    connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
    this->setDoubleRange(ssc::DoubleRange(0,1,0.1));
  }
  void setDoubleRange(const ssc::DoubleRange& range)
  {
    mRange = range;
    this->setRange(0, mRange.resolution());
    this->setSingleStep(mRange.step());
  }
  void setDoubleLowerValue(double val)
  {
    int v_i = (val-mRange.min())/mRange.step();
    this->setLowerValue(v_i);
  }
  double getDoubleLowerValue() const
  {
    double v_d = mRange.step()*(double)(this->lowerValue()) + mRange.min();
    return v_d;
  }
  void setDoubleUpperValue(double val)
  {
    int v_i = (val-mRange.min())/mRange.step();
    this->setUpperValue(v_i);
  }
  double getDoubleUpperValue() const
  {
    double v_d = mRange.step()*(double)(this->upperValue()) + mRange.min();
    return v_d;
  }
  void setDoubleTickInterval(double interval)
  {
    this->setTickInterval(interval/mRange.step());
  }

signals:
  void doubleSpanChanged(double lower, double upper);

private slots:
  void spanChangedSlot(int lower, int upper)
  {
    emit doubleSpanChanged(getDoubleLowerValue(), getDoubleUpperValue());
  }

private:
  ssc::DoubleRange mRange;
};

/**
 * \ingroup cxGUI
 *
 */
class SliderRangeGroupWidget : public QWidget
{
  Q_OBJECT
public:
  SliderRangeGroupWidget(QWidget* parent);

  void addToGridLayout(QGridLayout* gridLayout, int row);
  void setName(QString text);
  void setRange(const ssc::DoubleRange& range);
  void setDecimals(int decimals);
  void setValue(std::pair<double,double> val);
  std::pair<double,double> getValue() const;

signals:
   void valueChanged(double lower, double upper);

private slots:
  void doubleSpanChangedSlot(double lower, double upper);
  void textEditedSlot();

private:
  void dataChanged(std::pair<double,double> val);
  QLabel* mLabel;
//  ssc::DoubleLineEdit* mLowerEdit;
//  ssc::DoubleLineEdit* mUpperEdit;
  QDoubleSpinBox* mLowerEdit;
  QDoubleSpinBox* mUpperEdit;
  DoubleSpanSlider* mSpanSlider;
  std::pair<double,double> mValue;
};


} // namespace cx

#endif /* CXDOUBLESPANSLIDER_H_ */
