/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
