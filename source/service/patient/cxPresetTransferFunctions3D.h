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

private:
  QStringList generatePresetList(); ///< internally generate the preset list
  ssc::XmlOptionFile getPresetNode(const QString& presetName);
  ssc::XmlOptionFile getCustomFile();

  ssc::XmlOptionFile mPresetFile;
  QDomElement mLastReturnedPreset;
};

}//namespace cx

#endif /* PRESETTRANSFERFUNCTIONS3D_H_ */
