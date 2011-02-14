#include "cxSoundSpeedConversionWidget.h"

#include <QPushButton>;
#include <QDoubleSpinBox>;
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "sscMessageManager.h"
#include "cxProbe.h"

namespace cx
{

SoundSpeedConverterWidget::SoundSpeedConverterWidget(QWidget* parent) :
    QWidget(parent),
    mFromSoundSpeed(1540.0),
    mApplyButton(new QPushButton("Apply")),
    mSoundSpeedSpinBox(new QDoubleSpinBox()),
    mWaterDegreeSpinBox(new QDoubleSpinBox())
{
  QVBoxLayout* vLayout = new QVBoxLayout(this);

  connect(mApplyButton, SIGNAL(clicked()), this, SLOT(applySoundSpeedCompensationFactorSlot()));

  mWaterDegreeSpinBox->setRange(0.0, 50.0);
  mWaterDegreeSpinBox->setValue(25.0);
  connect(mWaterDegreeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterDegreeChangedSlot()));

  mToSoundSpeed = this->getWaterSoundSpeed();

  mSoundSpeedSpinBox->setRange(1000.0, 2000.0); //what's a suitable range?
  mSoundSpeedSpinBox->setValue(mToSoundSpeed);
  connect(mSoundSpeedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(waterSoundSpeedChangedSlot()));

  QHBoxLayout* speedLayout = new QHBoxLayout();
  speedLayout->addWidget(new QLabel("Water sound speed: "));
  speedLayout->addWidget(mSoundSpeedSpinBox);
  speedLayout->addWidget(new QLabel("m/s, or"));
  speedLayout->addWidget(mWaterDegreeSpinBox);
  speedLayout->addWidget(new QLabel("C"+QString::fromUtf8("\302\260"))); //\302\260 is the degree sign

  vLayout->addLayout(speedLayout);
  vLayout->addWidget(mApplyButton);
}

SoundSpeedConverterWidget::~SoundSpeedConverterWidget()
{}

void SoundSpeedConverterWidget::applySoundSpeedCompensationFactorSlot()
{
  if(!mProbe)
  {
    ssc::messageManager()->sendWarning("Don't know which probe to set the sound speed compensation for...");
    return;
  }

  double factor = this->getSoundSpeedCompensationFactor();

  ProbePtr probe = boost::dynamic_pointer_cast<Probe>(mProbe->getProbe());
  if(probe)
    probe->setSoundSpeedCompensationFactor(factor);
  else
    ssc::messageManager()->sendDebug("Could not cast probe to a cx probe...");
}

double SoundSpeedConverterWidget::getSoundSpeedCompensationFactor()
{
  return mToSoundSpeed/mFromSoundSpeed;
}

double SoundSpeedConverterWidget::getWaterSoundSpeed()
{
  double waterDegree = mWaterDegreeSpinBox->value();
  double retval = 1402.40 + 5.01*waterDegree - 0.055*pow(waterDegree, 2) + 0.00022*pow(waterDegree, 3);

  return retval;
}

void SoundSpeedConverterWidget::setProbe(ToolPtr probe)
{
  mProbe = probe;
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
}//namespace cx
