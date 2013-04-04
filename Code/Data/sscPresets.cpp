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
#include "sscMessageManager.h"

namespace ssc {

Presets::Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;
	mLastCustomPresetName = "";
}

void Presets::addCustomPreset(QDomElement& element)
{
	mLastCustomPresetName = element.firstChild().toElement().nodeName();
	std::cout << "mLastCustomPresetName " << mLastCustomPresetName.toStdString() << std::endl;
	this->addPreset(mCustomFile, element);
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

void Presets::addDefaultPreset(QDomElement& element)
{
	this->addPreset(mPresetFile, element);
}

void Presets::addPreset(ssc::XmlOptionFile& file, QDomElement& element)
{
	//TODO
	std::cout << "TODO refactor void Presets::addPreset(ssc::XmlOptionFile& file, QDomElement& element)!!!" << std::endl;

	file = file.descend("Presets");
	if(file.getElement().isNull())
	{
		messageManager()->sendError("The XmlOptionFile we are trying to add a preset to is null.");
		return;
	}

	QDomElement presetElement = file.getElement();
	file.removeChildren();
	presetElement.appendChild(element);
}

} /* namespace ssc */
