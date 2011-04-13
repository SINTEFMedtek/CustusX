#include "cxPresetTransferFunctions3D.h"

#include <iostream>
#include <vtkColorTransferFunction.h>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QColor>
#include "sscTypeConversions.h"
#include "cxDataLocations.h"
#include "sscXmlOptionItem.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include <sscImageTFData.h>

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
  ssc::ImageTF3DPtr transferFunctions = image->getTransferFunctions3D();
  ssc::ImageLUT2DPtr LUT2D = image->getLookupTable2D();
	ssc::XmlOptionFile node = this->getPresetNode(name);

	transferFunctions->parseXml(node.getElement().namedItem("transferfunctions"));
	LUT2D->parseXml(node.getElement().namedItem("lookuptable2D"));

	ssc::Image::ShadingStruct shading = image->getShading();
	shading.parseXml(node.getElement().namedItem("shading"));
	image->setShading(shading);

	this->fixTransferFunctions(transferFunctions, image);
	this->fixTransferFunctions(LUT2D, image);
}

void PresetTransferFunctions3D::fixTransferFunctions(ssc::ImageTFDataPtr trFunc, ssc::ImagePtr image)
{
  //Make sure min and max values for transferfunctions are set

  ssc::OpacityMapPtr opacityMap = trFunc->getOpacityMap();
  ssc::ColorMapPtr colorMap = trFunc->getColorMap();

  vtkColorTransferFunctionPtr interpolatedTrFunc = vtkColorTransferFunctionPtr::New();
  trFunc->fillColorTFFromMap(interpolatedTrFunc);

  if (opacityMap->find(image->getMin()) == opacityMap->end())
  {
    trFunc->addAlphaPoint(image->getMin(), 0);
  }
  if (opacityMap->find(image->getMax()) == opacityMap->end())
  {
    //The optimal solution may be to interpolate/extrapolate the max (min) values from the existing values
    //However, as most presets usually have all the top values set to white the error of the simpler code below is usually small
    ssc::IntIntMap::iterator opPoint = opacityMap->end();
    opPoint--;
    trFunc->addAlphaPoint(image->getMax(), opPoint->second);// Use value of current max element
  }
  if (colorMap->find(image->getMin()) == colorMap->end())
  {
    trFunc->addColorPoint(image->getMin(), QColor(0,0,0));
  }
  if (colorMap->find(image->getMax()) == colorMap->end())
  {
    //Interpolate to get correct color
    double* rgb = interpolatedTrFunc->GetColor(image->getMax());
    QColor newColor = QColor(int(rgb[0]*255), int(rgb[1]*255), int(rgb[2]*255));
    trFunc->addColorPoint(image->getMax(), newColor);
  }

  //Remove transfer function points outside range
  ssc::IntIntMap::iterator opIt = trFunc->getOpacityMap()->begin();
  while (opIt != trFunc->getOpacityMap()->end())
  {
    int delPoint = 1000000;
    if(opIt->first < image->getMin())
      delPoint = opIt->first;
    else if (opIt->first > image->getMax())
      delPoint = opIt->first;
    opIt++;

    if (delPoint != 1000000)
      trFunc->removeAlphaPoint(delPoint);
  }

  ssc::ColorMap::iterator it = trFunc->getColorMap()->begin();
   while (it != trFunc->getColorMap()->end())
   {
     int delPoint = 1000000;
     if(it->first < image->getMin())
       delPoint = it->first;
     else if (it->first > image->getMax())
       delPoint = it->first;
     it++;

     if (delPoint != 1000000)
       trFunc->removeColorPoint(delPoint);
   }
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
