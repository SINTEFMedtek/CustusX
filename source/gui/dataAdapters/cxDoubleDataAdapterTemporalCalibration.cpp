/*
 * cxDoubleDataAdapterTemporalCalibration.cpp
 *
 *  \date May 5, 2011
 *      \author christiana
 */

#include <cxDoubleDataAdapterTemporalCalibration.h>
#include "sscToolManager.h"

namespace cx
{

DoubleDataAdapterTimeCalibration::DoubleDataAdapterTimeCalibration()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
  this->dominantToolChanged();
}

void DoubleDataAdapterTimeCalibration::dominantToolChanged()
{
//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged()" << std::endl;

  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ssc::ToolPtr newTool = ssc::toolManager()->getDominantTool();
  if (!newTool || !newTool->hasType(ssc::Tool::TOOL_US_PROBE))
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = ssc::toolManager()->getDominantTool();

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged() .. " << mTool.get() << std::endl;
  emit changed();
}

ssc::DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return ssc::DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
//  std::cout << "DoubleDataAdapterTimeCalibration::getValue()" << std::endl;
  if (!mTool)
    return 0;
//  std::cout << "mTool->getProbe()->getData().mTemporalCalibration " << mTool->getProbe()->getData().mTemporalCalibration << std::endl;
  return mTool->getProbe()->getData().getTemporalCalibration();
}

QString DoubleDataAdapterTimeCalibration::getHelp() const
{
  return "Set a temporal shift to add to input probe frames.\n"
	  "Changes done here will NOT be saved.";
}

bool DoubleDataAdapterTimeCalibration::setValue(double val)
{
  if (!mTool)
    return 0;
  mTool->getProbe()->setTemporalCalibration(val);
  return true;
}

ssc::DoubleRange DoubleDataAdapterTimeCalibration::getValueRange() const
{
  return ssc::DoubleRange(-50000,50000,1);
}

}
