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

#ifndef CXSOUNDSPEEDCONVERSIONWIDGET_H_
#define CXSOUNDSPEEDCONVERSIONWIDGET_H_

#include "org_custusx_acquisition_Export.h"

#include "cxBaseWidget.h"
#include "cxTool.h"

class QPushButton;
class QDoubleSpinBox;

namespace cx
{
/**
* \file
* \addtogroup cx_module_acquisition
* @{
*/

/**
 * \class SoundSpeedConversionWidget
 *
 * \brief
 *
 * \date Feb 11, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_acquisition_EXPORT  SoundSpeedConverterWidget : public BaseWidget
{
  Q_OBJECT

public:
  SoundSpeedConverterWidget(TrackingServicePtr trackingService, QWidget* parent=NULL);
  ~SoundSpeedConverterWidget();

  virtual QString defaultWhatsThis() const;

public slots:
  void applySoundSpeedCompensationFactorSlot(); ///< sets the sounds speed conversion factor on the rt source

private slots:
  void waterSoundSpeedChangedSlot();
  void waterDegreeChangedSlot();
  void resetSlot();
  void setToolSlot(const QString& uid); ///< convenient slot for connecting to the toolmanagers activeToolChanged signal

private:
  void setSoundSpeed(double soundspeed);
  void setWaterDegree(double degree);
  void updateButtons();

  double getSoundSpeedCompensationFactor(); ///< calculates the sound speed conversion factor
  double getWaterSoundSpeed(); ///< the sound speed in water given a temperatur
  void setProbe(ProbePtr probe);

  const double mScannerSoundSpeed; //m/s
  double mToSoundSpeed; //m/s

  ProbePtr mProbe;
  TrackingServicePtr mTrackingService;

  QPushButton*    mApplyButton; ///< applies the compensation on the rt source
  QPushButton*    mResetButton; ///< resets the sound speed to scanner sound speed
  QDoubleSpinBox* mSoundSpeedSpinBox; //m/s
  QDoubleSpinBox* mWaterDegreeSpinBox; //celsius
};


/**
* @}
*/
}

#endif /* CXSOUNDSPEEDCONVERSIONWIDGET_H_ */
