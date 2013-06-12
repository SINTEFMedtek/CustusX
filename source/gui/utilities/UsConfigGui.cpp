#include "UsConfigGui.h"

#include <iostream>
#include <QGridLayout>
#include <QComboBox>
#include <QLabel>
#include <QFile>
#include <QFileInfo>
#include "cxDataLocations.h"
#include "cxSettings.h"

UsConfigGui::UsConfigGui(QWidget* parent) :
  QWidget(parent),
  mLayout(new QGridLayout(this)),
  mScannerBox(new QComboBox(this)),
  mProbeBox(new QComboBox(this)),
  mRtSourceBox(new QComboBox(this)),
  mConfigIdBox(new QComboBox(this))
{
  
  this->setObjectName("ProbePropertiesWidget");
  this->setWindowTitle("US Probe Properties");
  
  mXmlFileName = cx::DataLocations::getRootConfigPath()+QString("/tool/ProbeCalibConfigs.xml");
  mXml = new ProbeXmlConfigParserImpl(mXmlFileName);

  // Initialize settings if empty
  if (!cx::settings()->contains("Scanner"))
    cx::settings()->setValue("Scanner", "Select scanner...");
  if (!cx::settings()->contains("Probe"))
    cx::settings()->setValue("Probe", "Select probe...");
  if (!cx::settings()->contains("RTSource"))
    cx::settings()->setValue("RTSource", "Select realtime source...");
  if (!cx::settings()->contains("ConfigId"))
    cx::settings()->setValue("ConfigId", "Select configuration...");

  QLabel* scannerLabel = new QLabel("Scanner:", this);
  QLabel* probeLabel = new QLabel("Probe:", this);
  QLabel* sourceLabel = new QLabel("Source:", this);
  QLabel* configLabel = new QLabel("Configuration Id:", this);

  mLayout->addWidget(scannerLabel,  0, 0);
  mLayout->addWidget(mScannerBox,   1, 0);
  mLayout->addWidget(probeLabel,    2, 0);
  mLayout->addWidget(mProbeBox,     3, 0);
  mLayout->addWidget(sourceLabel,   4, 0);
  mLayout->addWidget(mRtSourceBox,  5, 0);
  mLayout->addWidget(configLabel,   6, 0);
  mLayout->addWidget(mConfigIdBox,  7, 0);
  mLayout->setRowStretch(8,1);

  connect(mScannerBox, SIGNAL(activated(const QString&)), this, SLOT(scannerChanged(const QString&)));
  connect(mProbeBox, SIGNAL(activated(const QString&)), this, SLOT(probeChanged(const QString&)));
  connect(mRtSourceBox, SIGNAL(activated(const QString&)), this, SLOT(rtSourceChanged(const QString&)));
  //connect(mConfigIdBox, SIGNAL(activated(const QString&)), this, SLOT(configIdChanged(const QString&)));
  connect(mConfigIdBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(configIdChanged(const QString&)));

  this->initComboBoxes();
}

UsConfigGui::~UsConfigGui()
{}

void UsConfigGui::initComboBoxes()
{
  this->populateScannerBox(cx::settings()->value("Scanner").toString());
  this->populateProbeBox(cx::settings()->value("Probe").toString());
  this->populateRtSourceBox(cx::settings()->value("RTSource").toString());
  this->populateConfigIdBox(cx::settings()->value("ConfigId").toString());
}

void UsConfigGui::populateScannerBox(const QString& tryToSelect)
{
  QStringList scannerList = mXml->getScannerList();
  mScannerBox->clear();
  mScannerBox->insertItems(0, scannerList);

  int index = mScannerBox->findText(tryToSelect);
  if(index != -1)
    mScannerBox->setCurrentIndex(index);
}

void UsConfigGui::populateProbeBox(const QString& tryToSelect)
{
  QStringList probeList = mXml->getProbeList(mScannerBox->currentText());
  mProbeBox->clear();
  mProbeBox->insertItems(0, probeList);

  int index = mProbeBox->findText(tryToSelect);
  if(index != -1)
    mProbeBox->setCurrentIndex(index);
}

