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

#include "cxTSFPresets.h"

#include <iostream>
#include <QDir>
#include <QTextStream>
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "tsf-config.h"

namespace cx
{

TSFPresets::TSFPresets() :
        Presets(XmlOptionFile("TSFPresets"), XmlOptionFile("Custom"))
{
	mPresetPath = cx::DataLocations::findConfigFolder("/tsf", QString(KERNELS_DIR)) + "/parameters";
	this->loadPresetsFromFiles();
}

TSFPresets::~TSFPresets()
{}

QDomElement TSFPresets::createPresetElement(QString name, std::map<QString, QString>& parameters)
{
	QStringList ignoreParametersWithName;
	ignoreParametersWithName << "centerline-vtk-file";
	ignoreParametersWithName << "storage-dir";

	QDomDocument doc;
	QDomElement retval = doc.createElement("Preset");
	retval.setAttribute("name", name);
	std::map<QString, QString>::iterator it;
	for (it = parameters.begin(); it != parameters.end(); ++it)
	{
		if (!ignoreParametersWithName.contains(it->first))
			retval.setAttribute(it->first, it->second);
	}
	return retval;
}

void TSFPresets::save()
{
	QDomDocument doc = this->getCustomFile().getDocument();
	QDomNodeList presetNodes = doc.elementsByTagName("Preset");
	for (int i = 0; i < presetNodes.count(); ++i)
	{
		QDomNode node = presetNodes.at(i);
		if (!node.isElement())
			break;
		QDomElement element = node.toElement();

		QString folderPath;
		folderPath = mPresetPath + "/centerline-gpu/";
		std::map<QString, QString> parameters;
		QDomNamedNodeMap attributes = element.attributes();
		for (int j = 0; j < attributes.count(); ++j)
		{
			QDomNode attribute = attributes.item(j);
			if (attribute.isNull())
					continue;
			if (attribute.nodeName() != "name")
				parameters[attribute.nodeName()] = attribute.nodeValue();
		}
		if (mLastCustomPresetAdded == element.attribute("name")) //Save only current preset
			this->saveFile(folderPath, parameters);
	}
}

void TSFPresets::remove()
{
	QString folderPath = mPresetPath + "/centerline-gpu/" + mLastCustomPresetRemoved;
	this->deleteFile(folderPath);
}

QStringList TSFPresets::generatePresetList(QString tag)
{
	this->getPresetsNameAndPath();
	QStringList retval;
	std::map<QString, QString>::iterator it;
	for (it = mPresetsMap.begin(); it != mPresetsMap.end(); ++it)
	{
		retval << it->first;
	}
	return retval;
}

void TSFPresets::loadPresetsFromFiles()
{
	this->getPresetsNameAndPath();
	this->convertToInternalFormat(mPresetsMap);
}

void TSFPresets::convertToInternalFormat(std::map<QString,QString>& presets)
{
	std::map<QString,QString>::iterator it;
	for (it = presets.begin();it != presets.end();++it)
		this->addAsCustomPreset(it);

}

std::map<QString, QString> TSFPresets::readFile(QString& filePath)
{
	std::map<QString, QString> retval;
	if (!QFile::exists(filePath))
	{
		reportError("File does not exists: " + filePath);
		return retval;
	}
	QFile file(filePath);
	if (file.open(QFile::ReadOnly))
	{
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			QStringList lineItems = line.split(" ");
			if (lineItems.count() == 2)
				retval[lineItems.at(0)] = lineItems.at(1);
		}
	}

	return retval;
}

void TSFPresets::saveFile(QString folderpath, std::map<QString, QString> parameters)
{
	QFile file(folderpath + mLastCustomPresetAdded);
	QString customPresetName = QFileInfo(file).fileName();
	QTextStream outPresetFile;
	if (!file.open(QFile::WriteOnly))
	{
		reportError("Could not open the file " + file.fileName() + " for writing.");
		return;
	}
	outPresetFile.setDevice(&file);
	std::map<QString, QString>::iterator it;
	for (it = parameters.begin(); it != parameters.end(); ++it)
	{
		QString line = it->first + " " + it->second;
		outPresetFile << line << "\n";
	}
	outPresetFile << flush;
	file.close();
}

void TSFPresets::deleteFile(QString filePath)
{
	QFile file(filePath);
	QString customPresetName = QFileInfo(file).fileName();
	if (!file.remove())
		reportError("File: " + filePath + " not removed...");
}

void TSFPresets::getPresetsNameAndPath()
{
	mPresetsMap.clear();
	QDir parametersDir(mPresetPath + "/centerline-gpu");
	if (!parametersDir.exists())
		reportError("Preset directory "+parametersDir.path()+" not found.");

	QFileInfoList fileInfoList = parametersDir.entryInfoList(QDir::Files);
	foreach(QFileInfo info, fileInfoList){
		QString name = info.baseName();
		mPresetsMap[name] = info.absoluteFilePath();
	}
}

void TSFPresets::addAsCustomPreset(std::map<QString,QString>::iterator it)
{
	std::map<QString,QString> params = this->readFile(it->second);
	QDomElement preset = TSFPresets::createPresetElement(it->first, params);
	Presets::addCustomPreset(preset);
}

} /* namespace cx */

