/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
* \addtogroup org_custusx_acquisition
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
