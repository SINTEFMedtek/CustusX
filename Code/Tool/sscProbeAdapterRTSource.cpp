// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

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

ProbeAdapterRTSource::ProbeAdapterRTSource(QString uid, ssc::ProbePtr probe, ssc::VideoSourcePtr source) :
	mUid(uid), mBase(source), mProbe(probe)
{
	connect(mProbe.get(), SIGNAL(sectorChanged()), this, SLOT(probeChangedSlot()));

	connect(mBase.get(), SIGNAL(streaming(bool)), this, SIGNAL(streaming(bool)));
	connect(mBase.get(), SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(mBase.get(), SIGNAL(newFrame()), this, SIGNAL(newFrame()));

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
	return mBase->getTimestamp() - mProbe->getData().getTemporalCalibration();
}

void ProbeAdapterRTSource::probeChangedSlot()
{
	//  std::cout << "ProbeAdapterRTSource::probeChangedSlot() validdata: " << validData() << std::endl;

	Eigen::Array3i dimImage(mRedirecter->GetOutput()->GetDimensions());
	//  ssc::Vector3D dimImage(mRedirecter->GetOutput()->GetDimensions());
	QSize dimProbe = mProbe->getData().getImage().mSize;

	bool nonZero = (dimProbe.width() != 0) && (dimProbe.height() != 0) && (dimImage[0] != 0) && (dimImage[1] != 0);

	if (this->validData() && nonZero && ((dimImage[0] != dimProbe.width()) || (dimImage[1] != dimProbe.height())))
	{
		std::stringstream ss;
		ss << "Mismatch rt stream and probe image size. " << "RT dim=(" << dimImage[0] << ", " << dimImage[1] << "), "
			<< "Probe dim=(" << dimProbe.width() << ", " << dimProbe.height() << ")";
		messageManager()->sendWarning(qstring_cast(ss.str()));
	}

	// Don't change spacing if it have an existing spacing from the OpenIGTLink message
	//  if (mBase->getVtkImageData()->GetSpacing()[0] == 0)
	{
		mRedirecter->SetOutputSpacing(mProbe->getData().getImage().mSpacing.begin());
	}
}

}
