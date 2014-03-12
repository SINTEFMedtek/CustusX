// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxActiveImageProxy.h"
#include "sscDataManager.h"
#include "sscImage.h"

namespace cx
{

ActiveImageProxy::ActiveImageProxy(DataServicePtr dataManager) :
	mDataManager(dataManager)
{
	connect(mDataManager.get(), SIGNAL(activeImageChanged(const QString&)), this,
					SLOT(activeImageChangedSlot(const QString&)));
	connect(mDataManager.get(), SIGNAL(activeImageChanged(const QString&)), this,
					SIGNAL(activeImageChanged(const QString&)));

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
	mImage = mDataManager->getActiveImage();
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
