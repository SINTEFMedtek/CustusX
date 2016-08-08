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

#include "cxSoundSpeedConversionWidget.h"

#include <QPushButton>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "cxLogger.h"
#include "cxTrackingService.h"
#include "cxProbe.h"

namespace cx
{

SoundSpeedConverterWidget::SoundSpeedConverterWidget(TrackingServicePtr trackingService, QWidget* parent) :
	BaseWidget(parent, "sound_speed_converter_widget", "Sound Speed Converter"),
    mScannerSoundSpeed(1540.0),
    mApplyButton(new QPushButton("Apply compensation")),
    mResetButton(new QPushButton("Reset")),
    mSoundSpeedSpinBox(new QDoubleSpinBox()),
	mWaterDegreeSpinBox(new QDoubleSpinBox()),
	mTrackingService(trackingService)
{
  QVBoxLayout* vLayout = new QVBoxLayout(this);
  this->setToolTip("Correct the sound speed received from the US scanner");

  connect(mApplyButton, SIGNAL(clicked()), this, SLOT(applySoundSpeedCompensationFactorSlot()));
  connect(mResetButton, SIGNAL(clicked()), this, SLOT(resetSlot()));

  mWaterDegreeSpinBox->setRange(0.0, 50.0);
  connect(mWaterDegreeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterDegreeChangedSlot()));
  mSoundSpeedSpinBox->setRange(1000.0, 2000.0); //what's a suitable range?
  connect(mSoundSpeedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterSoundSpeedChangedSlot()));

  QHBoxLayout* speedLayout = new QHBoxLayout();
  speedLayout->addWidget(new QLabel("Water sound speed: "));
  speedLayout->addWidget(mSoundSpeedSpinBox);
  speedLayout->addWidget(new QLabel("m/s, or "));
  speedLayout->addWidget(mWaterDegreeSpinBox);
  speedLayout->addWidget(new QLabel("C"+QString::fromUtf8("\302\260")+"")); //\302\260 is the degree sign

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(mApplyButton);
  buttonLayout->addWidget(mResetButton);

  vLayout->addLayout(speedLayout);
  vLayout->addLayout(buttonLayout);

  this->resetSlot();
  this->updateButtons();

  connect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &SoundSpeedConverterWidget::setToolSlot);
}

SoundSpeedConverterWidget::~SoundSpeedConverterWidget()
{}

void SoundSpeedConverterWidget::applySoundSpeedCompensationFactorSlot()
{
  if(!mProbe)
  {
    reportWarning("Don't know which probe to set the sound speed compensation for...");
    return;
  }

  double factor = this->getSoundSpeedCompensationFactor();
  mProbe->setSoundSpeedCompensationFactor(factor);
}

void SoundSpeedConverterWidget::setToolSlot(const QString& uid)
{
  ToolPtr tool = mTrackingService->getTool(uid);
  ProbePtr probe = tool->getProbe();
  if(!probe)
    return;
  this->setProbe(probe);
}

double SoundSpeedConverterWidget::getSoundSpeedCompensationFactor()
{
  return mToSoundSpeed/mScannerSoundSpeed;
}

double SoundSpeedConverterWidget::getWaterSoundSpeed()
{
  double waterDegree = mWaterDegreeSpinBox->value();
  double retval = 1402.40 + 5.01*waterDegree - 0.055*pow(waterDegree, 2) + 0.00022*pow(waterDegree, 3);

  return retval;
}

void SoundSpeedConverterWidget::setProbe(ProbePtr probe)
{
  mProbe = probe;
  this->updateButtons();
}

void SoundSpeedConverterWidget::waterSoundSpeedChangedSlot()
{
  mToSoundSpeed = mSoundSpeedSpinBox->value();

  QFont font = mWaterDegreeSpinBox->font();
  font.setStrikeOut(true);
  mWaterDegreeSpinBox->setFont(font);
}

void SoundSpeedConverterWidget::waterDegreeChangedSlot()
{
  mToSoundSpeed = this->getWaterSoundSpeed();

  if(mToSoundSpeed != mSoundSpeedSpinBox->value())
    mSoundSpeedSpinBox->setValue(mToSoundSpeed);

  QFont font = mWaterDegreeSpinBox->font();
  font.setStrikeOut(false);
  mWaterDegreeSpinBox->setFont(font);
}

void SoundSpeedConverterWidget::resetSlot()
{
  this->setSoundSpeed(mScannerSoundSpeed);

  if(!mProbe)
    return;
  this->applySoundSpeedCompensationFactorSlot();
}

void SoundSpeedConverterWidget::setSoundSpeed(double soundspeed)
{
  mSoundSpeedSpinBox->setValue(soundspeed);
}

void SoundSpeedConverterWidget::setWaterDegree(double degree)
{
  mWaterDegreeSpinBox->setValue(degree);
}

void SoundSpeedConverterWidget::updateButtons()
{
  mApplyButton->setEnabled(mProbe ? true : false);
  mResetButton->setEnabled(true);
}

}//namespace cx
