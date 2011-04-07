#include <cxToolConfigureWidget.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include "sscEnumConverter.h"
#include "sscMessageManager.h"
#include "cxStateMachineManager.h"
#include "cxToolManager.h"
#include "cxSelectionGroupBox.h"
#include "cxToolListWidget.h"
#include "cxDataLocations.h"

namespace cx
{

ToolConfigureGroupBox::ToolConfigureGroupBox(QWidget* parent) :
    QGroupBox(parent),
    mClinicalApplication(ssc::mdCOUNT),
    mConfigFilesComboBox(new QComboBox()),
    mConfigFilePathLineEdit(new QLineEdit()),
    mReferenceComboBox(new QComboBox())
{
  Q_PROPERTY("userEdited")

  mConfigFilesComboBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
  //mConfigFilesComboBox->setMinimumSize(200, 0);
  //mConfigFilesComboBox->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  mApplicationGroupBox = new SelectionGroupBox("Applications", stateManager()->getApplication()->getAllApplicationNames(), true, NULL);
  mApplicationGroupBox->setEnabledButtons(false); //< application application is determined by the application state chosen elsewhere in the system
  mApplicationGroupBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Expanding);
  mTrackingSystemGroupBox = new SelectionGroupBox("Tracking systems", ToolManager::getInstance()->getSupportedTrackingSystems(), true, NULL);
  mToolListWidget = new ConfigToolListWidget(NULL);

  this->setClinicalApplicationSlot(string2enum<ssc::CLINICAL_APPLICATION>(stateManager()->getApplication()->getActiveStateName()));

  QGroupBox* toolGroupBox = new QGroupBox();
  toolGroupBox->setTitle("Tools");
  QVBoxLayout* toolLayout = new QVBoxLayout();
  toolGroupBox->setLayout(toolLayout);
  toolLayout->addWidget(mToolListWidget);

  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(new QLabel("Selected config: "), 0, 0, 1, 1);
  layout->addWidget(mConfigFilesComboBox, 0, 1, 1, 1);
  layout->addWidget(new QLabel("Save path: "), 1, 0, 1, 1);
  layout->addWidget(mConfigFilePathLineEdit, 1, 1, 1, 1);
  layout->addWidget(mApplicationGroupBox, 2, 0, 1, 2);
  layout->addWidget(mTrackingSystemGroupBox, 3, 0, 1, 2);
  layout->addWidget(toolGroupBox, 4, 0, 1, 2);
  layout->addWidget(new QLabel("Reference: "), 5, 0, 1, 1);
  layout->addWidget(mReferenceComboBox, 5, 1, 1, 1);

  //changes due to programming actions
  connect(mConfigFilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChangedSlot()));
  connect(mToolListWidget, SIGNAL(listSizeChanged()), this, SLOT(toolsChangedSlot()));
  connect(mTrackingSystemGroupBox, SIGNAL(selectionChanged()), this, SLOT(filterToolsSlot()));
  connect(mToolListWidget, SIGNAL(toolSelected(QString)), this, SIGNAL(toolSelected(QString)));

  //changes due to user actions
  connect(mConfigFilePathLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(pathEditedSlot()));
  connect(mConfigFilePathLineEdit, SIGNAL(textEdited(const QString&)), this, SLOT(configEditedSlot()));
  connect(mApplicationGroupBox, SIGNAL(userClicked()), this, SLOT(configEditedSlot()));
  connect(mTrackingSystemGroupBox, SIGNAL(userClicked()), this, SLOT(configEditedSlot()));
  connect(mToolListWidget, SIGNAL(userChangedList()), this, SLOT(configEditedSlot()));
  connect(mReferenceComboBox, SIGNAL(activated(int)), this, SLOT(configEditedSlot()));

  this->populateConfigurations();
}

ToolConfigureGroupBox::~ToolConfigureGroupBox()
{}

QString ToolConfigureGroupBox::getCurrenctlySelectedConfiguration() const
{
  QString retval;
  retval = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
  return retval;
}

void ToolConfigureGroupBox::requestSaveConfigurationSlot()
{
  if(!mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), sEdited).toBool())
    return;

  ConfigurationFileParser::Configuration config = this->getCurrentConfiguration();
  ConfigurationFileParser::saveConfiguration(config);
}

void ToolConfigureGroupBox::setClinicalApplicationSlot(ssc::CLINICAL_APPLICATION clinicalApplication)
{
  mClinicalApplication = clinicalApplication;
  this->setTitle("Tool configurations for "+enum2string(mClinicalApplication));

  this->populateConfigurations();
}

