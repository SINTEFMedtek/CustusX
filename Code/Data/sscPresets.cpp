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

#include <iostream>
#include <QStringList>
#include <QTextStream>
#include "sscMessageManager.h"

namespace ssc {

Presets::Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;
	mLastCustomPresetAdded = "";
}

void Presets::deleteCustomPreset(QString name)
{
	mLastCustomPresetRemoved = name;
	ssc::XmlOptionFile node = this->getPresetNode(name);
	node.deleteNode();

	//emit changed(); TODO???
}

void Presets::addCustomPreset(QDomElement& element)
{
	mLastCustomPresetAdded = element.attribute("name");
	this->addPreset(mCustomFile, element);

	//emit changed(); TODO???
}

void Presets::save()
{
	this->getCustomFile().save();
}

void Presets::remove()
{
	//TODO
	std::cout << "TODO IMPLEMENT: void Presets::remove()" << std::endl;
}

QStringList Presets::getPresetList(QString tag)
{
	return this->generatePresetList(tag);
}

bool Presets::isDefaultPreset(QString presetName)
{

	ssc::XmlOptionFile testval = mPresetFile.tryDescend("Preset", "name", presetName);
	if (!testval.isNull())
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
	QString presetName = element.attribute("name");
	if(presetName.isEmpty())
		return;

	file = file.root();
	file = file.descend("Presets");
	file.getElement().appendChild(element);

	//TODO what if preset with name already exists?
}


void Presets::print(QDomElement element)
{
	QTextStream stream(stdout);
	stream << "\n";
	element.save(stream, 4);
	stream << "\n";
}


} /* namespace ssc */
