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

#ifndef SSCPRESETTRANSFERFUNCTIONS3D_H_
#define SSCPRESETTRANSFERFUNCTIONS3D_H_

#include <QDomElement>
#include "sscImage.h"
#include "sscXmlOptionItem.h"

class QString;
class QStringList;
class QDomDocument;

namespace ssc
{

typedef boost::shared_ptr<class PresetTransferFunctions3D> PresetTransferFunctions3DPtr;

/**
 * \date 11. juni 2010
 * \author: jbake
 */

/**\brief Handles transfer function presets
 *
 * \ingroup sscData
 */
class PresetTransferFunctions3D : public QObject
{
	Q_OBJECT
public:
	PresetTransferFunctions3D(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile);
	~PresetTransferFunctions3D();

	void save(QString name, ssc::ImagePtr image, bool _2D=true, bool _3D=true);
	void load(QString name, ssc::ImagePtr image, bool _2D=true, bool _3D=true);

	void save2D(QString name, ssc::ImagePtr image);
	void save3D(QString name, ssc::ImagePtr image);
	void load2D(QString name, ssc::ImagePtr image);
	void load3D(QString name, ssc::ImagePtr image);

	QStringList getPresetList(QString modality=""); ///< returns a list of the preset names for the given modality
	bool isDefaultPreset(QString presetName); ///< Check is the preset is one of the "system presets"
	void deletePresetData(QString name, bool _2D=true, bool _3D=true); ///< Delete the preset data node

signals:
	void changed();

private:
	QStringList generatePresetList(QString modality); ///< internally generate the preset list
	ssc::XmlOptionFile getPresetNode(const QString& presetName);
	ssc::XmlOptionFile getCustomFile();

	ssc::XmlOptionFile mPresetFile;
	ssc::XmlOptionFile mCustomFile;
	QDomElement mLastReturnedPreset;
};

}//namespace cx

#endif /* SSCPRESETTRANSFERFUNCTIONS3D_H_ */
