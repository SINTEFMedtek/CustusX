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
#include "cxColorSelectButton.h"
#include <iostream>
#include <QtWidgets>
#include "cxSettings.h"

namespace cx
{

ColorSelectButton::ColorSelectButton(QString caption, QColor color, QWidget* parent) : QToolButton(parent)
{
	if (!caption.isEmpty())
		this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	this->setText(caption);
  connect(this, SIGNAL(clicked()), this, SLOT(clickedSlot()));
  this->setColor(color);
}

void ColorSelectButton::setColor(QColor color)
{
  mColor = color;

  QPixmap pixmap(16,16);
  pixmap.fill(mColor);
  this->setIcon(QIcon(pixmap));
}

void ColorSelectButton::clickedSlot()
{
	QColor result = QColorDialog::getColor( mColor, this, "Select color", QColorDialog::ShowAlphaChannel );
	if (!result.isValid())
		return;
	this->setColor(result);
	emit (colorChanged(mColor));
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectColorSettingButton::SelectColorSettingButton(QString caption, QString id, QString help) :
	ColorSelectButton(caption),
	mId(id)
{
	this->setColor(settings()->value(mId).value<QColor>());
	this->setStatusTip(help);
	this->setToolTip(help);
	connect(this, &SelectColorSettingButton::colorChanged,
			this, &SelectColorSettingButton::storeColor);
}

void SelectColorSettingButton::storeColor(QColor color)
{
	settings()->setValue(mId, color);
}

}
