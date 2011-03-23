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
#include "sscTypeConversions.h"

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
  return mBase->getTimestamp() - mProbe->getData().mTemporalCalibration;
}

void ProbeAdapterRTSource::probeChangedSlot()
{
//  std::cout << "ProbeAdapterRTSource::probeChangedSlot() validdata: " << validData() << std::endl;

  ssc::Vector3D dimImage(mRedirecter->GetOutput()->GetDimensions());
  QSize dimProbe = mProbe->getData().mImage.mSize;

  bool nonZero = ( dimProbe.width()!=0 )&&( dimProbe.height()!=0 )&&
                 ( dimImage[0]!=0 )&&( dimImage[1]!=0 );

  if (this->validData() &&nonZero && (( dimImage[0]!=dimProbe.width() )||( dimImage[1]!=dimProbe.height() )))
  {
    std::stringstream ss;
    ss << "Mismatch rt stream and probe image size. "
       << "RT dim=(" << dimImage[0] << ", " << dimImage[1] << "), "
       << "Probe dim=(" << dimProbe.width() << ", " << dimProbe.height() << ")";
    messageManager()->sendWarning(qstring_cast(ss.str()));
  }

  mRedirecter->SetOutputSpacing(mProbe->getData().mImage.mSpacing.begin());
}

}
