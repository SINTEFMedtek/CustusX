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

#include "cxActiveImageProxy.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxLandmark.h"

namespace cx
{

ActiveImageProxy::ActiveImageProxy(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	connect(mPatientModelService.get(), &PatientModelService::activeImageChanged, this,
			&ActiveImageProxy::activeImageChangedSlot);
	connect(mPatientModelService.get(), &PatientModelService::activeImageChanged, this,
			&ActiveImageProxy::activeImageChanged);
}


ActiveImageProxy::~ActiveImageProxy()
{
	disconnect(mPatientModelService.get(), &PatientModelService::activeImageChanged, this,
			   &ActiveImageProxy::activeImageChangedSlot);
	disconnect(mPatientModelService.get(), &PatientModelService::activeImageChanged, this,
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
	mImage = mPatientModelService->getActiveData<Image>();
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
