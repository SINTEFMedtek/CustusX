/*
 *  UsConfig.cpp
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 3/11/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */

#include "UsConfig.h"
#include <qstring.h>
#include <qfile.h>
#include <qlayout.h>
#include <qdom.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <iostream>

UsConfig::UsConfig(QWidget* parent) :
  QWidget(parent),
  mDoc(new QDomDocument()),
  mLayout(new QGridLayout(this)),
  mScannerBox(new QComboBox(this)),
  mProbeBox(new QComboBox(this)),
  mRtSourceBox(new QComboBox(this)),
  mConfigBox(new QComboBox(this)),
  mWidthDeg(0),
  mDepth(0),
  mOffset(0),
  mOriginCol(0),
  mOriginRow(0),
  mNCorners(0),
  mLeftEdge(0),
  mRightEdge(0),
  mTopEdge(0),
  mBottomEdge(0),
  mPixelWidth(0),
  mPixelHeight(0),
{
  // Initialize settings if empty
  if (!mSettings->contains("Scanner"))
    mSettings->setValue("Scanner", "Select scanner...");
  if (!mSettings->contains("Probe"))
    mSettings->setValue("Probe", "Select probe...");
  if (!mSettings->contains("RTSource"))
    mSettings->setValue("RTSource", "Select realtime source...");
  if (!mSettings->contains("ConfigId"))
    mSettings->setValue("ConfigId", "Select configuration...");
    
  connect(mScannerBox, SIGNAL(activated(const QString&)), this, SLOT(populateProbeNamesSlot(const QString&)));
  connect(mProbeBox, SIGNAL(activated(const QString&)), this, SLOT(populateRtSourceSlot(const QString&)));
  connect(mProbeBox, SIGNAL(activated(const QString&)), this, SLOT(probeSelectedSlot(const QString&)));
  connect(mRtSourceBox, SIGNAL(activated(const QString&)), this, SLOT(populateProbePropertiesSlot(const QString&)));
  connect(mConfigBox, SIGNAL(activated(const QString&)), this, SLOT(readConfigParametersSlot(const QString&)));

  QLabel* scannerLabel = new QLabel("Scanner:", this);
  QLabel* probeLabel = new QLabel("Probe:", this);
  QLabel* sourceLabel = new QLabel("Source:", this);
  QLabel* configLabel = new QLabel("Configuration:", this);
  
  mLayout->addWidget(scannerLabel,  0, 0);
  mLayout->addWidget(mScannerBox,   1, 0);
  mLayout->addWidget(probeLabel,    2, 0);
  mLayout->addWidget(mProbeBox,     3, 0);
  mLayout->addWidget(sourceLabel,   4, 0);
  mLayout->addWidget(mRtSourceBox,  5, 0);
  mLayout->addWidget(configLabel,   6, 0);
  mLayout->addWidget(mConfigBox,    7, 0);
  mLayout->setRowStretch(8,1);
  
  try
  {
    this->initScannerList();
  }
  catch( char * str ) {
     std::cout << "Exception raised: " << str << std::endl;
  }
}

UsConfig::~UsConfig()
{}

QStringList UsConfig::getConfiguration()
{
  QStringList list;
  list << this->getCurrentScanner() << this->getCurrentProbe() << this->getCurrentRtSource() << this->getCurrentConfig();
  return list;
}

void UsConfig::initScannerList()
{  
  QFile file("ProbeCalibConfigs.xml");
  if(file.open(QIODevice::ReadOnly))
  { 
    QString emsg;
    int eline, ecolumn;
    // Read the file
    if (!mDoc->setContent(&file, false, &emsg, &eline, &ecolumn))
    {
      std::cout << "Could not parse XML file :";
      std::cout << file.fileName().toStdString() << " because: ";
      std::cout << emsg.toStdString() << std::endl;
      throw "Could not parse XML file :";
    }
    file.close();
    //Read the xml
    //QDomNode scannerNode = mDoc->namedItem("USScanner");
    QDomNode scannerNode = mDoc->namedItem("root");
    scannerNode = scannerNode.firstChild();
    while(!scannerNode.isNull())
    {
      QDomElement scannerElement = scannerNode.toElement();
      if(!scannerElement.isNull())// Is node an element?
      {
        if (scannerElement.tagName() == "USScanner")
        {
          // Insert scanner name
          QDomElement scannerNameElement = scannerNode.namedItem("Name").toElement();
          mScannerBox->addItem(scannerNameElement.text());
        }
      }
      scannerNode = scannerNode.nextSibling();
    }
  }
  else
    throw "Could not open file: ProbeCalibConfigs.xml";
    
  this->populateProbeNamesSlot(mScannerBox->currentText());
}

