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
 * \date Mar 11, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class Presets  : public QObject
{
	Q_OBJECT

public:
	Presets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile);
	virtual ~Presets(){};

	ssc::XmlOptionFile saveCustom(QString presetName, std::map<QString, QString> attributes, std::vector<QDomElement> children);

signals:
	void changed();

protected:
	ssc::XmlOptionFile getCustomFile();
	ssc::XmlOptionFile getPresetNode(const QString& presetName); ///< Look for a preset with the given name. Create one if not found.

	ssc::XmlOptionFile mPresetFile;
private:
	ssc::XmlOptionFile mCustomFile;
};

typedef boost::shared_ptr<class Presets> PresetsPtr;

} /* namespace ssc */
#endif /* SSCPRESETS_H_ */
