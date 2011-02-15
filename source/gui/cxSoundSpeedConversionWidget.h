#ifndef CXSOUNDSPEEDCONVERSIONWIDGET_H_
#define CXSOUNDSPEEDCONVERSIONWIDGET_H_

#include <QWidget>
#include "cxTool.h"

class QPushButton;
class QDoubleSpinBox;

namespace cx {

/**
 * \class SoundSpeedConversionWidget
 *
 * \brief
 *
 * \date Feb 11, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class SoundSpeedConverterWidget : public QWidget
{
  Q_OBJECT

public:
  SoundSpeedConverterWidget(QWidget* parent);
  ~SoundSpeedConverterWidget();

  double getSoundSpeedCompensationFactor(); ///< calculates the sound speed conversion factor
  double getWaterSoundSpeed(); ///< the sound speed in water given a temperatur

  void setProbe(ssc::ProbePtr probe);

public slots:
  void applySoundSpeedCompensationFactorSlot(); ///< sets the sounds speed conversion factor on the rt source
  void setToolSlot(const QString& uid); ///< convenient slot for connecting to the toolmanagers dominantToolChanged signal

private slots:
  void waterSoundSpeedChangedSlot();
  void waterDegreeChangedSlot();
  void resetSlot();

private:
  void setSoundSpeed(double soundspeed);
  void setWaterDegree(double degree);
  void updateButtons();

  const double mScannerSoundSpeed; //m/s
  double mToSoundSpeed; //m/s

  ssc::ProbePtr mProbe;

  QPushButton*    mApplyButton; ///< applies the compensation on the rt source
  QPushButton*    mResetButton; ///< resets the sound speed to scanner sound speed
  QDoubleSpinBox* mSoundSpeedSpinBox; //m/s
  QDoubleSpinBox* mWaterDegreeSpinBox; //celsius
};
}

#endif /* CXSOUNDSPEEDCONVERSIONWIDGET_H_ */
