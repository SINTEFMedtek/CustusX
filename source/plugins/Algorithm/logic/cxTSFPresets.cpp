#include "cxTSFPresets.h"

#ifdef CX_USE_TSF
#include <QDir>
#include <QTextStream>
#include "sscMessageManager.h"
#include "cxDataLocations.h"

namespace cx {

TSFPresets::TSFPresets() :
	Presets(ssc::XmlOptionFile(), ssc::XmlOptionFile())
{
	mPresetPath = cx::DataLocations::getTSFPath()+"/parameters";
	this->loadPresetsFromFiles();
}

void TSFPresets::save()
{
	ssc::XmlOptionFile customs = this->getCustomFile();
	QDomDocument doc = customs.getDocument();
	QDomNodeList presetNodes = doc.elementsByTagName("Preset");
	for (int i = 0; i < presetNodes.count(); ++i)
	{
		QDomNode node = presetNodes.at(i);

		if(!node.isElement())
			break;

		QString filepath;
		std::map<QString, QString> parameters;
		QDomNodeList childNodes = node.childNodes();
		for (int i = 0; i < childNodes.count(); ++i)
		{
			QDomNode child = childNodes.at(i);
			if(child.toElement().tagName() == "centerline-method")
				filepath = mPresetPath + "/centerline-"+child.firstChild().toText().data()+"/";
			else
				parameters[child.toElement().tagName()] = child.firstChild().toText().data();
		}
		if(!QFile::exists(filepath))
			continue;
		this->saveFile(filepath+mLastCustomPresetName, parameters);
	}
}

QDomElement TSFPresets::mapToQDomElement(std::map<QString, QString> map)
{
	QDomDocument doc;
	QDomElement element = ssc::XmlOptionFile().getElement();
	std::map<QString, QString>::iterator it;
	for(it = map.begin(); it != map.end(); ++it)
	{
		QDomElement child = doc.createElement("Preset");
		child.setAttribute(it->first, it->second);
		element.appendChild(child);
	}
	return element;
}

QStringList TSFPresets::generatePresetList(QString tag)
{
	this->loadPresetsFromFiles();
	QStringList retval;
	std::map<QString, QString>::iterator it;
	for(it = mPresetsMap.begin(); it != mPresetsMap.end(); ++it)
	{
		retval << it->first;
	}
	return retval;
}

void TSFPresets::loadPresetsFromFiles()
{
	mPresetsMap.clear();

	QDir parametersDir(mPresetPath);
	if(!parametersDir.exists())
		return;

	//get all folders in the presetpath
	QStringList subDirs;
	QFileInfoList infoList = parametersDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
	foreach(QFileInfo info, infoList)
		subDirs << info.baseName();

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
				mPresetsMap[name] = info.absoluteFilePath();
			}
			parametersDir.cdUp();
		}
	}
	this->convertToInternalFormat(mPresetsMap);
}

void TSFPresets::convertToInternalFormat(std::map<QString, QString>& presets)
{
	std::map<QString, QString>::iterator it;
	for(it = presets.begin(); it != presets.end(); ++it)
	{
		QDomElement preset = this->convertToXml(it->second);
		Presets::addDefaultPreset(it->first, preset);
	}
}

QDomElement TSFPresets::convertToXml(QString filePath)
{
	std::map<QString, QString> params = this->readFile(filePath);
	QDomElement retval = this->mapToQDomElement(params);

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
				retval[lineItems.at(0)] = lineItems.at(1);
	    }
	}
	return retval;
}

void TSFPresets::saveFile(QString filepath, std::map<QString, QString> parameters)
{
	QFile file(filepath);
	if(file.exists())
	{
		filepath+="(1)";
		this->saveFile(filepath, parameters);
	}

	//-------------------------------------------------------------
	//Need to add the name of the new file into the parameters file
	// ..../parameters/parameters
	//-------------------------------------------------------------
	QString parametersFile = mPresetPath+"/parameters";
	QFile paramFile(parametersFile);
	if(!paramFile.exists())
	{
		ssc::messageManager()->sendError("The file " + parametersFile + " does not exist.");
		return;
	}

	//Read the content
	if (!paramFile.open(QFile::ReadOnly))
	{
		ssc::messageManager()->sendError("Could not open the file " + parametersFile + " for reading.");
		return;
	}
	QTextStream inText;
	inText.setDevice(&paramFile);
	QString allText = inText.readAll();
	paramFile.close();

	QString customPresetName = QFileInfo(file).fileName();
	if(!allText.contains(customPresetName, Qt::CaseInsensitive))
	{
		QString searchString = "parameters str none";
		int index = allText.indexOf(searchString);
		index += searchString.size();
		allText.insert(index, " "+customPresetName);

		//Write the new content
		if (!paramFile.open(QFile::WriteOnly | QFile::Truncate))
		{
			ssc::messageManager()->sendError("Could not open the file " + parametersFile + " for writing.");
			return;
		}
		QTextStream outParametersFile(&paramFile);
		outParametersFile << allText;
		outParametersFile << flush;
		paramFile.close();
	}

	//-------------------------------------------------------------
	//Save the new parameters file
	// ..../parameters/<centerline-method>/<preset-name>
	//-------------------------------------------------------------
	QTextStream outPresetFile;
	if (!file.open(QFile::WriteOnly))
	{
		ssc::messageManager()->sendError("Could not open the file " + parametersFile + " for writing.");
		return;
	}

	outPresetFile.setDevice(&file);
	std::map<QString, QString>::iterator it;
	for(it=parameters.begin(); it != parameters.end(); ++it){
		QString line = it->first + " " + it->second;
		outPresetFile << line << "\n";
	}
	outPresetFile << flush;

	file.close();

}

void TSFPresets::print(QDomElement element)
{
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
