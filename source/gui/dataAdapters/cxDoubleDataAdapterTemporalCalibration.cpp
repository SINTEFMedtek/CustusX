/*
 * cxDoubleDataAdapterTemporalCalibration.cpp
 *
 *  \date May 5, 2011
 *      \author christiana
 */

#include <cxDoubleDataAdapterTemporalCalibration.h>
#include "cxToolManager.h"

namespace cx
{

DoubleDataAdapterTimeCalibration::DoubleDataAdapterTimeCalibration()
{
  connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChanged()));
  connect(toolManager(), SIGNAL(configured()), this, SLOT(dominantToolChanged())); // for debugging: if initializing a manual tool with probe properties
  connect(toolManager(), SIGNAL(trackingStarted()), this, SLOT(dominantToolChanged()));
  this->dominantToolChanged();
}

void DoubleDataAdapterTimeCalibration::dominantToolChanged()
{
//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged()" << std::endl;

  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ToolPtr newTool = toolManager()->getDominantTool();
  if (!newTool || !newTool->hasType(Tool::TOOL_US_PROBE))
    return;

  if (mTool)
    disconnect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

  mTool = toolManager()->getDominantTool();

  if (mTool)
    connect(mTool->getProbe().get(), SIGNAL(sectorChanged()), this, SIGNAL(changed()));

//  std::cout << "DoubleDataAdapterTimeCalibration::dominantToolChanged() .. " << mTool.get() << std::endl;
  emit changed();
}

DoubleDataAdapterPtr DoubleDataAdapterTimeCalibration::New()
{
  return DoubleDataAdapterPtr(new DoubleDataAdapterTimeCalibration());
}

double DoubleDataAdapterTimeCalibration::getValue() const
{
//  std::cout << "DoubleDataAdapterTimeCalibration::getValue()" << std::endl;
  if (!mTool)
    return 0;
//  std::cout << "mTool->getProbe()->getData().mTemporalCalibration " << mTool->getProbe()->getData().getTemporalCalibration() << std::endl;
  return mTool->getProbe()->getProbeData().getTemporalCalibration();
}

QString DoubleDataAdapterTimeCalibration::getHelp() const
{
	return "Set a temporal shift to add to input probe frames (the frames will be stored with this shift applied).\n"
		"Changes done here will NOT be saved.";
}

bool DoubleDataAdapterTimeCalibration::setValue(double val)
{
  if (!mTool)
    return 0;
  mTool->getProbe()->setTemporalCalibration(val);
  return true;
}

DoubleRange DoubleDataAdapterTimeCalibration::getValueRange() const
{
  return DoubleRange(-50000,50000,1);
}

}
