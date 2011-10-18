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
 * \class PresetTransferFunctions3D.h
 *
 * \brief Handles 3D transfer function presets
 *
 * \date 11. juni 2010
 * \author: jbake
 */
class PresetTransferFunctions3D
{
public:
	PresetTransferFunctions3D(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile);
	~PresetTransferFunctions3D();

	void save(QString name, ssc::ImagePtr image);
	void load(QString name, ssc::ImagePtr image);

	QStringList getPresetList(QString modality); ///< returns a list of the preset names for the given modality
	bool isDefaultPreset(QString presetName); ///< Check is the preset is one of the "system presets"
	void deletePresetData(QString name); ///< Delete the preset data node

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
