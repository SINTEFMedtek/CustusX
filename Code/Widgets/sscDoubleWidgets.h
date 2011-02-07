/*
 * sscDoubleWidgets.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */

#ifndef SSCDOUBLEWIDGETS_H_
#define SSCDOUBLEWIDGETS_H_

//#include <boost/shared_ptr.hpp>
#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>

#include "sscDoubleRange.h"
#include "sscDoubleDataAdapter.h"

namespace ssc
{
//typedef boost::shared_ptr<class DoubleDataAdapterDoDoubleDataAdapter


/**Custom widget for display of double-valued data
 * Use the double-named methods instead of qslider's int-based ones.
 */
class DoubleSlider : public QSlider
{
  Q_OBJECT

public:
  virtual ~DoubleSlider() {}
  DoubleSlider(QWidget* parent = 0) : QSlider(parent)
  {
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
    setDoubleRange(DoubleRange(0,1,0.1));
  }
  void setDoubleRange(const DoubleRange& range)
  {
    mRange = range;
    setRange(0, mRange.resolution());
    setSingleStep(mRange.step());
  }
  void setDoubleValue(double val)
  {
    int v_i = (val-mRange.min())/mRange.step();
    setValue(v_i);
  }
  double getDoubleValue() const
  {
    double v_d = mRange.step()*(double)value() + mRange.min();
    return v_d;
  }
  void setDoubleTickInterval(double interval)
  {
    setTickInterval(interval/mRange.step());
  }

signals:
  void doubleValueChanged(double value);

private slots:
  void valueChangedSlot(int val)
  {
    emit doubleValueChanged(getDoubleValue());
  }

private:
  DoubleRange mRange;
};

/**A QLineEdit specialized to deal with double data.
 */
class DoubleLineEdit : public QLineEdit
{
public:
  DoubleLineEdit(QWidget* parent = 0) : QLineEdit(parent) {}
  //virtual QSize sizeHint() const { return QSize(30,20); }
  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;
  double getDoubleValue(double defVal=0.0) const
  {
    bool ok;
    double newVal = this->text().toDouble(&ok);
    if (!ok)
      return defVal;
    return newVal;
  }
  void setDoubleValue(double val)
  {
    this->setText(QString::number(val, 'g', 4));
  }
};

/**Composite widget for scalar data manipulation.
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of ssc::DoubDoubleDataAdapter order to connect to data.
 */
class SliderGroupWidget : public QWidget
{
  Q_OBJECT
public:
  SliderGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void textEditedSlot();
  void doubleValueChanged(double val);

private:
  DoubleSlider* mSlider;
  QLabel* mLabel;
  DoubleLineEdit* mEdit;
  DoubleDataAdapterPtr mData;
};

/**Composite widget for scalar data manipulation.
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of ssc::DoubDoubleDataAdapter order to connect to data.
 *
 * Uses a QDoubleSpinBox instead of a slider - this gives a more compact widget.
 */
class SpinBoxGroupWidget : public QWidget
{
  Q_OBJECT
public:
  SpinBoxGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void doubleValueChanged(double val);

private:
  QDoubleSpinBox* mSpinBox;
  QLabel* mLabel;
  DoubleDataAdapterPtr mData;
};

} //namespace ssc



#endif /* SSCDOUBLEWIDGETS_H_ */
