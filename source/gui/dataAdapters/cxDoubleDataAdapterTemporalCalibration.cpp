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