QDomNode UsConfig::findScanner(QString scannerName)
{
  QDomNode scannerNode = mDoc->namedItem("root");
  scannerNode = scannerNode.namedItem("USScanner");
  while(!scannerNode.isNull())
  {
    QDomElement scannerElement = scannerNode.toElement();
    if(!scannerElement.isNull())// Is node an element?
      if (scannerElement.tagName() == "USScanner")
      {
        QDomElement scannerNameElement = scannerNode.namedItem("Name").toElement();
        if(scannerName == scannerNameElement.text())
        {
          return scannerNode;// Found selected scanner
        }
      }
    scannerNode = scannerNode.nextSibling();
  }
  //throw "Coudn't find scanner";
  std::cout << "UsConfig::findScanner - Coudn't find scanner";
  std::cout << scannerName.toStdString() << std::endl;
  return scannerNode;
}

QDomNode UsConfig::findProbe(QString scannerName, QString probeName)
{
  QDomNode scannerNode = this->findScanner(scannerName);
  QDomNode probeNode = scannerNode.namedItem("USProbe");
  while(!probeNode.isNull())
  {
    QDomElement probeElement = probeNode.toElement();
    if(!probeElement.isNull())// Is node an element?
      if (probeElement.tagName() == "USProbe")
      {
        QDomElement probeNameElement = probeNode.namedItem("Name").toElement();
        if(probeName == probeNameElement.text())
        {
          return probeNode;// Found selected probe
        }
      }
    probeNode = probeNode.nextSibling();
  }
  //throw "Couldn't find probe";
  std::cout << "UsConfig::findProbe - Couldn't find probe: ";
  std::cout << probeName.toStdString() << std::endl;
  return probeNode;
}

QDomNode UsConfig::findRtSource(QString scannerName, 
                                QString probeName, 
                                QString rtSource)
{
  QDomNode probeNode = this->findProbe(scannerName, probeName);
  QDomNode sourceNode = probeNode.namedItem("RTSource");
  while(!sourceNode.isNull())
  {
    QDomElement sourceElement = sourceNode.toElement();
    if(!sourceElement.isNull())// Is node an element?
      if (sourceElement.tagName() == "RTSource")
      {
        QDomElement sourceNameElement = sourceNode.namedItem("Name").toElement();
        if(rtSource == sourceNameElement.text())
        {
          return sourceNode;// Found selected source
        }
      }
    sourceNode = sourceNode.nextSibling();
  }
  //throw "Couldn't find RT source";
  std::cout << "UsConfig::findRtSource - Couldn't find RT source: ";
  std::cout << rtSource.toStdString() << std::endl;
  return sourceNode;
}

QDomNode UsConfig::findConfig(QString configName)
{
  QDomNode sourceNode = this->findRtSource(mScannerBox->currentText(), 
                                           mProbeBox->currentText(),
                                           mRtSourceBox->currentText());
  QDomNode configNode = sourceNode.namedItem("Config");
  while(!configNode.isNull())
  {
    QDomElement configElement = configNode.toElement();
    if(!configElement.isNull())// Is node an element?
      if (configElement.tagName() == "Config")
      {
        QDomElement configIdElement = configNode.namedItem("ID").toElement();
        if(configName == configIdElement.text())
        {
          return configNode;// Found selected config
        }
      }
    configNode = configNode.nextSibling();
  }
  //throw "Couldn't find config";
  std::cout << "UsConfig::findConfig - Couldn't find config: ";
  std::cout << configName.toStdString() << std::endl;
  return configNode;
}

void UsConfig::populateProbeNamesSlot(const QString& scanner)
{
  mProbeBox->clear();
  QDomNode scannerNode = this->findScanner(scanner);
  if(scannerNode.isNull())
    throw "Invalid US scanner name";
  
  QDomNode probeNode = scannerNode.namedItem("USProbe");
  while(!probeNode.isNull())
  {
    QDomElement probeElement = probeNode.toElement();
    if(!probeElement.isNull())// Is node an element?
      if (probeElement.tagName() == "USProbe")
      {
        // Insert probe name
        QDomElement probeNameElement = probeNode.namedItem("Name").toElement();
        mProbeBox->addItem(probeNameElement.text());
      }
    
    probeNode = probeNode.nextSibling();
  }
  this->populateRtSourceSlot(mProbeBox->currentText());
}


