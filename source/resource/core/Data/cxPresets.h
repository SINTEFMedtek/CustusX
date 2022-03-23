/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXPRESETS_H_
#define CXPRESETS_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <QObject>

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "cxXmlOptionItem.h"
#include "cxDefinitions.h"

namespace cx {

/**
 * \class Presets
 *
 * \brief Base class for a group of presets in the system.
 *
 * A preset node should look like this:
 * <Preset name="example" valuename1="value1" valuename2="value2" ... valuenameN="valueN"/>
 *
 * \ingroup cx_resource_core_data
 * \date Mar 11, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT Presets : public QObject
{
	Q_OBJECT

public:
	Presets(XmlOptionFile presetFile, XmlOptionFile customFile); //used
	virtual ~Presets(){};

	IMAGE_MODALITY getId() const;

	void addCustomPreset(QDomElement& element); ///< adds a custom preset
	virtual void deleteCustomPreset(QString name); ///< deletes the custom preset with the given name if it exists
	virtual void save(); ///< saves the presets to file
	virtual void remove(); ///< removes the presets from file

	QStringList getPresetList(IMAGE_MODALITY tag=imUNKNOWN); ///< returns a list of the preset names for the given tag
	bool isDefaultPreset(QString presetName); ///< Check is the preset is one of the "system presets"
	XmlOptionFile getCustomFile();

signals:
	void changed(); ///<

protected:
	virtual QStringList generatePresetList(IMAGE_MODALITY tag); ///< internally generate the preset list
	XmlOptionFile getPresetNode(const QString& presetName); ///< Look for a preset with the given name. Create one if not found.
	void addDefaultPreset(QDomElement& element); //used
	void addPreset(XmlOptionFile& file, QDomElement& element); //used

	//debugging
	void print(QDomElement element); ///< debugging function for printing xml elements

	QString mLastCustomPresetAdded; /// < the name of the last custom preset added
	QString mLastCustomPresetRemoved; /// < the name of the last custom preset removed

	XmlOptionFile mPresetFile; //used
private:
	XmlOptionFile mCustomFile; //used
	IMAGE_MODALITY mId;
};

typedef boost::shared_ptr<class Presets> PresetsPtr;

} /* namespace cx */
#endif /* CXPRESETS_H_ */
