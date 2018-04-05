/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxActiveImageProxy.h"
#include "cxImage.h"
#include "cxLandmark.h"
#include "cxActiveData.h"

namespace cx
{

ActiveImageProxy::ActiveImageProxy(ActiveDataPtr activeData) :
	mActiveData(activeData)
{
	connect(mActiveData.get(), &ActiveData::activeImageChanged, this,
			&ActiveImageProxy::activeImageChangedSlot);
	connect(mActiveData.get(), &ActiveData::activeImageChanged, this,
			&ActiveImageProxy::activeImageChanged);
}

ActiveImageProxy::~ActiveImageProxy()
{
	disconnect(mActiveData.get(), &ActiveData::activeImageChanged, this,
			   &ActiveImageProxy::activeImageChangedSlot);
	disconnect(mActiveData.get(), &ActiveData::activeImageChanged, this,
			   &ActiveImageProxy::activeImageChanged);
}

void ActiveImageProxy::activeImageChangedSlot(const QString& uid)
{
	if (mImage && mImage->getUid() == uid)
		return;

	if (mImage)
	{
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
		disconnect(mImage.get(), SIGNAL(propertiesChanged()), this, SIGNAL(propertiesChanged()));
		disconnect(mImage->getLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(landmarkRemoved(QString)));
		disconnect(mImage->getLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(landmarkAdded(QString)));
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SIGNAL(vtkImageDataChanged()));
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
		disconnect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(clipPlanesChanged()));
		disconnect(mImage.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(cropBoxChanged()));
	}
	mImage = mActiveData->getActive<Image>();
	if (mImage)
	{
		connect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
		connect(mImage.get(), SIGNAL(propertiesChanged()), this, SIGNAL(propertiesChanged()));
		connect(mImage->getLandmarks().get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(landmarkRemoved(QString)));
		connect(mImage->getLandmarks().get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(landmarkAdded(QString)));
		connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SIGNAL(vtkImageDataChanged()));
		connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
		connect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(clipPlanesChanged()));
		connect(mImage.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(cropBoxChanged()));
	}
}

} //namespace cx
