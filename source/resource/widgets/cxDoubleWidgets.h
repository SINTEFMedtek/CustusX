/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
 * Insert a subclass of DoubDoubleDataAdapter order to connect to data.
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
	ScalarInteractionWidget(QWidget* parent, DoubleDataAdapterPtr);

	void enableLabel();
	void enableSlider();
	void enableDial();
	void enableEdit();
	void enableSpinBox();
	void enableInfiniteSlider();

	void addToOwnLayout();
	void addToGridLayout(QGridLayout* gridLayout = 0, int row = 0);
	void build(QGridLayout* gridLayout = 0, int row = 0);


private slots:
	void textEditedSlot();
	void doubleValueChanged(double val);
	void intValueChanged(int val); //expecting a value that is 100 times as big as the double value (because QDial uses ints, not double, we scale)
	void infiniteSliderMouseMoved(QPointF delta);

protected:
    virtual void prePaintEvent();
	DoubleDataAdapterPtr mData;

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
 * Insert a subclass of DoubDoubleDataAdapter order to connect to data.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SliderGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
};

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoubDoubleDataAdapter order to connect to data.
 *
 * Uses a QDoubleSpinBox instead of a slider - this gives a more compact widget.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
};

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoubDoubleDataAdapter in order to connect to data.
 *
 * Uses both a slider and a spin box
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxAndSliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxAndSliderGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
};
typedef boost::shared_ptr<SpinBoxAndSliderGroupWidget> SpinBoxAndSliderGroupWidgetPtr;

/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, dial>.
 * Insert a subclass of DoubDoubleDataAdapter in order to connect to data.
 *
 * Uses both a dial and a spin box
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxAndDialGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
SpinBoxAndDialGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
};
typedef boost::shared_ptr<SpinBoxAndDialGroupWidget> SpinBoxAndDialGroupWidgetPtr;


/**\brief Composite widget for scalar data manipulation.
 *
 * Consists of <namelabel, valueedit, slider>.
 * Insert a subclass of DoubDoubleDataAdapter order to connect to data.
 *
 * This slider is custom-made and infinitely long.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT SpinBoxInfiniteSliderGroupWidget: public ScalarInteractionWidget
{
Q_OBJECT
public:
	SpinBoxInfiniteSliderGroupWidget(QWidget* parent, DoubleDataAdapterPtr, QGridLayout* gridLayout = 0, int row = 0);
};

} //namespace cx


#endif /* CXDOUBLEWIDGETS_H_ */
