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

#ifndef SSCPRESETS_H_
#define SSCPRESETS_H_

#include <QObject>

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "sscXmlOptionItem.h"

namespace ssc {

/*
 * \class Presets
 *
 * \brief Base class for a group of presets in the system.
 *
 * A preset node should look like this:
 * <Preset name="example" valuename1="value1" valuename2="value2" ... valuenameN="valueN"/>
 *
 * \date Mar 11, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class Presets : public QObject
{
	Q_OBJECT

public:
	Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile); //used
	virtual ~Presets(){};

	void addCustomPreset(QDomElement& element); ///< adds a custom preset
	virtual void deleteCustomPreset(QString name); ///< deletes the custom preset with the given name if it exists
	virtual void save(); ///< saves the presets to file
	virtual void remove(); ///< removes the presets from file

	QStringList getPresetList(QString tag=""); ///< returns a list of the preset names for the given tag
	bool isDefaultPreset(QString presetName); ///< Check is the preset is one of the "system presets"
	ssc::XmlOptionFile getCustomFile();

signals:
	void changed(); ///<

protected:
	virtual QStringList generatePresetList(QString tag); ///< internally generate the preset list
	ssc::XmlOptionFile getPresetNode(const QString& presetName); ///< Look for a preset with the given name. Create one if not found.
	void addDefaultPreset(QDomElement& element); //used
	void addPreset(ssc::XmlOptionFile& file, QDomElement& element); //used

	QString mLastCustomPresetAdded; /// < the name of the last custom preset added
	QString mLastCustomPresetRemoved; /// < the name of the last custom preset removed

	ssc::XmlOptionFile mPresetFile; //used
private:
	ssc::XmlOptionFile mCustomFile; //used
};

typedef boost::shared_ptr<class Presets> PresetsPtr;

} /* namespace ssc */
#endif /* SSCPRESETS_H_ */
