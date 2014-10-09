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

#include "cxPatientModelServiceProxy.h"

#include <boost/bind.hpp>
#include <ctkPluginContext.h>
#include "cxNullDeleter.h"
#include "cxReporter.h"
#include "cxLandmark.h"

namespace cx
{

PatientModelServiceProxy::PatientModelServiceProxy(ctkPluginContext *context) :
	mPluginContext(context),
	mPatientModelService(PatientModelService::getNullObject())
{
	this->initServiceListener();
}

PatientModelServiceProxy::~PatientModelServiceProxy()
{
//	mServiceListener.reset();//Needed?
}

void PatientModelServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<PatientModelService>(
								 mPluginContext,
								 boost::bind(&PatientModelServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (PatientModelService*)>(),
								 boost::bind(&PatientModelServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}

void PatientModelServiceProxy::onServiceAdded(PatientModelService* service)
{
	mPatientModelService.reset(service, null_deleter());
	connect(service, SIGNAL(dataAddedOrRemoved()), this, SIGNAL(dataAddedOrRemoved()));
	connect(service, SIGNAL(activeImageChanged(const QString&)), this, SIGNAL(activeImageChanged(const QString&)));
	connect(service, SIGNAL(debugModeChanged(bool)), this, SIGNAL(debugModeChanged(bool)));
	connect(service, SIGNAL(rMprChanged()), this, SIGNAL(rMprChanged()));
	connect(service, SIGNAL(streamLoaded()), this, SIGNAL(streamLoaded()));
	if(mPatientModelService->isNull())
		reportWarning("PatientModelServiceProxy::onServiceAdded mPatientModelService->isNull()");
}

void PatientModelServiceProxy::onServiceRemoved(PatientModelService *service)
{
	disconnect(service, SIGNAL(dataAddedOrRemoved()), this, SIGNAL(dataAddedOrRemoved()));
	disconnect(service, SIGNAL(activeImageChanged(const QString&)), this, SIGNAL(activeImageChanged(const QString&)));
	disconnect(service, SIGNAL(debugModeChanged(bool)), this, SIGNAL(debugModeChanged(bool)));
	disconnect(service, SIGNAL(rMprChanged()), this, SIGNAL(rMprChanged()));
	disconnect(service, SIGNAL(streamLoaded()), this, SIGNAL(streamLoaded()));
	mPatientModelService = PatientModelService::getNullObject();
}

void PatientModelServiceProxy::insertData(DataPtr data)
{
	mPatientModelService->insertData(data);
}

void PatientModelServiceProxy::updateRegistration_rMpr(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
	mPatientModelService->updateRegistration_rMpr(oldTime, newTransform);
}

std::map<QString, DataPtr> PatientModelServiceProxy::getData() const
{
	return mPatientModelService->getData();
}

DataPtr PatientModelServiceProxy::getData(const QString& uid) const
{
	return mPatientModelService->getData(uid);
}

LandmarksPtr PatientModelServiceProxy::getPatientLandmarks() const
{
	return mPatientModelService->getPatientLandmarks();
}

std::map<QString, LandmarkProperty> PatientModelServiceProxy::getLandmarkProperties() const
{
	return mPatientModelService->getLandmarkProperties();
}

Transform3D PatientModelServiceProxy::get_rMpr() const
{
	return mPatientModelService->get_rMpr();
}

ImagePtr PatientModelServiceProxy::getActiveImage() const
{
	return mPatientModelService->getActiveImage();
}

void PatientModelServiceProxy::setActiveImage(ImagePtr activeImage)
{
	mPatientModelService->setActiveImage(activeImage);
}

void PatientModelServiceProxy::autoSave()
{
	mPatientModelService->autoSave();
}

bool PatientModelServiceProxy::isNull()
{
	return mPatientModelService->isNull();
}

bool PatientModelServiceProxy::getDebugMode() const
{
	return mPatientModelService->getDebugMode();
}

void PatientModelServiceProxy::setDebugMode(bool on)
{
	mPatientModelService->setDebugMode(on);
}

cx::ImagePtr cx::PatientModelServiceProxy::createDerivedImage(vtkImageDataPtr data, QString uid, QString name, cx::ImagePtr parentImage, QString filePath)
{
	return mPatientModelService->createDerivedImage(data, uid, name, parentImage, filePath);
}

MeshPtr PatientModelServiceProxy::createMesh(vtkPolyDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
	return mPatientModelService->createMesh(data, uidBase, nameBase, filePath);
}

ImagePtr PatientModelServiceProxy::createImage(vtkImageDataPtr data, QString uidBase, QString nameBase, QString filePath)
{
	return mPatientModelService->createImage(data, uidBase, nameBase, filePath);
}

void PatientModelServiceProxy::loadData(DataPtr data)
{
	mPatientModelService->loadData(data);
}

void PatientModelServiceProxy::saveData(DataPtr data, const QString &basePath)
{
	mPatientModelService->saveData(data, basePath);
}

void PatientModelServiceProxy::saveImage(ImagePtr image, const QString &basePath)
{
	mPatientModelService->saveImage(image, basePath);
}

void PatientModelServiceProxy::saveMesh(MeshPtr mesh, const QString &basePath)
{
	mPatientModelService->saveMesh(mesh, basePath);
}

std::map<QString, VideoSourcePtr> PatientModelServiceProxy::getStreams() const
{
	return mPatientModelService->getStreams();
}

QString PatientModelServiceProxy::getActivePatientFolder() const
{
	return mPatientModelService->getActivePatientFolder();
}

bool PatientModelServiceProxy::isPatientValid() const
{
	return mPatientModelService->isPatientValid();
}

DataPtr PatientModelServiceProxy::importData(QString fileName, QString &infoText)
{
	return mPatientModelService->importData(fileName, infoText);
}

void PatientModelServiceProxy::exportPatient(bool niftiFormat)
{
	return mPatientModelService->exportPatient(niftiFormat);
}

void PatientModelServiceProxy::removePatientData(QString uid)
{
	return mPatientModelService->removePatientData(uid);
}

PresetTransferFunctions3DPtr PatientModelServiceProxy::getPresetTransferFunctions3D() const
{
	return mPatientModelService->getPresetTransferFunctions3D();
}

void PatientModelServiceProxy::setLandmarkActive(QString uid, bool active)
{
	mPatientModelService->setLandmarkActive(uid, active);
}

} //cx
