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
