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

#include "cxPatientModelImplService.h"

#include <ctkPluginContext.h>
#include "cxData.h"
#include "cxReporter.h"
#include "cxLogicManager.h"
#include "cxDataManager.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxRegistrationTransform.h"

namespace cx
{

PatientModelImplService::PatientModelImplService(ctkPluginContext *context) :
	mContext(context )
{
	connect(dataService().get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(dataAddedOrRemoved()));
	connect(dataService().get(), SIGNAL(activeImageChanged(const QString&)), this, SIGNAL(activeImageChanged(const QString&)));
	connect(dataService().get(), SIGNAL(debugModeChanged(bool)), this, SIGNAL(debugModeChanged(bool)));
	connect(dataService().get(), SIGNAL(rMprChanged()), this, SIGNAL(rMprChanged()));
	connect(dataService().get(), SIGNAL(streamLoaded()), this, SIGNAL(streamLoaded()));
}

PatientModelImplService::~PatientModelImplService()
{
	if(dataService())
	{
		disconnect(dataService().get(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(dataAddedOrRemoved()));
		disconnect(dataService().get(), SIGNAL(activeImageChanged(const QString&)), this, SIGNAL(activeImageChanged(const QString&)));
		disconnect(dataService().get(), SIGNAL(debugModeChanged(bool)), this, SIGNAL(debugModeChanged(bool)));
		disconnect(dataService().get(), SIGNAL(rMprChanged()), this, SIGNAL(rMprChanged()));
		disconnect(dataService().get(), SIGNAL(streamLoaded()), this, SIGNAL(streamLoaded()));
	}
}

void PatientModelImplService::insertData(DataPtr data)
{
	LogicManager* lm = LogicManager::getInstance();
	lm->getDataService()->loadData(data);
	QString outputBasePath = lm->getPatientService()->getPatientData()->getActivePatientFolder();
	lm->getDataService()->saveData(data, outputBasePath);
}

void PatientModelImplService::updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
	dataService()->get_rMpr_History()->updateRegistration(oldTime, newTransform);
	patientService()->getPatientData()->autoSave();
}

std::map<QString, DataPtr> PatientModelImplService::getData() const
{
	return dataService()->getData();
}

DataPtr PatientModelImplService::getData(const QString& uid) const
{
	std::map<QString, DataPtr> dataMap = this->getData();
	std::map<QString, DataPtr>::const_iterator iter = dataMap.find(uid);
	if (iter == dataMap.end())
		return DataPtr();
	return iter->second;
}

LandmarksPtr PatientModelImplService::getPatientLandmarks() const
{
	return dataService()->getPatientLandmarks();
}

std::map<QString, LandmarkProperty> PatientModelImplService::getLandmarkProperties() const
{
	return dataService()->getLandmarkProperties();
}

Transform3D PatientModelImplService::get_rMpr() const
{
	return dataService()->get_rMpr();
}

void PatientModelImplService::autoSave()
{
	patientService()->getPatientData()->autoSave();
}

bool PatientModelImplService::isNull()
{
	return false;
}

bool PatientModelImplService::getDebugMode() const
{
	return dataService()->getDebugMode();
}
void PatientModelImplService::setDebugMode(bool on)
{
	dataService()->setDebugMode(on);
}

ImagePtr PatientModelImplService::getActiveImage() const
{
	return dataService()->getActiveImage();
}

void PatientModelImplService::setActiveImage(ImagePtr activeImage)
{
	dataService()->setActiveImage(activeImage);
}

cx::ImagePtr cx::PatientModelImplService::createDerivedImage(vtkImageDataPtr data, QString uid, QString name, cx::ImagePtr parentImage, QString filePath)
{
	return dataService()->createDerivedImage(data, uid, name, parentImage, filePath);
}

MeshPtr PatientModelImplService::createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
	return dataService()->createMesh(data, uidBase, nameBase, filePath);
}

void PatientModelImplService::loadData(DataPtr data)
{
	dataService()->loadData(data);
}

void PatientModelImplService::saveData(DataPtr data, const QString &basePath)
{
	dataService()->saveData(data, basePath);
}

void PatientModelImplService::saveImage(ImagePtr image, const QString &basePath)
{
	dataService()->saveImage(image, basePath);
}

void PatientModelImplService::saveMesh(MeshPtr mesh, const QString &basePath)
{
	dataService()->saveMesh(mesh, basePath);
}

std::map<QString, VideoSourcePtr> PatientModelImplService::getStreams() const
{
	dataService()->getStreams();
}

QString PatientModelImplService::getActivePatientFolder() const
{
	return patientService()->getPatientData()->getActivePatientFolder();
}

} /* namespace cx */
