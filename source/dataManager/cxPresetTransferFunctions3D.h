#ifndef PRESETTRANSFERFUNCTIONS3D_H_
#define PRESETTRANSFERFUNCTIONS3D_H_

#include <QDomElement>
#include "sscImage.h"
#include "sscXmlOptionItem.h"

class QString;
class QStringList;
class QDomDocument;

namespace cx
{

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
  PresetTransferFunctions3D();
  ~PresetTransferFunctions3D();

  void save(QString name, ssc::ImagePtr image);
  void load(QString name, ssc::ImagePtr image);

  QStringList getPresetList(); ///< returns a list of the preset names
  //ssc::Image::ShadingStruct getShadingPresets(const QString& presetName);///< Returns a struct with specific shading presets
  //void save(QString name, ssc::ImagePtr image);

private:
//  QDomElement& getPresetDomElement(const QString& presetName); ///< returns a specific presets QDomElement representation
  QStringList generatePresetList(); ///< internally generate the preset list
//  void initializeDomDocument(); ///< internally populate the dom document with the available presets
//  QDomElement& getDefaultPresetDomElement(); ///< returns a default preset
  ssc::XmlOptionFile getPresetNode(const QString& presetName);
  ssc::XmlOptionFile getCustomFile();

  //QDomDocument* mPresetDomDocument; ///< the xml document with the presets
  std::map<QString, ssc::Image::ShadingStruct> mShadings;// Map with the shading presets
  ssc::XmlOptionFile mPresetFile;
  QDomElement mLastReturnedPreset;
};
}//namespace cx

#endif /* PRESETTRANSFERFUNCTIONS3D_H_ */
