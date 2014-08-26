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

#include "cxSelectionGroupBox.h"

#include <QComboBox>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCheckBox>

namespace cx
{
SelectionGroupBox::SelectionGroupBox(QString title, QStringList selectionList, Qt::Orientation orientation, bool exclusive, QWidget* parent) :
    QGroupBox(parent),
    mSelectionList(selectionList),
    mButtonGroup(new QButtonGroup(parent))
{
	if (orientation==Qt::Vertical)
		mLayout = new QVBoxLayout(this);
	else
		mLayout = new QHBoxLayout(this);

  this->setTitle(title);

  this->populate(exclusive);
}

SelectionGroupBox::~SelectionGroupBox()
{}

QStringList SelectionGroupBox::getSelected()
{
  QStringList retval;
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    if(button->isChecked())
      retval << button->text();
  }
  return retval;
}

void SelectionGroupBox::setSelected(QStringList selectedlist)
{
  this->filter(selectedlist);
}

void SelectionGroupBox::setEnabledButtons(bool value)
{
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    button->setEnabled(value);
  }
}

void SelectionGroupBox::populate(bool exclusive)
{
  mButtonGroup->setExclusive(exclusive);
  foreach(QString string, mSelectionList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mButtonGroup->addButton(box);
    mLayout->addWidget(box);

    //need to tell the outside world that the state of a button changed
    connect(box, SIGNAL(stateChanged(int)), this, SIGNAL(selectionChanged()));
    connect(box, SIGNAL(clicked(bool)), this, SIGNAL(userClicked()));
  }
//  mLayout->addStretch();
  QHBoxLayout* hbox = dynamic_cast<QHBoxLayout*>(mLayout);
  if (hbox)
	  hbox->addStretch();
}

void SelectionGroupBox::filter(QStringList filter)
{
  bool exclusive = mButtonGroup->exclusive();

  mButtonGroup->setExclusive(false);
  QList<QAbstractButton*> applicationButtonList = mButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    bool on = filter.contains(button->text(), Qt::CaseInsensitive);
    button->setChecked(on);
  }
  mButtonGroup->setExclusive(exclusive);
}
}//namespace cx

