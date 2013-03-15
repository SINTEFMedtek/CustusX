#ifndef CXTSFPRESETS_H_
#define CXTSFPRESETS_H_

#include <map>
#include <QDomElement>

#include "sscPresets.h"
#include "sscXmlOptionItem.h"

namespace cx {

/*
 * \class TSFPresets
 *
 * \brief Defines a set of presets for the tubesegmentation filter
 *
 * \date Mar 14, 2013
 * \author Janne Beate Bakeng, SINTEF
 */

class TSFPresets : public ssc::Presets
{
	Q_OBJECT

public:
//	TSFPresets(ssc::XmlOptionFile presetFile, ssc::XmlOptionFile customFile);
	TSFPresets();
	virtual ~TSFPresets(){};

private:
	virtual QStringList generatePresetList(QString tag); ///< internally generate the preset list
	std::map<QString, QString> loadPresetsFromFiles();
	void convertToInternalFormat(std::map<QString, QString>& presets);
	QDomElement convertToXml(QString filePath);
	std::map<QString, QString> readFile(QString& filePath);

	//debugging
	void print(QDomElement& element);

	QString mPresetPath;
	std::map<QString, QString> mPresetsMap;
};
typedef boost::shared_ptr<class TSFPresets> TSFPresetsPtr;

} /* namespace cx */
#endif /* CXTSFPRESETS_H_ */
