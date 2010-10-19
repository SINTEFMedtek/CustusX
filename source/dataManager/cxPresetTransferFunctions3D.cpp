#include "cxPresetTransferFunctions3D.h"

#include <iostream>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include "sscTypeConversions.h"

namespace cx
{
PresetTransferFunctions3D::PresetTransferFunctions3D() :
    mPresetDomDocument(new QDomDocument("PresetTransferFunctions3D"))
{
  this->initializeDomDocument();
  //std::cout << "QDomDoc: " << mPresetDomDocument->toString().toStdString() << std::endl;
}

PresetTransferFunctions3D::~PresetTransferFunctions3D()
{
  delete mPresetDomDocument;
}

QStringList PresetTransferFunctions3D::getPresetList()
{
  return this->generatePresetList();
}

QDomElement& PresetTransferFunctions3D::getPresetDomElement(const QString& presetName)
{
  QDomNodeList presetNodeList = mPresetDomDocument->elementsByTagName("Preset");
  for(int i=0; i < presetNodeList.count(); ++i)
  {
    QString name = presetNodeList.item(i).toElement().attribute("name");
    if(presetName == name)
    {
      std::cout << "Found presetelement with attribute " << presetName.toStdString() << std::endl;
      mLastReturnedPreset = presetNodeList.item(i).toElement();
      return mLastReturnedPreset;
    }
  }

  mLastReturnedPreset = this->getDefaultPresetDomElement();
  return mLastReturnedPreset;
}

ssc::Image::shadingStruct PresetTransferFunctions3D::getShadingPresets(const QString& presetName)
{
  return mShadings[presetName];
}
  
QStringList PresetTransferFunctions3D::generatePresetList()
{
  QStringList presetList;
  presetList.append("Transfer function preset...");

  QDomNodeList presetNodeList = mPresetDomDocument->elementsByTagName("Preset");
  for(int i=0; i < presetNodeList.count(); ++i)
  {
    QString presetName = presetNodeList.item(i).toElement().attribute("name");
    if(presetName == "Default")
      continue;
    else
      presetList << presetName;
  }
  return presetList;
}

void PresetTransferFunctions3D::initializeDomDocument()
{
  //========== ct - abdomen ==========
  QDomElement presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Abdomen");
  // Add alpha points
  QDomElement alphaNode = mPresetDomDocument->createElement("alpha");
  QStringList pointStringList;
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("690=0"));
  pointStringList.append(QString("1069=0"));
  pointStringList.append(QString("1206=138"));
  pointStringList.append(QString("1342=255"));
  pointStringList.append(QString("2528=255"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  QDomElement colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Abdomen"].on = true;
  mShadings["CT Abdomen"].ambient = 0.2;
  mShadings["CT Abdomen"].diffuse = 0.54;
  mShadings["CT Abdomen"].specular = 0.72;
  mShadings["CT Abdomen"].specularPower = 15.0;

  //========== ct - bone ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Bone");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("690=0"));
  pointStringList.append(QString("1144=0"));
  pointStringList.append(QString("1254=255"));
  pointStringList.append(QString("2528=255"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Bone"].on = true;
  mShadings["CT Bone"].ambient = 0.2;
  mShadings["CT Bone"].diffuse = 0.54;
  mShadings["CT Bone"].specular = 0.72;
  mShadings["CT Bone"].specularPower = 15.0;

  //========== ct - lung + bone ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Lung + bone");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=128"));
  pointStringList.append(QString("552=0"));
  pointStringList.append(QString("1159=0"));
  pointStringList.append(QString("1254=255"));
  pointStringList.append(QString("2528=255"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=102/192/255"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Lung + bone"].on = true;
  mShadings["CT Lung + bone"].ambient = 0.2;
  mShadings["CT Lung + bone"].diffuse = 0.54;
  mShadings["CT Lung + bone"].specular = 0.72;
  mShadings["CT Lung + bone"].specularPower = 15.0;

  //========== ct - lung ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Lung");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=70"));
  pointStringList.append(QString("552=70"));
  pointStringList.append(QString("2528=0"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=102/192/255"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Lung"].on = true;
  mShadings["CT Lung"].ambient = 0.2;
  mShadings["CT Lung"].diffuse = 0.54;
  mShadings["CT Lung"].specular = 0.72;
  mShadings["CT Lung"].specularPower = 15.0;

  //========== ct - aorta ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Aorta");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("300=0"));
  pointStringList.append(QString("1106=1"));
  pointStringList.append(QString("1201=85"));
  pointStringList.append(QString("1390=1"));
  pointStringList.append(QString("1473=2"));
  pointStringList.append(QString("2528=0"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=144/203/255"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Aorta"].on = true;
  mShadings["CT Aorta"].ambient = 0.2;
  mShadings["CT Aorta"].diffuse = 0.54;
  mShadings["CT Aorta"].specular = 0.72;
  mShadings["CT Aorta"].specularPower = 15.0;

  //========== ct - Lung + aorta ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Lung + aorta");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=30"));
  pointStringList.append(QString("300=0"));
  pointStringList.append(QString("1106=1"));
  pointStringList.append(QString("1228=156"));
  pointStringList.append(QString("1232=189"));
  pointStringList.append(QString("1390=1"));
  pointStringList.append(QString("1473=2"));
  pointStringList.append(QString("2528=0"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=144/203/255"));
  pointStringList.append(QString("1133=255/0/0"));
  pointStringList.append(QString("1168=255/154/73"));
  pointStringList.append(QString("1416=255/255/255"));
  pointStringList.append(QString("2528=0/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Lung + aorta"].on = true;
  mShadings["CT Lung + aorta"].ambient = 0.2;
  mShadings["CT Lung + aorta"].diffuse = 0.54;
  mShadings["CT Lung + aorta"].specular = 0.72;
  mShadings["CT Lung + aorta"].specularPower = 15.0;

  //========== ct - Airways ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  presetElement.setAttribute("name", "CT Airways");
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("362=0"));
  pointStringList.append(QString("698=147"));
  pointStringList.append(QString("926=0"));
  pointStringList.append(QString("4095=0"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("1960=0/255/255"));
  pointStringList.append(QString("2725=255/0/0"));
  pointStringList.append(QString("4095=255/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(
      pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  // Add shading parameters
  mShadings["CT Airways"].on = true;
  mShadings["CT Airways"].ambient = 0.2;
  mShadings["CT Airways"].diffuse = 0.54;
  mShadings["CT Airways"].specular = 0.72;
  mShadings["CT Airways"].specularPower = 15.0;
  //========== ct - fire ==========
  /*QDomElement presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);

  presetElement.setAttribute("name", "CT - Fire");

  // Add alpha points
  QDomElement alphaNode = mPresetDomDocument->createElement("alpha");
  QStringList pointStringList;
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("990=0"));
  pointStringList.append(QString("1133=1"));
  pointStringList.append(QString("1400=200"));
  pointStringList.append(QString("2000=200"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  QDomElement colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("1170=255/0/0"));
  pointStringList.append(QString("1280=255/255/0"));
  pointStringList.append(QString("2000=255/255/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["CT - Fire"].on = true;
  mShadings["CT - Fire"].ambient = 0.2;
  mShadings["CT - Fire"].diffuse = 0.9;
  mShadings["CT - Fire"].specular = 0.3;
  mShadings["CT - Fire"].specularPower = 15.0;*/
  
  //========== ct - blue ==========
  /*presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  
  presetElement.setAttribute("name", "CT - Blue");
  
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("920=0"));
  pointStringList.append(QString("1245=2"));
  pointStringList.append(QString("1920=200"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("910=255/255/0"));
  pointStringList.append(QString("1610=0/0/255"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["CT - Blue"].on = true;
  mShadings["CT - Blue"].ambient = 0.5;
  mShadings["CT - Blue"].diffuse = 0.9;
  mShadings["CT - Blue"].specular = 0.3;
  mShadings["CT - Blue"].specularPower = 2.0;*/
  
  //========== ct - soft tissue standard ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  
  presetElement.setAttribute("name", "Soft Tissue CT - Standard");
  
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("1024=0"));
  pointStringList.append(QString("1226=7"));//6.885"));
  pointStringList.append(QString("1428=34"));//33.915"));
  pointStringList.append(QString("1573=174"));//173.91"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("1036=0/0/0"));
  pointStringList.append(QString("1226=255/0/0"));
  pointStringList.append(QString("1428=255/255/0"));
  pointStringList.append(QString("1573=255/255/255"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["Soft Tissue CT - Standard"].on = true;
  mShadings["Soft Tissue CT - Standard"].ambient = 0.15;
  mShadings["Soft Tissue CT - Standard"].diffuse = 0.90;
  mShadings["Soft Tissue CT - Standard"].specular = 0.30;
  mShadings["Soft Tissue CT - Standard"].specularPower = 15.00;
  
  //========== default ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  
  presetElement.setAttribute("name", "Default");
  //presetName = mPresetDomDocument->createElement("name");
  //presetElement.appendChild(presetName);
  //presetName.appendChild(mPresetDomDocument->createTextNode("Default"));
  
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  //pointStringList.append(QString("0=0"));
  //pointStringList.append(QString("200=50"));
  //pointStringList.append(QString("1000=255"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  //pointStringList.append(QString("0=0/0/0"));
  //pointStringList.append(QString("200=255/255/0"));
  //pointStringList.append(QString("1000=0/0/255"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["Default"].on = true;
  mShadings["Default"].ambient = 0.2;
  mShadings["Default"].diffuse = 0.9;
  mShadings["Default"].specular = 0.3;
  mShadings["Default"].specularPower = 15.0;
  
}

QDomElement& PresetTransferFunctions3D::getDefaultPresetDomElement()
{
  mLastReturnedPreset = mPresetDomDocument->elementsByTagName("Default").item(0).toElement();
  return mLastReturnedPreset;
}

}
