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

namespace cx
{

Raidionics::Raidionics(CommandStringVariables variables, QStringList targets) :
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
	settings.setValue("task", "neuro_diagnosis");
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
	inputFile.copy(fullInputFolder+inputFileInfo.fileName());

	if (inputFileName.contains(".mhd"))
	{
		QString fileNameRaw = inputFileName.left(inputFileName.lastIndexOf("."))+".raw";
		QFile inputRawFile(fileNameRaw);
		QFileInfo inputRawFileInfo(fileNameRaw);
		inputRawFile.copy(fullInputFolder+inputRawFileInfo.fileName());
	}
	return inputFolder;
}

QString Raidionics::getModelFolder()
{
	return profile()->getPath() + "/raidionics_models";
}

//The ordering of the Qt generated json file is alphabetoical, and will not match the json example file
void Raidionics::createRaidionicsJasonFile(QString jsonFilePath)
{
	//CX_LOG_DEBUG() << "createRaidionicsJasonFile: " << jsonFilePath;

	QFile jsonFile(jsonFilePath);
	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);

	QJsonObject rootObject;

	for(int i = 0; i < mTargets.size(); ++i)
	{
		QJsonObject taskObject;
		QJsonObject inputObject;
		QJsonObject spaceObject;
		QJsonObject number0Object;
		inputObject.insert("timestamp", 0);
		inputObject.insert("sequence", "T1-CE");
		inputObject.insert("labels", QJsonValue::Null);
		spaceObject.insert("timestamp", 0);
		spaceObject.insert("sequence", "T1-CE");
		inputObject.insert("space", spaceObject);

		taskObject.insert("task", "Segmentation");
		number0Object.insert("0", inputObject);
		taskObject.insert("inputs", number0Object);
		QJsonArray targetArray;
		targetArray.push_back(mTargets[i]);
		taskObject.insert("target", targetArray);//TODO: Need more than one target in array?
		taskObject.insert("model", "CT_"+mTargets[i]);
		taskObject.insert("description", mTargets[i]+" segmentation in T1CE ("+subfolderT0()+")");
		QString taskNumber;
		taskNumber.setNum(i+1);
		rootObject.insert(taskNumber, taskObject);
	}

	QJsonDocument jsonDoc(rootObject);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();
}

}//cx
