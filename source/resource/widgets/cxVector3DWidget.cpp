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
#include <cxVector3DWidget.h>

#include <QTextEdit>
#include <QLayout>
#include <QLabel>
#include <QFontMetrics>

#include "cxTypeConversions.h"

#include "cxVector3DComponentDataAdapter.h"
#include "cxDoubleWidgets.h"

namespace cx
{

Vector3DWidget::Vector3DWidget(QWidget* parent, Vector3DDataAdapterPtr data) :
				QWidget(parent), mData(data)
{
	this->setToolTip(data->getHelp());
}

Vector3DWidget* Vector3DWidget::createSmallHorizontal(QWidget* parent, Vector3DDataAdapterPtr data)
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

Vector3DWidget* Vector3DWidget::createVerticalWithSliders(QWidget* parent, Vector3DDataAdapterPtr data)
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
	Vector3DComponentDataAdapterPtr component(new Vector3DComponentDataAdapter(mData, index, name, help));
	mWidgets[index] = new SpinBoxInfiniteSliderGroupWidget(this, component);
	layout->addWidget(mWidgets[index]);

	mDoubleAdapter[index] = component;
}

/** Create a single adapter for a component of the Vector3D, along with widgets.
 *
 */
void Vector3DWidget::addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout)
{
	Vector3DComponentDataAdapterPtr component(new Vector3DComponentDataAdapter(mData, index, name, help));

	ScalarInteractionWidget* widget = new ScalarInteractionWidget(this, component);
	widget->enableSpinBox();
	widget->build();
	mWidgets[index] = widget;
	layout->addWidget(mWidgets[index], 1);

	mDoubleAdapter[index] = component;
}

}
