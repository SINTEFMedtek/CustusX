/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

PropertyPtr UsReconstructionServiceNull::getParam(QString uid)
{
	printWarning();
	return PropertyNull::create();
}

//ReconstructParamsPtr UsReconstructionServiceNull::getParams()
//{
//	printWarning();
//	return ReconstructParamsPtr();
//}

std::vector<PropertyPtr> UsReconstructionServiceNull::getAlgoOptions()
{
	printWarning();
	return std::vector<PropertyPtr>();
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
