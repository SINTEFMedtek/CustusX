#include "cxPresetTransferFunctions3D.h"

#include <iostream>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QColor>
#include "sscTypeConversions.h"
#include "cxDataLocations.h"
#include "sscXmlOptionItem.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"

namespace cx
{

PresetTransferFunctions3D::PresetTransferFunctions3D()
{
	mPresetFile = ssc::XmlOptionFile(DataLocations::getRootConfigPath()+"/transferFunctions/presets.xml", "transferFunctions"); ///< create from filename, create trivial document of type name and root node if no file exists.
}

PresetTransferFunctions3D::~PresetTransferFunctions3D()
{
}

QStringList PresetTransferFunctions3D::getPresetList()
{
  return this->generatePresetList();
}

ssc::XmlOptionFile PresetTransferFunctions3D::getCustomFile()
{
	return ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(),"CustusX").descend("presetTransferFunctions");
}


void PresetTransferFunctions3D::save(QString name, ssc::ImagePtr image)
{
	ssc::XmlOptionFile file = this->getCustomFile();
	file = file.descend("Preset", "name", name);

	QDomNode tf3DNode = file.getElement("transferfunctions");
	while (tf3DNode.hasChildNodes())
	  tf3DNode.removeChild(tf3DNode.firstChild());
  QDomNode tf2DNode = file.getElement("lookuptable2D");
  while (tf2DNode.hasChildNodes())
    tf2DNode.removeChild(tf2DNode.firstChild());

  image->getTransferFunctions3D()->addXml(file.getElement("transferfunctions"));
  image->getLookupTable2D()->addXml(file.getElement("lookuptable2D"));
	image->getShading().addXml(file.getElement("shading"));

	file.save();
}

void PresetTransferFunctions3D::load(QString name, ssc::ImagePtr image)
{
	ssc::XmlOptionFile node = this->getPresetNode(name);

	image->getTransferFunctions3D()->parseXml(node.getElement().namedItem("transferfunctions"));
	image->getLookupTable2D()->parseXml(node.getElement().namedItem("lookuptable2D"));

	ssc::Image::ShadingStruct shading = image->getShading();
	shading.parseXml(node.getElement().namedItem("shading"));
	image->setShading(shading);

//  //Make sure min and max values for transferfunctions are set
//  transferFunctions->addAlphaPoint(image->getMin(), 0);
//  transferFunctions->addAlphaPoint(image->getMax(), 255);
//  transferFunctions->addColorPoint(image->getMin(), QColor(0,0,0));
//  transferFunctions->addColorPoint(image->getMax(), QColor(1,1,1));
}

/** look for a preset with the given name. Create one if not found.
 *
 */
ssc::XmlOptionFile PresetTransferFunctions3D::getPresetNode(const QString& presetName)
{
  ssc::XmlOptionFile retval = mPresetFile;
  retval = retval.tryDescend("Preset", "name", presetName);
  if (!retval.getDocument().isNull())
    return retval;

  retval = this->getCustomFile();
  retval = retval.descend("Preset", "name", presetName);
  return retval;
}
  
QStringList PresetTransferFunctions3D::generatePresetList()
{
  QStringList presetList;
  presetList.append("Transfer function preset...");

  QDomNodeList presetNodeList = mPresetFile.getElement().elementsByTagName("Preset");
  for(int i=0; i < presetNodeList.count(); ++i)
  {
    QString presetName = presetNodeList.item(i).toElement().attribute("name");
    if(presetName == "Default")
      continue;
    else
      presetList << presetName;
  }

  ssc::XmlOptionFile customFile = this->getCustomFile();
  presetNodeList = customFile.getElement().elementsByTagName("Preset");
  for(int i=0; i < presetNodeList.count(); ++i)
  {
    QString presetName = presetNodeList.item(i).toElement().attribute("name");
    presetList << presetName;
  }

  return presetList;
}


}