void ToolConfigureGroupBox::configChangedSlot()
{
  QStringList selectedApplications;
  QStringList selectedTrackingSystems;
  QStringList selectedTools;
  QString absoluteConfigFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
  bool suggestDefaultNames;

  if(!mConfigFilesComboBox->currentText().contains("<new config>"))
  {
    ConfigurationFileParser parser(absoluteConfigFilePath);

    ssc::CLINICAL_APPLICATION application = parser.getApplicationapplication();
    selectedApplications << enum2string(application);

    std::vector<IgstkTracker::InternalStructure> trackers = parser.getTrackers();
    for(unsigned i=0; i<trackers.size(); ++i)
    {
      selectedTrackingSystems << enum2string(trackers[i].mType);
    }

    std::vector<QString> tools = parser.getAbsoluteToolFilePaths();
    for(unsigned i=0; i<tools.size(); ++i)
    {
      selectedTools << tools[i];
    }
    suggestDefaultNames = false;
  }
  else
  {
    selectedApplications << enum2string(mClinicalApplication); // just want a default
    selectedTrackingSystems << enum2string(ssc::tsPOLARIS); //just want a default
    suggestDefaultNames = true;
  }

//  std::cout << "absoluteConfigFilePath" << absoluteConfigFilePath << std::endl;
  mConfigFilePathLineEdit->setText(absoluteConfigFilePath);
  this->setState(mConfigFilePathLineEdit, !suggestDefaultNames);
  mApplicationGroupBox->setSelected(selectedApplications);
  mTrackingSystemGroupBox->setSelected(selectedTrackingSystems);
  mToolListWidget->configSlot(selectedTools);
}

void ToolConfigureGroupBox::configEditedSlot()
{
  this->setState(mConfigFilesComboBox, mConfigFilesComboBox->currentIndex(), true);
}

void ToolConfigureGroupBox::toolsChangedSlot()
{
  this->populateReference();
  this->filterToolsSlot();
}

void ToolConfigureGroupBox::filterToolsSlot()
{
  QStringList trackingsystemFilter = mTrackingSystemGroupBox->getSelected();
  mToolListWidget->filterSlot(trackingsystemFilter);
}

void ToolConfigureGroupBox::pathEditedSlot()
{
  this->setState(mConfigFilePathLineEdit, true);
}

void ToolConfigureGroupBox::populateConfigurations()
{
  mConfigFilesComboBox->clear();

  QDir dir(DataLocations::getRootConfigPath()+"/tool/"+enum2string(mClinicalApplication));
  dir.setFilter(QDir::Files);

  QStringList nameFilters;
  nameFilters << "*.xml";
  dir.setNameFilters(nameFilters);

  QString newConfig("<new config>");
  int index = this->addConfigurationToComboBox(newConfig, this->generateConfigName());

  QStringList configlist = dir.entryList();
  foreach(QString filename, configlist)
  {
    QFile file(dir.absolutePath()+"/"+filename);
    QFileInfo info(file);
    index = this->addConfigurationToComboBox(filename, info.absoluteFilePath());
  }

  int currentIndex = mConfigFilesComboBox->findText(newConfig);
  mConfigFilesComboBox->setCurrentIndex(currentIndex);
}

int ToolConfigureGroupBox::addConfigurationToComboBox(QString displayName, QString absoluteFilePath, bool edited)
{
  mConfigFilesComboBox->addItem(displayName);
  int index = mConfigFilesComboBox->findText(displayName);
  mConfigFilesComboBox->setItemData(index, absoluteFilePath, Qt::ToolTipRole);
  this->setState(mConfigFilesComboBox, index, false);

  return index;
}

void ToolConfigureGroupBox::setState(QComboBox* box, int index, bool edited)
{
  box->setItemData(index, edited, sEdited);
//  std::cout << "Config file " << box->itemText(index) << " now is set as " << (edited ? "" : "un") << "edited." << std::endl;

  if(edited && !mConfigFilePathLineEdit->property("userEdited").toBool())
//  {
//    std::cout << "Generating name..." << std::endl;
    mConfigFilePathLineEdit->setText(this->generateConfigName());
//  }
}

