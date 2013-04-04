#ifndef CXTSFPRESETS_H_
#define CXTSFPRESETS_H_

#include <map>
#include <QDomElement>

#include "sscPresets.h"
#include "sscXmlOptionItem.h"

#ifdef CX_USE_TSF

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
	TSFPresets();
	virtual ~TSFPresets(){};

	static QDomElement createPresetElement(QString name, std::map<QString, QString>& parameters);
	virtual void save(); ///< saves the newly added presets to file

private:
//	QDomElement mapToQDomElement(std::map<QString, QString> map); ///< converts a map of parameter names and values into a xml element
	virtual QStringList generatePresetList(QString tag); ///< internally generate the preset list
	void loadPresetsFromFiles(); ///< loads all preset files in a given location into the system
	void convertToInternalFormat(std::map<QString, QString>& presets); ///< converts a map of names and filepaths into the internal default list of presets
//	QDomElement convertToXml(QString filePath); ///< converts a preset file into a xml element
	std::map<QString, QString> readFile(QString& filePath); ///< converts a parameter file into a map of parameter names and values
	void saveFile(QString folderpath, std::map<QString, QString> parameters); ///< saves the last preset into a file in the given folder with the given parameters, also updates the parameterfile

	//debugging
	void print(QDomElement element); ///< debugging function for printing xml elements

	QString mPresetPath; ///< path to the location where presets can be found and saved
	std::map<QString, QString> mPresetsMap; ///< map of currently available presets
};
typedef boost::shared_ptr<class TSFPresets> TSFPresetsPtr;

} /* namespace cx */

#endif // CX_USE_TSF

#endif /* CXTSFPRESETS_H_ */
