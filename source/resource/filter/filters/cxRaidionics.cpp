/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxRaidionics.h"
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "cxDataLocations.h"
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxVisServices.h"
#include "cxFileManagerService.h"
#include "cxData.h"
#include "cxEnumConversion.h"

namespace cx
{

Raidionics::Raidionics(VisServicesPtr services, CommandStringVariables variables, QStringList targets) :
	mServices(services),
	mVariables(variables),
	mTargets(targets)
{}

QString Raidionics::raidionicsCommandString()
{
	QString raidionicsIni = Raidionics::createRaidionicsIniFile();

	QString commandString = mVariables.envPath;

	commandString.append(" " + mVariables.scriptFilePath);
	commandString.append(" " + raidionicsIni);
//	commandString.append(" " + variables.cArguments);
	return commandString;
}

QString Raidionics::getOutputFolder()
{
	return mOutputFolder + "/" + subfolderT0();
}

QString Raidionics::getTempFolder()
{
	return mTempFolder;
}

QString Raidionics::createRaidionicsIniFile()
{
	mTempFolder = DataLocations::getCachePath() + "/Raidionics_temp/";
	CX_LOG_DEBUG() << "Creating Raidionics temp folder: " << mTempFolder;
	QDir().mkpath(mTempFolder);

	QString iniFilePath = mTempFolder + getIniFileName();
	QString jsonFilePath = mTempFolder + getJsonFileName();
	createRaidionicsJasonFile(jsonFilePath);

	mOutputFolder = mTempFolder + "output/";
	QString modelFolder = getModelFolder();
	QDir().mkpath(mOutputFolder);
	QDir().mkpath(modelFolder);

	QString inputFolder = copyInputFiles(mVariables.inputFilePath, subfolderT0());

	QSettings settings(iniFilePath, QSettings::IniFormat);

	settings.beginGroup("Default");
	settings.setValue("task", "mediastinum_diagnosis");
	settings.setValue("trace", "False");
	settings.setValue("caller", "CustusX");
	settings.endGroup();

	settings.beginGroup("System");
	settings.setValue("gpu_id", "-1");
	settings.setValue("input_folder", inputFolder);
	settings.setValue("output_folder", mOutputFolder);
	settings.setValue("model_folder", modelFolder);
	settings.setValue("pipeline_filename", jsonFilePath);
	settings.endGroup();

	settings.beginGroup("Runtime");
	settings.setValue("non_overlapping", "False");
	settings.setValue("reconstruction_method", "thresholding");
	settings.setValue("reconstruction_order", "resample_first");
	settings.endGroup();

//	settings.beginGroup("Neuro");
//	settings.endGroup();
//	settings.beginGroup("Mediastinum");
//	settings.endGroup();

	return iniFilePath;
}

QString Raidionics::copyInputFiles(QString inputFileName, QString subfolder)
{
	QString inputFolder = mTempFolder + "input/";

	QString fullInputFolder = inputFolder + "/" + subfolder + "/";
	QDir().mkpath(fullInputFolder);

	QFile inputFile(inputFileName);
	QFileInfo inputFileInfo(inputFile);

	std::vector<DataPtr> datas = mServices->file()->read(inputFileName);
	if(datas.size() == 1)
		mServices->file()->save(datas[0], fullInputFolder+getRadionicsInputFileName(inputFileInfo.fileName()));
	else
		CX_LOG_WARNING() << "Raidionics::copyInputFiles: Got " << datas.size() << " input files. Expecting 1";

	return inputFolder;
}

QString Raidionics::getRadionicsInputFileName(QString inputFile)
{
	QFileInfo inputFileInfo(inputFile);
	return getRaidionicsCTFileNamePrefix()+inputFileInfo.fileName();
}

QString Raidionics::getModelFolder()
{
	return DataLocations::getModelsPath() + "/raidionics_models";
}

//The ordering of the Qt generated json file is alphabetoical, and will not match the json example file
void Raidionics::createRaidionicsJasonFile(QString jsonFilePath)
{
	//CX_LOG_DEBUG() << "createRaidionicsJasonFile: " << jsonFilePath;

	QFile jsonFile(jsonFilePath);
	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);

	QJsonObject rootObject;

	QString sequence = "High-resolution"; //For task=mediastinum_diagnosis
//	QString sequence = "T1-CE"; //For task=neuro_diagnosis

