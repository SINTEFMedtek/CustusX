/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include <cxVector3DWidget.h>

#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QFontMetrics>

#include "cxTypeConversions.h"

#include "cxVector3DComponentProperty.h"
#include "cxDoubleWidgets.h"

namespace cx
{

Vector3DWidget::Vector3DWidget(QWidget* parent, Vector3DPropertyBasePtr data) :
				QWidget(parent), mData(data)
{
	this->setToolTip(data->getHelp());
}

Vector3DWidget* Vector3DWidget::createSmallHorizontal(QWidget* parent, Vector3DPropertyBasePtr data)
{
	Vector3DWidget* retval = new Vector3DWidget(parent, data);

	QHBoxLayout* aLayout = new QHBoxLayout(retval);
	aLayout->setMargin(0);
	aLayout->addWidget(new QLabel(data->getDisplayName(), retval), 0);
	retval->addSmallControlsForIndex("x", "X", 0, aLayout);
	retval->addSmallControlsForIndex("y", "Y", 1, aLayout);
	retval->addSmallControlsForIndex("z", "Z", 2, aLayout);

	return retval;
}

Vector3DWidget* Vector3DWidget::createVerticalWithSliders(QWidget* parent, Vector3DPropertyBasePtr data)
{
	Vector3DWidget* retval = new Vector3DWidget(parent, data);

	QVBoxLayout* aLayout = new QVBoxLayout(retval);
	aLayout->setMargin(0);
	retval->addSliderControlsForIndex("x", "X", 0, aLayout);
	retval->addSliderControlsForIndex("y", "Y", 1, aLayout);
	retval->addSliderControlsForIndex("z", "Z", 2, aLayout);

	return retval;
}

void Vector3DWidget::showDim(int dim, bool visible)
{
	mWidgets[dim]->setVisible(visible);
}

/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addSliderControlsForIndex(QString name, QString help, int index, QBoxLayout* layout)
{
	Vector3DComponentPropertyPtr component(new Vector3DComponentProperty(mData, index, name, help));
	mWidgets[index] = new SpinBoxInfiniteSliderGroupWidget(this, component);
	layout->addWidget(mWidgets[index]);

	mDoubleAdapter[index] = component;
}

/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout)
{
	Vector3DComponentPropertyPtr component(new Vector3DComponentProperty(mData, index, name, help));

	ScalarInteractionWidget* widget = new ScalarInteractionWidget(this, component);
	widget->enableSpinBox();
	widget->build();
	mWidgets[index] = widget;
	layout->addWidget(mWidgets[index], 1);

	mDoubleAdapter[index] = component;
}

}
