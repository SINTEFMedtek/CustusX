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

#include "cxUsReconstructionServiceProxy.h"

#include <boost/bind.hpp>
//#include <QString.h>

//#include "cxUSReconstructInputData.h"


//#include <set>
//#include "cxForwardDeclarations.h"
//#include "cxXmlOptionItem.h"
//#include "cxReconstructCore.h"
#include "cxUSReconstructInputData.h"
//#include "cxReconstructedOutputVolumeParams.h"
//#include "cxReconstructionMethodService.h"
//#include "cxServiceTrackerListener.h"

#include "cxNullDeleter.h"

namespace cx {

UsReconstructionServiceProxy::UsReconstructionServiceProxy(ctkPluginContext *pluginContext) :
	mPluginContext(pluginContext),
	mUsReconstructionService(UsReconstructionService::getNullObject())
{
	this->initServiceListener();
}


void UsReconstructionServiceProxy::initServiceListener()
{
	mServiceListener.reset(new ServiceTrackerListener<UsReconstructionService>(
								 mPluginContext,
								 boost::bind(&UsReconstructionServiceProxy::onServiceAdded, this, _1),
								 boost::function<void (UsReconstructionService*)>(),
								 boost::bind(&UsReconstructionServiceProxy::onServiceRemoved, this, _1)
								 ));
	mServiceListener->open();
}
void UsReconstructionServiceProxy::onServiceAdded(UsReconstructionService* service)
{
	mUsReconstructionService.reset(service, null_deleter());
	connect(service, &UsReconstructionService::paramsChanged, this, &UsReconstructionService::paramsChanged);
	connect(service, &UsReconstructionService::algorithmChanged, this, &UsReconstructionService::algorithmChanged);
	connect(service, &UsReconstructionService::inputDataSelected, this, &UsReconstructionService::inputDataSelected);
	connect(service, &UsReconstructionService::reconstructAboutToStart, this, &UsReconstructionService::reconstructAboutToStart);
	connect(service, &UsReconstructionService::reconstructStarted, this, &UsReconstructionService::reconstructStarted);
	connect(service, &UsReconstructionService::reconstructFinished, this, &UsReconstructionService::reconstructFinished);
	connect(service, &UsReconstructionService::newInputDataAvailable, this, &UsReconstructionService::newInputDataAvailable);
	connect(service, &UsReconstructionService::newInputDataPath, this, &UsReconstructionService::newInputDataAvailable);

	if(mUsReconstructionService->isNull())
		reportWarning("UsReconstructionServiceProxy::onServiceAdded mVideoService->isNull()");
}

void UsReconstructionServiceProxy::onServiceRemoved(UsReconstructionService *service)
{
	disconnect(service, &UsReconstructionService::paramsChanged, this, &UsReconstructionService::paramsChanged);
	disconnect(service, &UsReconstructionService::algorithmChanged, this, &UsReconstructionService::algorithmChanged);
	disconnect(service, &UsReconstructionService::inputDataSelected, this, &UsReconstructionService::inputDataSelected);
	disconnect(service, &UsReconstructionService::reconstructAboutToStart, this, &UsReconstructionService::reconstructAboutToStart);
	disconnect(service, &UsReconstructionService::reconstructStarted, this, &UsReconstructionService::reconstructStarted);
	disconnect(service, &UsReconstructionService::reconstructFinished, this, &UsReconstructionService::reconstructFinished);
	disconnect(service, &UsReconstructionService::newInputDataAvailable, this, &UsReconstructionService::newInputDataAvailable);
	disconnect(service, &UsReconstructionService::newInputDataPath, this, &UsReconstructionService::newInputDataAvailable);
	mUsReconstructionService = UsReconstructionService::getNullObject();
}

bool UsReconstructionServiceProxy::isNull()
{
	return mUsReconstructionService->isNull();
}



void UsReconstructionServiceProxy::selectData(QString filename, QString calFilesPath)
{
	mUsReconstructionService->selectData(filename, calFilesPath);
}

void UsReconstructionServiceProxy::selectData(USReconstructInputData data)
{
	mUsReconstructionService->selectData(data);
}

QString UsReconstructionServiceProxy::getSelectedFilename() const
{
	return mUsReconstructionService->getSelectedFilename();
}

USReconstructInputData UsReconstructionServiceProxy::getSelectedFileData()
{
	return mUsReconstructionService->getSelectedFileData();
}

DataAdapterPtr UsReconstructionServiceProxy::getParam(QString uid)
{
	return mUsReconstructionService->getParam(uid);
}

//ReconstructParamsPtr UsReconstructionServiceProxy::getParams()
//{
//	return mUsReconstructionService->getParams();
//}

std::vector<DataAdapterPtr> UsReconstructionServiceProxy::getAlgoOptions()
{
	return mUsReconstructionService->getAlgoOptions();
}

XmlOptionFile UsReconstructionServiceProxy::getSettings()
{
	return mUsReconstructionService->getSettings();
}

OutputVolumeParams UsReconstructionServiceProxy::getOutputVolumeParams() const
{
	return mUsReconstructionService->getOutputVolumeParams();
}

void UsReconstructionServiceProxy::setOutputVolumeParams(const OutputVolumeParams &par)
{
	mUsReconstructionService->setOutputVolumeParams(par);
}

void UsReconstructionServiceProxy::setOutputRelativePath(QString path)
{
	mUsReconstructionService->setOutputRelativePath(path);
}

void UsReconstructionServiceProxy::setOutputBasePath(QString path)
{
	mUsReconstructionService->setOutputBasePath(path);
}

void UsReconstructionServiceProxy::startReconstruction()
{
	mUsReconstructionService->startReconstruction();
}

std::set<TimedAlgorithmPtr> UsReconstructionServiceProxy::getThreadedReconstruction()
{
	return mUsReconstructionService->getThreadedReconstruction();
}

ReconstructionMethodService* UsReconstructionServiceProxy::createAlgorithm()
{
	return mUsReconstructionService->createAlgorithm();
}

ReconstructCore::InputParams UsReconstructionServiceProxy::createCoreParameters()
{
	return mUsReconstructionService->createCoreParameters();
}

void UsReconstructionServiceProxy::newDataOnDisk(QString mhdFilename)
{
	return mUsReconstructionService->newDataOnDisk(mhdFilename);
}

} //cx
