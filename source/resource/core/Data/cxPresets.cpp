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


#include "cxPresets.h"

#include <iostream>
#include <QStringList>
#include <QTextStream>


namespace cx {

Presets::Presets(XmlOptionFile presetFile, XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;
	mLastCustomPresetAdded = "";
    mId = mPresetFile.getFileName();
}

void Presets::deleteCustomPreset(QString name)
{
	mLastCustomPresetRemoved = name;
	XmlOptionFile node = this->getPresetNode(name);
	node.deleteNode();
}

void Presets::addCustomPreset(QDomElement& element)
{
	mLastCustomPresetAdded = element.attribute("name");
	this->addPreset(mCustomFile, element);
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

	XmlOptionFile testval = mPresetFile.tryDescend("Preset", "name", presetName);
	if (!testval.isNull())
		return true;
	return false;
}

QStringList Presets::generatePresetList(QString tag)
{
	return QStringList();
}

XmlOptionFile Presets::getCustomFile()
{
	return mCustomFile;
}

XmlOptionFile Presets::getPresetNode(const QString& presetName)
{
	XmlOptionFile retval = mPresetFile;
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

void Presets::addPreset(XmlOptionFile& file, QDomElement& element)
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

QString Presets::getId() const
{
    return mId;
}


} /* namespace cx */