void UsConfig::populateRtSourceSlot(const QString& probe)
{
  mRtSourceBox->clear();
  QDomNode probeNode = this->findProbe(mScannerBox->currentText(), probe);
  if(probeNode.isNull())
    throw "Invalid probe name";
  QDomNode defaultRtSourceNode = probeNode.namedItem("DefaultRTSource");
    
  QDomNode rtSourceNode = probeNode.namedItem("RTSource");
  while(!rtSourceNode.isNull())
  {
    QDomElement rtSourceElement = rtSourceNode.toElement();
    if(!rtSourceElement.isNull())// Is node an element?
      if (rtSourceElement.tagName() == "RTSource")
      {
        QDomNode rtSourceNameNode = rtSourceNode.namedItem("Name");
        if(!rtSourceNameNode.isNull())
        {
          QDomElement rtSourceNameElement = rtSourceNameNode.toElement();
          if(!rtSourceNameElement.isNull())// Is node an element?
          {
            std::cout << "Insert: " << rtSourceNameElement.text().toStdString() << std::endl;
            if(!defaultRtSourceNode.isNull() 
               && !defaultRtSourceNode.toElement().isNull()
               && rtSourceNameElement.text() == defaultRtSourceNode.toElement().text())
            {
              mRtSourceBox->insertItem(1, rtSourceNameElement.text());
              mRtSourceBox->setCurrentIndex(1);
            }
            else
              mRtSourceBox->addItem(rtSourceNameElement.text());
          }
        }
      }
    rtSourceNode = rtSourceNode.nextSibling();
  }
  
  /*if(!defaultRtSourceNode.isNull())
  {
    QDomElement rtSourceElement = defaultRtSourceNode.toElement();
    if(!rtSourceElement.isNull())
    {
      //Insert default source if it don't exist
      if(this->findRtSource(mScannerBox->currentText(), probe, 
                            rtSourceElement.text()).isNull())
        mRtSourceBox->addItem(rtSourceElement.text());
      mRtSourceBox->setCurrentText(rtSourceElement.text());
    }
  }*/
  this->populateProbePropertiesSlot(mRtSourceBox->currentText());
}

void UsConfig::populateProbePropertiesSlot(const QString& rtSource)
{
  mConfigBox->clear();
  QString defaultConfig;
  QDomNode rtSourceNode = this->findRtSource(mScannerBox->currentText(), 
                                             mProbeBox->currentText(),
                                             rtSource);
  if(rtSourceNode.isNull())
  {
    //throw "Invalid source name";
    std::cout << "UsConfig::populateProbePropertiesSlot - Invalid source name: ";
    std::cout << rtSource.toStdString() << std::endl;
    return;
  }
  else
  {
    QDomElement rtSourceElement = rtSourceNode.toElement();
    if(!rtSourceElement.isNull())// Is node an element?       
    { 
      QDomNode rtSourceSubNode = rtSourceNode.firstChild();
      while(!rtSourceSubNode.isNull())
      {
        QDomElement rtSourceSubElement = rtSourceSubNode.toElement();
        if(!rtSourceSubElement.isNull())// Is node an element?
        {
          if(rtSourceSubElement.tagName() == "DefaultConfig")
            defaultConfig = rtSourceSubElement.text();
          else if(rtSourceSubElement.tagName() == "Config")
          {
            QDomElement element = rtSourceSubNode.toElement();
            if(!element.isNull())// Is node an element?
            {
              //QDomNode node = rtSourceSubNode.firstChild();
              QDomNode node = rtSourceSubNode.namedItem("ID");
              element = node.toElement();
              if(!element.isNull())
              {
                if(defaultConfig == element.text())
                {
                  mConfigBox->insertItem(1, element.text());
                  mConfigBox->setCurrentIndex(1);
                }
                else
                  mConfigBox->addItem(element.text());
              }
            }
          }
        }
        rtSourceSubNode = rtSourceSubNode.nextSibling();
      }
    }
    //if(!defaultConfig.isEmpty())
    //  mConfigBox->setCurrentText(defaultConfig);
  }
}

