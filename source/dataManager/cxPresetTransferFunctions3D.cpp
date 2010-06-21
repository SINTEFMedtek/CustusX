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
  return mShadings[string_cast(presetName)];
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
  //========== ct - fire ==========
  QDomElement presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);

  presetElement.setAttribute("name", "CT - Fire");

  // Add alpha points
  QDomElement alphaNode = mPresetDomDocument->createElement("alpha");
  QStringList pointStringList;
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("100=100"));
  pointStringList.append(QString("150=100"));
  pointStringList.append(QString("200=200"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  QDomElement colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("150=255/255/0"));
  pointStringList.append(QString("200=255/0/0"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["CT - Fire"].on = true;
  mShadings["CT - Fire"].ambient = 0.2;
  mShadings["CT - Fire"].diffuse = 0.9;
  mShadings["CT - Fire"].specular = 0.3;
  mShadings["CT - Fire"].specularPower = 15.0;
  
  //========== ct - blue ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  
  presetElement.setAttribute("name", "CT - Blue");
  
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("0=0"));
  pointStringList.append(QString("200=50"));
  pointStringList.append(QString("300=255"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("0=0/0/0"));
  pointStringList.append(QString("200=255/255/0"));
  pointStringList.append(QString("300=0/0/255"));
  colorNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(colorNode);
  
  // Add shading parameters
  mShadings["CT - Blue"].on = true;
  mShadings["CT - Blue"].ambient = 0.2;
  mShadings["CT - Blue"].diffuse = 0.9;
  mShadings["CT - Blue"].specular = 0.3;
  mShadings["CT - Blue"].specularPower = 15.0;
  
  //========== ct - soft tissue standard ==========
  presetElement = mPresetDomDocument->createElement("Preset");
  mPresetDomDocument->appendChild(presetElement);
  
  presetElement.setAttribute("name", "Soft Tissue CT - Standard");
  
  // Add alpha points
  alphaNode = mPresetDomDocument->createElement("alpha");
  pointStringList.append(QString("12=0"));
  pointStringList.append(QString("202=7"));//6.885"));
  pointStringList.append(QString("404=34"));//33.915"));
  pointStringList.append(QString("549=174"));//173.91"));
  alphaNode.appendChild(mPresetDomDocument->createTextNode(pointStringList.join(" ")));
  pointStringList.clear();
  presetElement.appendChild(alphaNode);
  
  // Add color points
  colorNode = mPresetDomDocument->createElement("color");
  pointStringList.append(QString("12=0/0/0"));
  pointStringList.append(QString("202=255/0/0"));
  pointStringList.append(QString("404=255/255/0"));
  pointStringList.append(QString("549=255/255/255"));
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
