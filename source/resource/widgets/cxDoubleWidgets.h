/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDOUBLEWIDGETS_H_
#define CXDOUBLEWIDGETS_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QDial>

#include "cxDoubleRange.h"
#include "cxDoublePropertyBase.h"
#include "cxOptimizedUpdateWidget.h"

namespace cx
{
class MousePadWidget;

/**\brief Custom widget for display of double-valued data.
 *
 * Use the double-named methods instead of qslider's int-based ones.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT DoubleSlider: public QSlider
{
Q_OBJECT

public:
	virtual ~DoubleSlider()
	{
	}
	DoubleSlider(QWidget* parent = 0) :
		QSlider(parent)
	{
		connect(this, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
		setDoubleRange(DoubleRange(0, 1, 0.1));
	}
	void setDoubleRange(const DoubleRange& range)
	{
		mRange = range;
		setRange(0, mRange.resolution());
		setSingleStep(mRange.step());
	}
	void setDoubleValue(double val)
	{
		int v_i = (val - mRange.min()) / mRange.step();
		setValue(v_i);
	}
	double getDoubleValue() const
	{
		double v_d = mRange.step() * (double) value() + mRange.min();
		return v_d;
	}
	void setDoubleTickInterval(double interval)
	{
		setTickInterval(interval / mRange.step());
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

/**\brief A QLineEdit specialized to deal with double data.
 *
 * \ingroup sscWidget
 */
class cxResourceWidgets_EXPORT DoubleLineEdit: public QLineEdit
{
public:
	DoubleLineEdit(QWidget* parent = 0) :
		QLineEdit(parent)
	{
	}
	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;
	double getDoubleValue(double defVal = 0.0) const
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

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoublePropertyBasePtr in order to connect to data.
 *
 * Before use: enable the widgets you need, then add either to Own layout, in case you wish to
 * use the widget normally, or add to grid, in case you wish to integrate the elements into a
 * larger grid.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT ScalarInteractionWidget: public OptimizedUpdateWidget
{
Q_OBJECT
public:
	ScalarInteractionWidget(QWidget* parent, DoublePropertyBasePtr);

	void enableLabel();
	void enableSlider();
	void enableDial();
	void enableEdit();
	void enableSpinBox();
	void enableInfiniteSlider();

	void addToOwnLayout();
	void addToGridLayout(QGridLayout* gridLayout = 0, int row = 0);
	void build(QGridLayout* gridLayout = 0, int row = 0);

	void showLabel(bool on);

private slots:
	void textEditedSlot();
	void doubleValueChanged(double val);
	void intValueChanged(int val); //expecting a value that is 100 times as big as the double value (because QDial uses ints, not double, we scale)
	void infiniteSliderMouseMoved(QPointF delta);

protected:
    virtual void prePaintEvent();
	DoublePropertyBasePtr mData;

private:
	void enableAll(bool);

	DoubleSlider* mSlider;
	QDial* mDial;
	QDoubleSpinBox* mSpinBox;
	QLabel* mLabel;
	DoubleLineEdit* mEdit;
	MousePadWidget* mInfiniteSlider;
};

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoublePropertyBasePtr in order to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SliderGroupWidget(QWidget* parent, DoublePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
};

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoublePropertyBasePtr in order to connect to data.
 *
 * Uses a QDoubleSpinBox instead of a slider - this gives a more compact widget.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxGroupWidget(QWidget* parent, DoublePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
};

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoublePropertyBasePtr in order to connect to data.
 *
 * Uses both a slider and a spin box
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxAndSliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxAndSliderGroupWidget(QWidget* parent, DoublePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
};
typedef boost::shared_ptr<SpinBoxAndSliderGroupWidget> SpinBoxAndSliderGroupWidgetPtr;

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, dial>.
 * Insert a subclass of DoublePropertyBasePtr in order to connect to data.
 *
 * Uses both a dial and a spin box
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxAndDialGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
SpinBoxAndDialGroupWidget(QWidget* parent, DoublePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
};
typedef boost::shared_ptr<SpinBoxAndDialGroupWidget> SpinBoxAndDialGroupWidgetPtr;


/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoublePropertyBasePtr order to connect to data.
 *
 * This slider is custom-made and infinitely long.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxInfiniteSliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxInfiniteSliderGroupWidget(QWidget* parent, DoublePropertyBasePtr, QGridLayout* gridLayout = 0, int row = 0);
};

} //namespace cx


#endif /* CXDOUBLEWIDGETS_H_ */
