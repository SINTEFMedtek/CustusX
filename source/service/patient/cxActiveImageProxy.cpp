/*
 * cxActiveImageProxy.cpp
 *
 *  Created on: Oct 18, 2011
 *      Author: olevs
 */

#include <cxActiveImageProxy.h>
#include "cxDatamanager.h"

namespace cx
{

ActiveImageProxy::ActiveImageProxy()
{
	connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChangedSlot(const QString&)));
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SIGNAL(activeImageChanged(const QString&)));

}

void ActiveImageProxy::activeImageChangedSlot(const QString& uid)
{
	if(mImage && mImage->getUid() == uid)
		return;

	if(mImage)
	{
		disconnect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
		disconnect(mImage.get(), SIGNAL(propertiesChanged()), this, SIGNAL(propertiesChanged()));
		disconnect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(landmarkRemoved(QString)));
		disconnect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(landmarkAdded(QString)));
		disconnect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SIGNAL(vtkImageDataChanged()));
		disconnect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
    disconnect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(clipPlanesChanged()));
    disconnect(mImage.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(cropBoxChanged()));
	}
	mImage = ssc::dataManager()->getActiveImage();
	if(mImage)
	{
    connect(mImage.get(), SIGNAL(transformChanged()), this, SIGNAL(transformChanged()));
    connect(mImage.get(), SIGNAL(propertiesChanged()), this, SIGNAL(propertiesChanged()));
    connect(mImage.get(), SIGNAL(landmarkRemoved(QString)), this, SIGNAL(landmarkRemoved(QString)));
    connect(mImage.get(), SIGNAL(landmarkAdded(QString)), this, SIGNAL(landmarkAdded(QString)));
    connect(mImage.get(), SIGNAL(vtkImageDataChanged()), this, SIGNAL(vtkImageDataChanged()));
    connect(mImage.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
    connect(mImage.get(), SIGNAL(clipPlanesChanged()), this, SIGNAL(clipPlanesChanged()));
    connect(mImage.get(), SIGNAL(cropBoxChanged()), this, SIGNAL(cropBoxChanged()));
	}
}

}//namespace cx
