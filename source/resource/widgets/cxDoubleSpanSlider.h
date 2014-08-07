/*
 * cxDoubleSpanSlider.h
 *
 *  \date Aug 26, 2010
 *      \author christiana
 */
#ifndef CXDOUBLESPANSLIDER_H_
#define CXDOUBLESPANSLIDER_H_

#include <QWidget>
#include "cxDoubleRange.h"
#include "qxtspanslider.h"
#include "cxDoubleWidgets.h"
#include "cxDoublePairDataAdapter.h"
#include "cxOptimizedUpdateWidget.h"
class QDoubleSpinBox;

namespace cx
{

/**\brief Custom widget for display of double-valued data.
 * \ingroup cx_resource_widgets
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
    this->setDoubleRange(DoubleRange(0,1,0.1));
  }
  void setDoubleRange(const DoubleRange& range)
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
  DoubleRange mRange;
};

/**
 * \ingroup cx_guisource_widgets
 *
 */
class SliderRangeGroupWidget : public OptimizedUpdateWidget
{
  Q_OBJECT
public:
	SliderRangeGroupWidget(QWidget* parent, DoublePairDataAdapterPtr dataInterface, QGridLayout* gridLayout = 0, int row = 0);

	void addToGridLayout(QGridLayout* gridLayout, int row);
  void setRange(const DoubleRange& range);
	void setDecimals(int decimals);
  std::pair<double,double> getValue() const;
	bool setValue(double lower, double upper);

signals:
   void valueChanged(double lower, double upper);

private slots:
  void doubleSpanChangedSlot(double lower, double upper);
  void textEditedSlot();
	void dataChanged();

private:
	SliderRangeGroupWidget();
	void init(QGridLayout *gridLayout, int row);
	void updateGuiRange();
	QLabel* mLabel;
  QDoubleSpinBox* mLowerEdit;
  QDoubleSpinBox* mUpperEdit;
	DoubleSpanSlider* mSpanSlider;

	DoublePairDataAdapterPtr mData;

};


} // namespace cx

#endif /* CXDOUBLESPANSLIDER_H_ */
