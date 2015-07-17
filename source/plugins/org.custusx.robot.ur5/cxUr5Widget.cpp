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

#include "cxUr5Widget.h"
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace cx
{

Ur5Widget::Ur5Widget(QWidget* parent) :
    QWidget(parent),
    mHorisontalLayout(new QHBoxLayout(this))
{
    this->setObjectName("Ur5Widget");
    this->setWindowTitle("UR5 Robot");
    this->setWhatsThis(this->defaultWhatsThis());

    //mHorisontalLayout->addWidget(new QLabel("Hello Plugin!"));

    QSpinBox *spinnerX = new QSpinBox;
    QSlider *sliderX   = new QSlider(Qt::Vertical);
    spinnerX->setRange(0,100);
    sliderX->setRange(0,100);
    QObject::connect(spinnerX, SIGNAL(valueChanged(int)), sliderX, SLOT(setValue(int)));
    QObject::connect(sliderX, SIGNAL(valueChanged(int)), spinnerX, SLOT(setValue(int)));
    spinnerX->setValue(0);

    QSpinBox *spinnerY = new QSpinBox;
    QSlider *sliderY   = new QSlider(Qt::Vertical);
    spinnerY->setRange(0,100);
    sliderY->setRange(0,100);
    QObject::connect(spinnerY, SIGNAL(valueChanged(int)), sliderY, SLOT(setValue(int)));
    QObject::connect(sliderY, SIGNAL(valueChanged(int)), spinnerY, SLOT(setValue(int)));
    spinnerY->setValue(0);

    QSpinBox *spinnerZ = new QSpinBox;
    QSlider *sliderZ   = new QSlider(Qt::Vertical);
    spinnerZ->setRange(0,100);
    sliderZ->setRange(0,100);
    QObject::connect(spinnerZ, SIGNAL(valueChanged(int)), sliderZ, SLOT(setValue(int)));
    QObject::connect(sliderZ, SIGNAL(valueChanged(int)), spinnerZ, SLOT(setValue(int)));
    spinnerY->setValue(0);

    mHorisontalLayout->addWidget(sliderX);
    mHorisontalLayout->addWidget(spinnerX);

    mHorisontalLayout->addWidget(sliderY);
    mHorisontalLayout->addWidget(spinnerY);

    mHorisontalLayout->addWidget(sliderZ);
    mHorisontalLayout->addWidget(spinnerZ);
}

Ur5Widget::~Ur5Widget()
{
}

QString Ur5Widget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Example plugin.</h3>"
      "<p>Used for developers as a starting points for developing a new plugin</p>"
      "</html>";
}



} /* namespace cx */
