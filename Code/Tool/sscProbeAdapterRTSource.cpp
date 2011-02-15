/*
 * cxProbeAdapterRTSource.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: christiana
 */

#include "sscProbeAdapterRTSource.h"
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "sscMessageManager.h"
#include "sscTool.h"

namespace ssc
{

ProbeAdapterRTSource::ProbeAdapterRTSource(QString uid, ssc::ProbePtr probe, ssc::RTSourcePtr source) :
    mUid(uid), mProbe(probe), mBase(source)
{
  connect(mProbe.get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

  connect(mBase.get(), SIGNAL(streaming(bool)), this, SIGNAL(streaming(bool)));
  connect(mBase.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
  connect(mBase.get(), SIGNAL(newFrame()),      this, SIGNAL(newFrame()));

  mRedirecter = vtkImageChangeInformationPtr::New();
  mRedirecter->SetInput(mBase->getVtkImageData());
  this->probeChangedSlot();
}

QString ProbeAdapterRTSource::getUid()
{
  return mUid;
}

vtkImageDataPtr ProbeAdapterRTSource::getVtkImageData()
{
  return mRedirecter->GetOutput();
}

double ProbeAdapterRTSource::getTimestamp()
{
  return mBase->getTimestamp() + mProbe->getData().mTemporalCalibration;
}

void ProbeAdapterRTSource::probeChangedSlot()
{
  mRedirecter->SetOutputSpacing(mProbe->getData().mImage.mSpacing.begin());
}

}