	for(int i = 0; i < mTargets.size(); ++i)
	{
		QString target = mTargets[i];

		QJsonObject taskObject;
		QJsonObject inputObject;
		QJsonObject spaceObject;
		QJsonObject number0Object;
		inputObject.insert("timestamp", 0);
		inputObject.insert("sequence", sequence);
		inputObject.insert("labels", QJsonValue::Null);
		spaceObject.insert("timestamp", 0);
		spaceObject.insert("sequence", sequence);
		inputObject.insert("space", spaceObject);

		taskObject.insert("task", "Segmentation");
		if(useFormatThresholding(target))
			taskObject.insert("format", "thresholding");
		number0Object.insert("0", inputObject);
		taskObject.insert("inputs", number0Object);
		QJsonArray targetArray = createTargetArray(target);
		taskObject.insert("target", targetArray);
		taskObject.insert("model", "CT_"+target);
		taskObject.insert("description", targetDescription(target)+" segmentation in "+sequence+" ("+subfolderT0()+")");
		QString taskNumber;
		taskNumber.setNum(i+1);
		rootObject.insert(taskNumber, taskObject);
	}

	QJsonDocument jsonDoc(rootObject);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();
}

bool Raidionics::useFormatThresholding(QString target)
{
	if(target == enum2string(lsLUNG))
		return true;
	return false;
}

QStringList Raidionics::createTargetList(QString target)
{
	QStringList targets;

	LUNG_MODELS model = string2enum<LUNG_MODELS> (target);
	if(model == lmCOUNT)
		targets << target;
	else if(model == lmMEDIUM_ORGANS_MEDIASTINUM)
		targets << getTarget(otVENA_CAVA) << getTarget(otAORTIC_ARCH) << getTarget(otASCENDING_AORTA) << getTarget(otDESCENDING_AORTA) << getTarget(otSPINE);
	else if(model == lmPULMSYST_HEART)
		targets << getTarget(otHEART) << getTarget(otPULMONARY_VEINS) << getTarget(otPULMONARY_TRUNK);
	else if(model == lmSMALL_ORGANS_MEDIASTINUM)
		targets << getTarget(otBRACHIO_CEPHALIC_VEINS) << getTarget(otSUBCLAVIAN_ARTERY) << getTarget(otAZYGOS) <<	getTarget(otESOPHAGUS);

	return targets;
}

QString Raidionics::getTarget(ORGAN_TYPE organType)
{
	QString target = enum2string(organType);
	if(organType == otSUBCLAVIAN_ARTERY)
		target = "SubCarArt";
	return target;
}

ORGAN_TYPE Raidionics::getOrganType(QString target)
{
	ORGAN_TYPE organType = string2enum<ORGAN_TYPE>(target);
	if(target == "SubCarArt")
		organType = otSUBCLAVIAN_ARTERY;
	return organType;
}

QString Raidionics::targetDescription(QString target)
{
	QStringList targets = createTargetList(target);
	QString retval = targets.join(", ");
	return retval;
}

QJsonArray Raidionics::createTargetArray(QString target)
{
	QStringList targets = createTargetList(target);
	QJsonArray targetArray;
	for(int i = 0; i < targets.size(); ++i)
		targetArray.push_back(targets[i]);
	return targetArray;
}

QStringList Raidionics::expandOutputClasses(QStringList targetList)
{
	QStringList retval;
	for(int i = 0; i < targetList.size(); ++i)
	{
		QString target = targetList[i];
		QStringList targets = createTargetList(target);
		retval << targets;
	}
	return retval;
}

QStringList Raidionics::setOutputClasses(QStringList outputClasses)
{
	mTargets = outputClasses;
	return this->expandOutputClasses(mTargets);
}

QString Raidionics::colorForLungClass(QString outputClass)
{
	QString color = "255,0,0";

	ORGAN_TYPE target = Raidionics::getOrganType(outputClass);
	switch (target)
	{
	case otAIRWAYS:
		color = "254,175,180,43";break;
	case otLUNGS:
		color = "254,175,180,43";break;
	case otLYMPH_NODES:
		color = "0,255,0,255";break;
	case otVENA_CAVA:
		color = "153,153,255,255";break;
	case otAORTIC_ARCH:
		color = "255,127,127,255";break;
	case otASCENDING_AORTA:
		color = "255,127,127,255";break;
	case otDESCENDING_AORTA:
		color = "255,127,127,255";break;
	case otSPINE:
		color = "255,255,255,255";break;
	case otHEART:
		color = "254,128,204,128";break;
	case otPULMONARY_VEINS:
		color = "131,105,79,255";break;
	case otPULMONARY_TRUNK:
		color = "229,179,255,255";break;
	case otBRACHIO_CEPHALIC_VEINS:
		color = "153,153,255,255";break;
	case otSUBCLAVIAN_ARTERY:
		color = "255,127,127,255";break;
	case otAZYGOS:
		color = "153,153,255,255";break;
	case otESOPHAGUS:
		color = "170,85,0,255";break;
	default:
//	otUNKNOWN,
//	organtypeCOUNT
		CX_LOG_WARNING() << "GenericScriptFilter::colorForClass(): No color found for " << enum2string(target) << " (Converted from string: " << outputClass << "). Setting color to red";
		break;
	}
	return color;
}

}//cx