void UsConfigGui::populateRtSourceBox(const QString& tryToSelect)
{
  QStringList rtSourceList = mXml->getRtSourceList(mScannerBox->currentText(), mProbeBox->currentText());
  mRtSourceBox->clear();
  mRtSourceBox->insertItems(0, rtSourceList);

  int index = mRtSourceBox->findText(tryToSelect);
  if(index != -1)
    mRtSourceBox->setCurrentIndex(index);
}

void UsConfigGui::populateConfigIdBox(const QString& tryToSelect)
{
  QStringList configIdList = mXml->getConfigIdList(mScannerBox->currentText(), mProbeBox->currentText(), mRtSourceBox->currentText());
  mConfigIdBox->clear();
  mConfigIdBox->insertItems(0, configIdList);

  int index = mConfigIdBox->findText(tryToSelect);
  if(index != -1)
    mConfigIdBox->setCurrentIndex(index);
}

void UsConfigGui::scannerChanged(const QString& scanner)
{
  this->populateProbeBox("");
  this->populateRtSourceBox(cx::settings()->value("RTSource").toString());
  this->populateConfigIdBox("");
  
  cx::settings()->setValue("Scanner", mScannerBox->currentText());
}

void UsConfigGui::probeChanged(const QString& probe)
{
  this->populateRtSourceBox(cx::settings()->value("RTSource").toString());
  this->populateConfigIdBox("");
  
  cx::settings()->setValue("Probe", mProbeBox->currentText());
  
  emit probeSelected(probe);
}

void UsConfigGui::rtSourceChanged(const QString& rtsource)
{
  //std::cout << "UsConfigGui::rtSourceChanged " << rtsource.toStdString().c_str() << std::endl;
  this->populateConfigIdBox("");
  
  cx::settings()->setValue("RTSource", mRtSourceBox->currentText());
}

void UsConfigGui::RTsourceDetected(const QString& source)
{
  std::cout << "UsConfigGui::RTsourceDetected " << source.toStdString().c_str() << std::endl;
  // Set RT source to VGA or S-VHS according to detected grabber
  if ( (source.compare(QString("VGA")) == 0) || (source.compare(QString("VGA_DVI"))) == 0)  
  {
    mRtSourceBox->setCurrentIndex(mRtSourceBox->findText("VGA"));
    //cx::settings()->setValue("VideoSource", QString("VGA"));
    
  } else {
    mRtSourceBox->setCurrentIndex(mRtSourceBox->findText("SVHS"));
    //cx::settings()->setValue("VideoSource", QString("SVHS"));
  }

  cx::settings()->setValue("RTSource", mRtSourceBox->currentText());
  this->populateConfigIdBox("");
}

void UsConfigGui::configIdChanged(const QString& configId)
{
  cx::settings()->setValue("ConfigId", mConfigIdBox->currentText());
  emit configurationChanged();
  //std::cout << "emit configurationChanged(); SHOULD DRAW NOW!!!" << std::endl;
}

QStringList UsConfigGui::getConfigurationString()
{
  QStringList list;
  list << mScannerBox->currentText() << mProbeBox->currentText() << mRtSourceBox->currentText() << mConfigIdBox->currentText();
  return list;
}

ProbeXmlConfigParser::Configuration UsConfigGui::getConfiguration()
{
  QStringList list = this->getConfigurationString();
  return mXml->getConfiguration(list.at(0), list.at(1), list.at(2), list.at(3));
}

void UsConfigGui::setXml(QString filename)
{
  QFile newXml(filename);
  QFileInfo info(newXml);
  QString absolutePath = info.absoluteFilePath();
  if(!newXml.exists())
  {
    std::cout << "The xml file("<< absolutePath.toStdString() <<") you are trying to select cannot be found." << std::endl;
  }
  
  delete mXml;
  mXml = new ProbeXmlConfigParserImpl(absolutePath);
  
  this->initComboBoxes();
}

void UsConfigGui::setEditMode(bool on)
{
  mScannerBox->setEditable(on);
  mProbeBox->setEditable(on);
  mRtSourceBox->setEditable(on);
  mConfigIdBox->setEditable(on);
}
