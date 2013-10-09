#include "cxTSFPresets.h"

#ifdef CX_USE_TSF
#include <iostream>
#include <QDir>
#include <QTextStream>
#include "sscMessageManager.h"
#include "cxDataLocations.h"

namespace cx
{

TSFPresets::TSFPresets() :
		Presets(XmlOptionFile("Preset"), XmlOptionFile("Custom"))
{
	mPresetPath = cx::DataLocations::getTSFPath() + "/parameters";
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
		std::map<QString, QString> parameters;
		QDomNamedNodeMap attributes = element.attributes();
		for (int i = 0; i < attributes.count(); ++i)
		{
			QDomNode attribute = attributes.item(i);
			if (attribute.nodeName() != "name")
				parameters[attribute.nodeName()] = attribute.nodeValue();
			if (attribute.nodeName() == "centerline-method")
				folderPath = mPresetPath + "/centerline-gpu/";
		}
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
	this->loadPresetsFromFiles();
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
		messageManager()->sendError("File does not exists: " + filePath);
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
		messageManager()->sendError("Could not open the file " + file.fileName() + " for writing.");
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
		messageManager()->sendError("File: " + filePath + " not removed...");
}

void TSFPresets::getPresetsNameAndPath()
{
	mPresetsMap.clear();
	QDir parametersDir(mPresetPath + "/centerline-gpu");
	if (!parametersDir.exists())
		messageManager()->sendError("Preset directory "+parametersDir.path()+" not found.");

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

#endif //CX_USE_TSF
