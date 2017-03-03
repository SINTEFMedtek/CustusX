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

#include "cxBaseWidget.h"
#include "cxTypeConversions.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLabel>

namespace cx
{

BaseWidget::BaseWidget(QWidget* parent, QString objectName, QString windowTitle) :
	OptimizedUpdateWidget(parent), mObjectName(objectName), mWindowTitle(windowTitle)
{
	this->setFocusPolicy(Qt::StrongFocus); // needed for help system: focus is used to display help text
    this->setObjectName(mObjectName);
    this->setWindowTitle(mWindowTitle);
}

QWidget* BaseWidget::createMethodWidget(QWidget* inputWidget, QWidget* outputWidget, QString methodname, bool inputChecked, bool outputVisible)
{
    QWidget* retval = new QWidget(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);
    QGridLayout* layout = new QGridLayout();
    toplayout->addLayout(layout);
    toplayout->addStretch();

    QLabel* methodLabel = new QLabel("<b>" + methodname + "</b>");
    QCheckBox* checkBox = new QCheckBox("generate");

    inputWidget->setVisible(inputChecked);
    outputWidget->setVisible(outputVisible);
    connect(checkBox, SIGNAL(clicked(bool)), inputWidget, SLOT(setVisible(bool)));

    layout->addWidget(methodLabel, 0, 0);
    layout->addWidget(checkBox, 0, 1);
    layout->addWidget(inputWidget, 1, 0, 1, 2);
    layout->addWidget(outputWidget, 2, 0, 1, 2);

    return retval;
}

QGroupBox* BaseWidget::createGroupbox(QWidget* widget, QString boxname)
{
    QGroupBox* retval = new QGroupBox(this);
    QVBoxLayout* toplayout = new QVBoxLayout(retval);

    QLabel* nameLabel = new QLabel(boxname);
    toplayout->addWidget(nameLabel);
    toplayout->addWidget(widget);

    return retval;
}

QFrame* BaseWidget::createHorizontalLine() ///< creates a horizontal line witch can be inserted into widgets
{
    QFrame* retval = new QFrame();
    retval->setFrameStyle(QFrame::Sunken + QFrame::HLine);
    return retval;
}

CXFrame* BaseWidget::wrapInFrame(QWidget* base)
{
    CXFrame* frame = new CXFrame(NULL);
    QVBoxLayout* layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    layout->addWidget(base);
    return frame;
}

QGroupBox* BaseWidget::wrapInGroupBox(QWidget* base, QString name)
{
    QGroupBox* groupBox = new QGroupBox(name);
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->setMargin(2);
    layout->addWidget(base);
    return groupBox;
}

void BaseWidget::adjustSizeSlot()
{
    this->parentWidget()->adjustSize();
    this->adjustSize();
}

void BaseWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

}

