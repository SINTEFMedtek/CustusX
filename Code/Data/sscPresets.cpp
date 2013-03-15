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
}

void Presets::addCustomPreset(QString name, QDomElement& element)
{
//	std::cout << "Presets::addCustomPreset(QString name, QDomElement element)" << std::endl;
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", name);

	QDomElement presetElement = file.getElement("Preset");

	//TODO add attributes
//	std::map<QString, QString>::iterator it;
//	for(it=attributes.begin(); it!= attributes.end(); ++it)
//	{
//		presetElement.setAttribute(it->first, it->second);
//	}

	//delete old children
	while (presetElement.hasChildNodes())
		presetElement.removeChild(presetElement.firstChild());

	//add element
	presetElement.appendChild(element);

	file.save();
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

} /* namespace ssc */
