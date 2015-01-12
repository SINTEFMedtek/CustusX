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

#include "cxUsReconstructionServiceNull.h"
#include "cxPropertyNull.h"
#include "cxUSReconstructInputData.h"

namespace cx {

UsReconstructionServiceNull::UsReconstructionServiceNull()
{}

void UsReconstructionServiceNull::printWarning() const
{
//	reportWarning("Trying to use UsReconstructionServiceNull. Is UsReconstructionService (org.custusx.usreconstruction) disabled?");
}

void UsReconstructionServiceNull::selectData(QString filename, QString calFilesPath)
{
	printWarning();
}

void UsReconstructionServiceNull::selectData(USReconstructInputData data)
{
	printWarning();
}

QString UsReconstructionServiceNull::getSelectedFilename() const
{
	printWarning();
	return QString();
}

USReconstructInputData UsReconstructionServiceNull::getSelectedFileData()
{
	printWarning();
	return USReconstructInputData();
}

DataAdapterPtr UsReconstructionServiceNull::getParam(QString uid)
{
	printWarning();
	return DataAdapterNull::create();
}

//ReconstructParamsPtr UsReconstructionServiceNull::getParams()
//{
//	printWarning();
//	return ReconstructParamsPtr();
//}

std::vector<DataAdapterPtr> UsReconstructionServiceNull::getAlgoOptions()
{
	printWarning();
	return std::vector<DataAdapterPtr>();
}

XmlOptionFile UsReconstructionServiceNull::getSettings()
{
	printWarning();
	return XmlOptionFile();
}

OutputVolumeParams UsReconstructionServiceNull::getOutputVolumeParams() const
{
	printWarning();
	return OutputVolumeParams();
}

void UsReconstructionServiceNull::setOutputVolumeParams(const OutputVolumeParams &par)
{
	printWarning();
}

void UsReconstructionServiceNull::setOutputRelativePath(QString path)
{
	printWarning();
}

void UsReconstructionServiceNull::setOutputBasePath(QString path)
{
	printWarning();
}

void UsReconstructionServiceNull::startReconstruction()
{
	printWarning();
}

std::set<TimedAlgorithmPtr> UsReconstructionServiceNull::getThreadedReconstruction()
{
	printWarning();
	return std::set<TimedAlgorithmPtr>();
}

ReconstructionMethodService *UsReconstructionServiceNull::createAlgorithm()
{
	printWarning();
	return NULL;
}

ReconstructCore::InputParams UsReconstructionServiceNull::createCoreParameters()
{
	printWarning();
	return ReconstructCore::InputParams();
}

bool UsReconstructionServiceNull::isNull()
{
	return true;
}

void UsReconstructionServiceNull::newDataOnDisk(QString mhdFilename)
{
	printWarning();
}

} //cx
