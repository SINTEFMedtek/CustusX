// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscPresets.h"

#include <QStringList>

namespace ssc {

Presets::Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;
	mLastCustomPresetName = "";
}

void Presets::addCustomPreset(QString name, QDomElement& element)
{
	mLastCustomPresetName = name;
	this->addPreset(mCustomFile, name, element);
}

void Presets::save()
{
	this->getCustomFile().save();
}

QStringList Presets::getPresetList(QString tag)
{
	return this->generatePresetList(tag);
}

bool Presets::isDefaultPreset(QString presetName)
{
	ssc::XmlOptionFile testval = mPresetFile.tryDescend("Preset", "name", presetName);
	if (!testval.getDocument().isNull())
		return true;
	return false;
}

QStringList Presets::generatePresetList(QString tag)
{
	return QStringList();
}

ssc::XmlOptionFile Presets::getCustomFile()
{
	return mCustomFile;
}

ssc::XmlOptionFile Presets::getPresetNode(const QString& presetName)
{
	ssc::XmlOptionFile retval = mPresetFile;
	retval = retval.tryDescend("Preset", "name", presetName);
	if (!retval.getDocument().isNull())
		return retval;

	retval = this->getCustomFile();
	retval = retval.descend("Preset", "name", presetName);
	return retval;
}

void Presets::addDefaultPreset(QString name, QDomElement& element)
{
	this->addPreset(mPresetFile, name, element);
}

void Presets::addPreset(ssc::XmlOptionFile& file, QString name, QDomElement& element)
{
	file = file.descend("Preset");

	if(file.getElement().isNull())
		std::cout << "file is null" << std::endl;

	QDomElement presetElement = file.getElement();

	//DEBUG
	/*
	QDomNode n = element.firstChild();
	while(!n.isNull())  {
	    QDomElement e = n.toElement(); // try to convert the node to an element.
	    if(!e.isNull())  {
	        std::cout << qPrintable(e.tagName()) << std::endl; // the node really is an element.
	    }
	    n = n.nextSibling();
	}
	*/
	//DEBUG

	//delete old children
	file.removeChildren();

	//add element
	presetElement.appendChild(element);
}

} /* namespace ssc */