void UsConfig::readConfigParametersSlot(const QString& config)
{
  try
  {
    QDomNode configNode = findConfig(config);
    if(configNode.isNull())
      throw "Can't find selected config settings";

    QDomElement configElement = configNode.toElement();
    if(configElement.isNull())
      throw "Config node is no element";
    QDomElement element = configNode.namedItem("WidthDeg").toElement();
    if(element.isNull())
      throw "Can't find WidthDeg";
    bool ok;
    mWidthDeg = element.text().toInt(&ok);
    if(!ok)
      throw "WidthDeg not a number";

    element = configNode.namedItem("Depth").toElement();
    if(element.isNull())
      throw "Can't find Depth";
    mDepth = element.text().toInt(&ok);
    if(!ok)
      throw "Depth not a number";

    element = configNode.namedItem("Offset").toElement();
    if(element.isNull())
      throw "Can't find Offset";
    mOffset = element.text().toInt(&ok);
    if(!ok)
      throw "Offset not a number";

    QDomNode originNode = configNode.namedItem("Origin");
    element = originNode.namedItem("Col").toElement();
    if(element.isNull())
      throw "Can't find Origin.Col";
    mOriginCol = element.text().toInt(&ok);
    if(!ok)
      throw "Origin.Col not a number";

    element = originNode.namedItem("Row").toElement();
    if(element.isNull())
      throw "Can't find Origin.Row";
    mOriginRow = element.text().toInt(&ok);
    if(!ok)
      throw "Origin.Row not a number";

    element = configNode.namedItem("NCorners").toElement();
    if(element.isNull())
      throw "Can't find NCorners";
    mNCorners = element.text().toInt(&ok);
    if(!ok)
      throw "NCorners not a number";

    QDomNode cornerNode = configNode.firstChildElement("Corner");
    if(cornerNode.isNull())
      throw "Can't find Corner";
    mCorners.clear();
    for(int i=0; i<mNCorners ; ++i)
    {
      element = cornerNode.namedItem("Col").toElement();
      if(element.isNull())
        throw "Can't find Corner.Col";
      int col = element.text().toInt(&ok);
      if(!ok)
        throw "Corner.Col not a number";

      element = cornerNode.namedItem("Row").toElement();
      if(element.isNull())
        throw "Can't find Corner.Row";
      int row = element.text().toInt(&ok);
      if(!ok)
        throw "Corner.Row not a number";

      mCorners.push_back(ColRowPair(col,row));

      cornerNode = cornerNode.nextSibling();
    }

    QDomNode edgesNode = configNode.namedItem("Edges");
    element = edgesNode.namedItem("Left").toElement();
    if(element.isNull())
      throw "Can't find Left";
    mLeftEdge = element.text().toInt(&ok);
    if(!ok)
      throw "Left not a number";

    element = edgesNode.namedItem("Right").toElement();
    if(element.isNull())
      throw "Can't find Right";
    mRightEdge = element.text().toInt(&ok);
    if(!ok)
      throw "Right not a number";

    element = edgesNode.namedItem("Top").toElement();
    if(element.isNull())
      throw "Can't find Top";
    mTopEdge = element.text().toInt(&ok);
    if(!ok)
      throw "Top not a number";

    element = edgesNode.namedItem("Bottom").toElement();
    if(element.isNull())
      throw "Can't find Bottom";
    mBottomEdge = element.text().toInt(&ok);
    if(!ok)
      throw "Bottom not a number";

    QDomNode pixelSizeNode = configNode.namedItem("PixelSize");
    element = pixelSizeNode.namedItem("Width").toElement();
    if(element.isNull())
      throw "Can't find PixelSize.Width";
    mPixelWidth = element.text().toDouble(&ok);
    if(!ok)
      throw "PixelSize.Width not a number";

    element = pixelSizeNode.namedItem("Height").toElement();
    if(element.isNull())
      throw "Can't find PixelSize.Height";
    mPixelHeight = element.text().toDouble(&ok);
    if(!ok)
      throw "PixelSize.Height not a number";

  }
  catch( char * str ) {
     std::cout << "Exception raised: " << str << std::endl;
  }
  emit USProbePropertiesChanged();
}

void UsConfig::probeSelectedSlot(const QString& probe)
{
  std::cout << "UsConfig::probeSelectedSlot(const QString& "<< probe.toStdString() <<")" << std::endl;
  emit probeSelected(probe);
}
