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


#ifndef CXPRESETS_H_
#define CXPRESETS_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <QObject>

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "cxXmlOptionItem.h"

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

    QString getId() const;

	void addCustomPreset(QDomElement& element); ///< adds a custom preset
	virtual void deleteCustomPreset(QString name); ///< deletes the custom preset with the given name if it exists
	virtual void save(); ///< saves the presets to file
	virtual void remove(); ///< removes the presets from file

	QStringList getPresetList(QString tag=""); ///< returns a list of the preset names for the given tag
	bool isDefaultPreset(QString presetName); ///< Check is the preset is one of the "system presets"
	XmlOptionFile getCustomFile();

signals:
	void changed(); ///<

protected:
	virtual QStringList generatePresetList(QString tag); ///< internally generate the preset list
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
    QString mId;
};

typedef boost::shared_ptr<class Presets> PresetsPtr;

} /* namespace cx */
#endif /* CXPRESETS_H_ */
