/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxDoublePropertyTemporalCalibration.h"
#include "cxTrackingService.h"

namespace cx
{

DoublePropertyTimeCalibration::DoublePropertyTimeCalibration(TrackingServicePtr trackingService) :
	mTrackingService(trackingService)
{
  connect(mTrackingService.get(), &TrackingService::activeToolChanged, this, &DoublePropertyTimeCalibration::activeToolChanged);
  connect(mTrackingService.get(), &TrackingService::stateChanged, this, &DoublePropertyTimeCalibration::activeToolChanged);
  this->activeToolChanged();
}

void DoublePropertyTimeCalibration::activeToolChanged()
{
  // ignore tool changes to something non-probeish.
  // This gives the user a chance to use the widget without having to show the probe.
  ToolPtr newTool = mTrackingService->getFirstProbe();
  if (!newTool || !newTool->getProbe())
    return;

  if (mTool)
	  disconnect(mTool->getProbe().get(), &Probe::sectorChanged, this, &DoublePropertyTimeCalibration::changed);

  mTool = newTool;

  if (mTool)
	  connect(mTool->getProbe().get(), &Probe::sectorChanged, this, &DoublePropertyTimeCalibration::changed);

  emit changed();
}

DoublePropertyBasePtr DoublePropertyTimeCalibration::New(TrackingServicePtr trackingService)
{
  return DoublePropertyBasePtr(new DoublePropertyTimeCalibration(trackingService));
}

double DoublePropertyTimeCalibration::getValue() const
{
  if (!mTool || !mTool->getProbe())
    return 0;
  return mTool->getProbe()->getProbeDefinition().getTemporalCalibration();
}

QString DoublePropertyTimeCalibration::getHelp() const
{
	return "Set a temporal shift to add to input probe frames (the frames will be stored with this shift applied).\n"
		"Changes done here will NOT be saved.";
}

bool DoublePropertyTimeCalibration::setValue(double val)
{
  if (!mTool)
    return 0;
  mTool->getProbe()->setTemporalCalibration(val);
  return true;
}

DoubleRange DoublePropertyTimeCalibration::getValueRange() const
{
  return DoubleRange(-50000,50000,1);
}

}
