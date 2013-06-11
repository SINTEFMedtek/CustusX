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
	virtual ~TSFPresets();

	static QDomElement createPresetElement(QString name, std::map<QString,QString>& parameters);
	virtual void save();
	virtual void remove();

protected:
	virtual QStringList generatePresetList(QString tag);
	void loadPresetsFromFiles();
	void convertToInternalFormat(std::map<QString,QString>& presets);
	std::map<QString,QString> readFile(QString& filePath);
	void saveFile(QString folderPath, std::map<QString,QString> parameters);
	void deleteFile(QString filePath);
	void getPresetsNameAndPath();
	void addAsCustomPreset(std::map<QString,QString>::iterator it);

	QString mPresetPath;
	std::map<QString,QString> mPresetsMap;
};
typedef boost::shared_ptr<class TSFPresets> TSFPresetsPtr;

} /* namespace cx */

#endif // CX_USE_TSF

#endif /* CXTSFPRESETS_H_ */
