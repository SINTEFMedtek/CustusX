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

namespace cx
{

Raidionics::Raidionics()
{}

QString Raidionics::raidionicsCommandString(CommandStringVariables variables)
{
	//TODO:
	// - Create Python wrapper as we have done for deepSintef

	//Example code for running raidionics with the example setup
	//The output isn't automatically imported into CustusX yet

	//The raidionics virtual environment can be setup with the script: cxCreateRaidionicsVenv.sh

	QString raidionicsIni = Raidionics::createRaidionicsIniFile(variables);

	QString commandString = variables.envPath;
	commandString.append(" " + variables.scriptFilePath);
	commandString.append(" " + raidionicsIni);
//	commandString.append(" " + variables.cArguments);
	return commandString;
}

QString Raidionics::createRaidionicsIniFile(CommandStringVariables variables)
{
	QString tempFolder = DataLocations::getCachePath() + "/Raidionics_temp/";
	CX_LOG_DEBUG() << "Creating Raidionics temp folder: " << tempFolder;
	QDir().mkpath(tempFolder);

	QString iniFilePath = tempFolder + "Raidionics.ini";
//	QString iniFilePath = variables.scriptFilePath;
	QString jsonFilePath = tempFolder + "Raidionics.json";
	createRaidionicsJasonFile(jsonFilePath);

	QString inputFolder = tempFolder + "input/";
	QString outputFolder = tempFolder + "output/";
	QString modelFolder = tempFolder + "model/";//TODO: Don't use a temp folder for models
	QDir().mkpath(inputFolder);
	QDir().mkpath(outputFolder);
	QDir().mkpath(modelFolder);


	QString inputFileName(variables.inputFilePath);
	QFile inputFile(inputFileName);
	QFileInfo inputFileInfo(variables.inputFilePath);
	inputFile.copy(inputFolder+inputFileInfo.fileName());

	if (inputFileName.contains(".mhd"))
	{
		QString fileNameRaw = inputFileName.left(inputFileName.lastIndexOf("."))+".raw";
		QFile inputRawFile(fileNameRaw);
		QFileInfo inputRawFileInfo(fileNameRaw);
		inputRawFile.copy(inputFolder+inputRawFileInfo.fileName());
	}

	QSettings settings(iniFilePath, QSettings::IniFormat);

	settings.beginGroup("Default");
	settings.setValue("task", "neuro_diagnosis");
	settings.setValue("trace", "False");
	settings.setValue("caller", "CustusX");
	settings.endGroup();

	settings.beginGroup("System");
	settings.setValue("gpu_id", "1");
	settings.setValue("input_folder", inputFolder);//Example. Actually need to copy volume and convert to T0/*_gd.nii.gz
	settings.setValue("output_folder", outputFolder);
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

//The ordering of the Qt generated json file is alphabetoical, and will not match the json example file
void Raidionics::createRaidionicsJasonFile(QString jsonFilePath)
{
	//CX_LOG_DEBUG() << "createRaidionicsJasonFile: " << jsonFilePath;

	QFile jsonFile(jsonFilePath);
	jsonFile.open(QIODevice::WriteOnly | QIODevice::Text);

	QJsonObject rootObject;

	QStringList targets;
	targets << "Lungs" << "Airways";

	for(int i = 0; i < targets.size(); ++i)
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
		targetArray.push_back(targets[i]);
		taskObject.insert("target", targetArray);//TODO: Need more than one target in array?
		taskObject.insert("model", "CT_"+targets[i]);
		taskObject.insert("description", targets[i]+" segmentation in T1CE (T0)");
		QString taskNumber;
		taskNumber.setNum(i+1);
		rootObject.insert(taskNumber, taskObject);
	}

	QJsonDocument jsonDoc(rootObject);
	jsonFile.write(jsonDoc.toJson());
	jsonFile.close();
}

}//cx
