#ifndef SSCHELPERWIDGETS_H_
#define SSCHELPERWIDGETS_H_

#include <QtGui>
#include "sscAbstractInterface.h"

namespace ssc
{

/** A widget that floats above the other windows. Used as basis for the floating controls. */
class sscFloatingWidget : public QWidget
{
	Q_OBJECT

public:
	virtual ~sscFloatingWidget() {}
	sscFloatingWidget(QWidget* parent = 0) : QWidget(parent)
	{
		Qt::WindowFlags flags = Qt::FramelessWindowHint;
		flags |= Qt::Window;
		flags |= Qt::WindowStaysOnTopHint;
		this->setWindowFlags(flags);
	}
};

/**Custom widget for display of double-valued data
 * Use the double-named methods instead of qslider's int-based ones.
 */
class ssc DoubleSlider : public QSlider
{
	Q_OBJECT

public:
	virtual ~sscDoubleSlider() {}
	sscDoubleSlider(QWidget* parent = 0) : QSlider(parent)
	{
		connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
		setDoubleRange(DoubleRange(0,1,1000));
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

}

#endif
