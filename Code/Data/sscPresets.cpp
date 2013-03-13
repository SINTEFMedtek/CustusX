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

namespace ssc {

Presets::Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile)
{
	mPresetFile = presetFile;
	mCustomFile = customFile;
}

ssc::XmlOptionFile Presets::saveCustom(QString presetName, std::map<QString, QString> attributes, std::vector<QDomElement> children)
{
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", presetName);

	QDomElement presetElement = file.getElement("Preset");
	std::map<QString, QString>::iterator it;
	for(it=attributes.begin(); it!= attributes.end(); ++it)
	{
		presetElement.setAttribute(it->first, it->second);
	}

	//delete old children
	while (presetElement.hasChildNodes())
		presetElement.removeChild(presetElement.firstChild());

	//add new children
	std::vector<QDomElement>::iterator it2;
	for(it2 = children.begin(); it2 != children.end(); ++it2)
	{
		file.getDocument().appendChild(*it2);
	}

	file.save();
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
