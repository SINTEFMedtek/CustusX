#include "cxTSFPresets.h"

#ifdef CX_USE_TSF
#include <QDir>
#include <QTextStream>
#include "sscMessageManager.h"
#include "cxDataLocations.h"

namespace cx {

//TSFPresets::TSFPresets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile) :
TSFPresets::TSFPresets() :
	Presets(ssc::XmlOptionFile(), ssc::XmlOptionFile())
{
//	std::cout << "TSFPresets()" << std::endl;
	mPresetPath = cx::DataLocations::getTSFPath()+"/parameters";
	std::map<QString, QString> presetsMap = this->loadPresetsFromFiles();

	this->convertToInternalFormat(presetsMap);
}

QStringList TSFPresets::generatePresetList(QString tag)
{
	QStringList retval;
	std::map<QString, QString>::iterator it;
	for(it = mPresetsMap.begin(); it != mPresetsMap.end(); ++it)
	{
		retval << it->first;
	}
	return retval;
}

std::map<QString, QString> TSFPresets::loadPresetsFromFiles()
{
//	std::cout << "TSFPresets::loadPresetsFromFiles()" << std::endl;
	mPresetsMap.clear();

	QDir parametersDir(mPresetPath);
	if(!parametersDir.exists())
		return mPresetsMap;

	//get all folders in the presetpath
	QStringList subDirs;
	QFileInfoList infoList = parametersDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo info, infoList)
	{
		subDirs << info.baseName();
//		std::cout << info.baseName().toStdString() << std::endl;
	}

	//for each folder get all files
	foreach(const QString dir, subDirs)
	{
		if(parametersDir.cd(dir))
		{
			//for each file generate name based on part of the foldername and the filename: CPU - airways, GPU - airways etc...
			QFileInfoList infoList = parametersDir.entryInfoList(QDir::Files);
			foreach(QFileInfo info, infoList)
			{
				QString name = info.dir().dirName() + ": " + info.baseName();
//				std::cout << name.toStdString() << std::endl;
				mPresetsMap[name] = info.absoluteFilePath();
			}
			parametersDir.cdUp();
		}
	}

	return mPresetsMap;
}

void TSFPresets::convertToInternalFormat(std::map<QString, QString>& presets)
{
//	std::cout << "TSFPresets::convertToInternalFormat(std::map<QString, QString> presets) " << presets.size() << std::endl;
	std::map<QString, QString>::iterator it;
	for(it = presets.begin(); it != presets.end(); ++it)
	{
		QDomElement preset = this->convertToXml(it->second);
		Presets::addCustomPreset(it->first, preset);
		//Debugging
//		this->print(preset);
	}
}

QDomElement TSFPresets::convertToXml(QString filePath)
{
	QDomDocument doc;
	QDomElement retval = ssc::XmlOptionFile().getElement();

	//QString absFilePath = mPresetPath+"/"+filePath;

	std::map<QString, QString> params = this->readFile(filePath);
	std::map<QString, QString>::iterator it;
	for(it = params.begin(); it != params.end(); ++it)
	{
		QDomElement child = doc.createElement("Paramenter");
		child.setAttribute(it->first, it->second);
		retval.appendChild(child);
	}

	return retval;
}

std::map<QString, QString> TSFPresets::readFile(QString& filePath)
{
	std::map<QString, QString> retval;

	if(!QFile::exists(filePath))
	{
		ssc::messageManager()->sendError("File does not exists: " + filePath);
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
	        if(lineItems.count() == 2)
	        {
//				std::cout << lineItems.at(0).toStdString() << " " << lineItems.at(1).toStdString() << std::endl;
				retval[lineItems.at(0)] = lineItems.at(1);
	        }
	    }
	}
	return retval;
}

void TSFPresets::print(QDomElement& element)
{
//	std::cout << "TSFPresets::print(QDomElement element)" << std::endl;
	 // print out the element names of all elements that are direct children
	 // of the outermost element.
//	 QDomElement docElem = doc.documentElement();
//	std::cout << "element has childnodes? " << element.hasChildNodes() << std::endl;

	 QDomNode n = element.firstChild();
	 while(!n.isNull()) {
	     QDomElement e = n.toElement(); // try to convert the node to an element.
	     if(!e.isNull()) {
	         std::cout << qPrintable(e.tagName()) << std::endl; // the node really is an element.
	     }
	     n = n.nextSibling();
	 }
}

} /* namespace cx */

#endif //CX_USE_TSF
