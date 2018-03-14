/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXDOUBLESPANSLIDER_H_
#define CXDOUBLESPANSLIDER_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include "cxDoubleRange.h"
#include "qxtspanslider.h"
#include "cxDoubleWidgets.h"
#include "cxDoublePairPropertyBase.h"
#include "cxOptimizedUpdateWidget.h"
class QDoubleSpinBox;

namespace cx
{

/**\brief Custom widget for display of double-valued data.
 * \ingroup cx_resource_widgets
 *
 * Use the double-named methods instead of qslider's int-based ones.
 */
class cxResourceWidgets_EXPORT DoubleSpanSlider : public QxtSpanSlider
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
  DoubleRange getDoubleRange() const
  {
	return mRange;
  }
  void setDoubleSpan(double lower, double upper)
  {
	int lower_i = (lower-mRange.min())/mRange.step();
	int upper_i = (upper-mRange.min())/mRange.step();
	this->setSpan(lower_i, upper_i);
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
class cxResourceWidgets_EXPORT SliderRangeGroupWidget : public OptimizedUpdateWidget
{
  Q_OBJECT
public:
	SliderRangeGroupWidget(QWidget* parent, DoublePairPropertyBasePtr dataInterface, QGridLayout* gridLayout = 0, int row = 0);

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

	DoublePairPropertyBasePtr mData;
};


} // namespace cx

#endif /* CXDOUBLESPANSLIDER_H_ */