ConfigurationFileParser::Configuration ToolConfigureGroupBox::getCurrentConfiguration()
{
  ConfigurationFileParser::Configuration retval;
  retval.mFileName = mConfigFilePathLineEdit->text();
  retval.mClinical_app = string2enum<ssc::CLINICAL_APPLICATION>(mApplicationGroupBox->getSelected()[0]);

  QStringList selectedTools = mToolListWidget->getTools();
  QString referencePath = mReferenceComboBox->itemData(mReferenceComboBox->currentIndex(), Qt::ToolTipRole).toString();

  ssc::TRACKING_SYSTEM selectedTracker = string2enum<ssc::TRACKING_SYSTEM>(mTrackingSystemGroupBox->getSelected()[0]);

  ConfigurationFileParser::ToolFilesAndReferenceVector toolfilesAndRefVector;
  QFile configFile(retval.mFileName);
  QFileInfo info(configFile);
  QDir dir = info.dir();
  foreach(QString absoluteToolPath, selectedTools)
  {
    QString relativeToolFilePath = dir.relativeFilePath(absoluteToolPath);
//    std::cout << "Relative tool file path: " << relativeToolFilePath << std::endl;

    ConfigurationFileParser::ToolFileAndReference tool;
    tool.first = relativeToolFilePath;

//    std::cout << "====" << std::endl;
//    std::cout << "absoluteToolPath " << absoluteToolPath << std::endl;
//    std::cout << "referencePath " << referencePath << std::endl;
    tool.second = (absoluteToolPath == referencePath);
    toolfilesAndRefVector.push_back(tool);
  }

  retval.mTrackersAndTools[selectedTracker] = toolfilesAndRefVector;

  return retval;
}

QString ToolConfigureGroupBox::generateConfigName()
{
  QString retval;

  QStringList applicationFilter = mApplicationGroupBox->getSelected();
  QStringList trackingsystemFilter = mTrackingSystemGroupBox->getSelected();
  QStringList absoluteToolFilePathsFilter = mToolListWidget->getTools();

  QString absoluteDirPath;
  QString trackingSystems;
  QString tools;

  absoluteDirPath = DataLocations::getRootConfigPath()+"/tool/"+((applicationFilter.size() >= 1) ? applicationFilter[0]+"/" : "")+""; //a config can only belong to one application

  foreach(QString string, trackingsystemFilter)
  {
    trackingSystems.append(string+"_");
  }

  foreach(QString string, absoluteToolFilePathsFilter)
  {
    QFile file(string);
    QFileInfo info(file);
    trackingSystems.append(info.baseName()+"_");
  }
  if(retval.endsWith("_", Qt::CaseInsensitive))
    retval.remove(retval.size()-2, 1);

  retval = absoluteDirPath+trackingSystems+tools+".xml";

  return retval;
}


void ToolConfigureGroupBox::setState(QLineEdit* line, bool userEdited)
{
//  std::cout << "line set to " << (userEdited ? "edited (should not generate new names from now)" : "not editet (generate from now)") << std::endl;
  QVariant value(userEdited);
  mConfigFilePathLineEdit->setProperty("userEdited", value);
}

void ToolConfigureGroupBox::populateReference()
{
  mReferenceComboBox->clear();

  int currentIndex = 0;
  if(mConfigFilesComboBox->currentText().contains("<new config>"))
  {
    QStringList selectedTools = mToolListWidget->getTools();
    foreach(QString string, selectedTools)
    {
      ToolFileParser parser(string);
      Tool::InternalStructure internal = parser.getTool();
      if(internal.mType == ssc::Tool::TOOL_REFERENCE)
      {
//        std::cout << "Found reference when making new config: " << string << std::endl;
        currentIndex = this->addRefrenceToComboBox(string);
      }
//      else
//      {
//        std::cout << string << " is not a refrence." << std::endl;
//      }
    }
  }
  else
  {
    QString configAbsoluteFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
    ConfigurationFileParser parser(configAbsoluteFilePath);
    QString reference = parser.getAbsoluteReferenceFilePath();
//    std::cout << "Added reference: " << reference << std::endl;
    if(reference.isEmpty())
    {
//      ssc::messageManager()->sendDebug("Could not determine the reference for configfile: "+configAbsoluteFilePath);
      return;
    }

    currentIndex = this->addRefrenceToComboBox(reference);
  }
  mReferenceComboBox->setCurrentIndex(currentIndex);
}
int ToolConfigureGroupBox::addRefrenceToComboBox(QString absoluteRefereneFilePath)
{
  int index;

  QFile file(absoluteRefereneFilePath);
  QFileInfo info(file);

  mReferenceComboBox->addItem(info.dir().dirName());
  //    std::cout << "Added reference: " << info.dir().dirName() << std::endl;
  index = mReferenceComboBox->findText(info.dir().dirName());
  mReferenceComboBox->setItemData(index, info.absoluteFilePath(), Qt::ToolTipRole);

  return index;
}
}//namespace cx
