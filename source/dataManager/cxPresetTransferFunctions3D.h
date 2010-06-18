#ifndef PRESETTRANSFERFUNCTIONS3D_H_
#define PRESETTRANSFERFUNCTIONS3D_H_

#include <QDomElement>
#include "sscImage.h"

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

  QStringList getPresetList(); ///< returns a list of the preset names
  QDomElement& getPresetDomElement(const QString& presetName); ///< returns a specific presets QDomElement representation
  ssc::Image::shadingStruct getShadingPresets(const QString& presetName);///< Returns a struct with specific shading presets

private:
  QStringList generatePresetList(); ///< internally generate the preset list
  void initializeDomDocument(); ///< internally populate the dom document with the available presets
  QDomElement& getDefaultPresetDomElement(); ///< returns a default preset

  QDomDocument* mPresetDomDocument; ///< the xml document with the presets
  std::map<std::string, ssc::Image::shadingStruct> mShadings;// Map with the shading presets

  QDomElement mLastReturnedPreset;
};
}//namespace cx

#endif /* PRESETTRANSFERFUNCTIONS3D_H_ */
