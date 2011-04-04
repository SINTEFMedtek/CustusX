#include <cxToolConfigWidget.h>

#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QListWidget>
#include <QLineEdit>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscEnumConverter.h"
#include "cxDataLocations.h"
#include "cxStateMachineManager.h"
#include "cxToolManager.h"
#include "cxFilePreviewWidget.h"

//testing
#include "cxToolFilterWidget.h"
#include "cxToolConfigureWidget.h"
namespace cx
{

ToolConfigWidget::ToolConfigWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mConfigFilesComboBox(new QComboBox()),
    mCurrentConfigFile("<new config>"),
    mConfigFileLineEdit(new QLineEdit("Select a config...")),
    mTrackerGroupBox(new QGroupBox()),
    mTrackerButtonGroup(new QButtonGroup()),
    mToolGroup(new QGroupBox()),
    mToolListWidget(new QListWidget()),
    mSelectedReferenceComboBox(new QComboBox()),
    mApplicationGroupBox(new QGroupBox()),
    mApplicationButtonGroup(new QButtonGroup())
{
  //layout
  QGridLayout* layout = new QGridLayout(this);

  layout->addWidget(new QLabel("Config files:"), 0, 0);
  layout->addWidget(mConfigFilesComboBox, 0, 1);

  layout->addWidget(mApplicationGroupBox, 1, 0, 1, 2);

  layout->addWidget(mTrackerGroupBox, 2, 0, 1, 2);

  layout->addWidget(mToolGroup, 3, 0, 1, 2);

  QHBoxLayout* hlayout = new QHBoxLayout();
  hlayout->addWidget(new QLabel("Config file: "));
  hlayout->addWidget(mConfigFileLineEdit);
  layout->addLayout(hlayout, 4, 0, 1, 2);

  //testing
  layout->addWidget(new ToolConfigureGroupBox(ssc::mdLABORATORY, this), 5, 0, 1, 2);
  layout->addWidget(new ToolFilterGroupBox(this), 6, 0, 1, 2);
  //testing

  mApplicationGroupBox->setTitle("Applications");
  QHBoxLayout* applicationlayout = new QHBoxLayout();
  mApplicationGroupBox->setLayout(applicationlayout);

  mTrackerGroupBox->setTitle("Tracking systems");
  QHBoxLayout* trackingsystemlayout = new QHBoxLayout();
  mTrackerGroupBox->setLayout(trackingsystemlayout);

  mToolGroup->setTitle("Tools");
  QVBoxLayout* toolslayout = new QVBoxLayout();
  mToolGroup->setLayout(toolslayout);
  toolslayout->addWidget(mToolListWidget);
  QGridLayout* refrenceLayout = new QGridLayout();
  refrenceLayout->addWidget(new QLabel("Selected reference: "), 0, 0);
  refrenceLayout->addWidget(mSelectedReferenceComboBox, 0, 1, 1, 2);
  toolslayout->addWidget(mToolListWidget);
  toolslayout->addLayout(refrenceLayout);

  //connect
  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));
  connect(mConfigFilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChangedSlot()));
  connect(mToolListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(toolClickedSlot(QListWidgetItem*)));
  connect(mToolListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(toolDoubleClickedSlot(QListWidgetItem*)));
  connect(mToolListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(toolSelectionChangedSlot()));
  connect(this, SIGNAL(toolSelected(QString)), this, SLOT(fileSelectedSlot(QString)));

  //populate
  this->populateConfigComboBox();
  this->populateApplications();
  this->populateTrackingSystems();
  this->populateToolList();
}

ToolConfigWidget::~ToolConfigWidget()
{}

QString ToolConfigWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Tool configuration.</h3>"
      "<p>Lets you construct a tool configuration from the systems available tools.</p>"
      "<p><i>Click Apply to use the selected configuration.</i></p>"
      "</html>";
}

QString ToolConfigWidget::getSelectedFile() const
{
  return mCurrentlySelectedFile;
}

void ToolConfigWidget::saveConfigurationSlot()
{
  if(mCurrentConfigFile != "<new config>")
    return;

  ConfigurationFileParser::Configuration config = this->getCurrentConfiguration();
  ConfigurationFileParser::saveConfiguration(config);
}

void ToolConfigWidget::applicationStateChangedSlot()
{
  this->populateConfigComboBox();
}

void ToolConfigWidget::filterToolsSlot()
{
  QStringList applicationFilter;
  QList<QAbstractButton*> applicationButtonList = mApplicationButtonGroup->buttons();
  foreach(QAbstractButton* button, applicationButtonList)
  {
    if(button->isChecked())
      applicationFilter << button->text();
  }
  QStringList trackingsystemFilter;
  QList<QAbstractButton*> trackerButtonList = mTrackerButtonGroup->buttons();
  foreach(QAbstractButton* button, trackerButtonList)
  {
    if(button->isChecked())
      trackingsystemFilter << button->text();
  }

  this->populateToolList(applicationFilter, trackingsystemFilter);
}

void ToolConfigWidget::configChangedSlot()
{
  mCurrentConfigFile = mConfigFilesComboBox->currentText();

  QString absoluteConfigFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();
  emit toolSelected(absoluteConfigFilePath);

  //update the filters to match the selected configuration file
  QStringList applicationFilter;
  QStringList trackingsystemFilter;
  QStringList absoluteToolFilePathsFilter;
  if(mCurrentConfigFile != "<new config>")
  {
    ConfigurationFileParser parser(absoluteConfigFilePath);
    //block signals???
    ssc::CLINICAL_APPLICATION application = parser.getApplicationapplication();
    applicationFilter << enum2string(application);

    std::vector<IgstkTracker::InternalStructure> trackers = parser.getTrackers();
    for(unsigned i=0; i<trackers.size(); ++i)
    {
      trackingsystemFilter << enum2string(trackers[i].mType);
//      std::cout << "added " << enum2string(trackers[i].mType) << " to tracking filter..." << std::endl;
    }

    std::vector<QString> absoluteToolFilePaths = parser.getAbsoluteToolFilePaths();
    for(unsigned i=0; i<absoluteToolFilePaths.size(); ++i)
    {
      absoluteToolFilePathsFilter << absoluteToolFilePaths[i];
      std::cout << "added " << absoluteToolFilePaths[i] << " to tool filter..." << std::endl;
    }

    mTrackerGroupBox->setEnabled(false);
    mApplicationGroupBox->setEnabled(false);
    mToolListWidget->setEnabled(false);
    mSelectedReferenceComboBox->setEnabled(false);
  }
  else
  {
    mTrackerGroupBox->setEnabled(true);
    mApplicationGroupBox->setEnabled(true);
    mToolListWidget->setEnabled(true);
    mSelectedReferenceComboBox->setEnabled(true);
  }

  this->filterButtonGroup(mApplicationButtonGroup, applicationFilter);
  this->filterButtonGroup(mTrackerButtonGroup, trackingsystemFilter);
  this->populateToolList(applicationFilter, trackingsystemFilter, absoluteToolFilePathsFilter);
  this->populateReferenceComboBox();
}

void ToolConfigWidget::toolClickedSlot(QListWidgetItem* item)
{
  QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
  emit toolSelected(absoluteFilePath);
}

void ToolConfigWidget::toolDoubleClickedSlot(QListWidgetItem* item)
{
  QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
  emit wantToEdit(absoluteFilePath);
}

void ToolConfigWidget::fileSelectedSlot(QString fileSelected)
{
  mCurrentlySelectedFile = fileSelected;
}

void ToolConfigWidget::toolSelectionChangedSlot()
{
  //update the config file path
  if(mConfigFilesComboBox->currentText() == "<new config>")
    mConfigFileLineEdit->setReadOnly(false);
  else
    mConfigFileLineEdit->setReadOnly(true);

  mConfigFileLineEdit->setText(this->getConfigFileName());

  //update the refrence suggestion
  this->populateReferenceComboBox();
}

void ToolConfigWidget::populateConfigComboBox()
{
  QDir dir(DataLocations::getApplicationToolConfigPath());
  dir.setFilter(QDir::Files);

  QStringList nameFilters;
  nameFilters << "*.xml";
  dir.setNameFilters(nameFilters);

  this->addConfigurationToComboBox("<new config>", ConfigurationFileParser::getTemplatesAbsoluteFilePath());

  QStringList configlist = dir.entryList();
  foreach(QString filename, configlist)
  {
    QFile file(dir.absolutePath()+"/"+filename);
    QFileInfo info(file);
    this->addConfigurationToComboBox(filename, info.absoluteFilePath());
  }

  int currentIndex = mConfigFilesComboBox->findText(mCurrentConfigFile);
  mConfigFilesComboBox->setCurrentIndex(currentIndex);
}

void ToolConfigWidget::populateApplications()
{
  QStringList applicationList = stateManager()->getApplication()->getAllApplicationNames();

  mApplicationButtonGroup->setExclusive(false);
  foreach(QString string, applicationList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mApplicationButtonGroup->addButton(box);
    mApplicationGroupBox->layout()->addWidget(box);

    connect(box, SIGNAL(stateChanged(int)), this, SLOT(filterToolsSlot()));
    connect(box, SIGNAL(stateChanged(int)), this, SLOT(toolSelectionChangedSlot()));
  }
}

void ToolConfigWidget::populateTrackingSystems()
{
  QStringList trackingSystemList = ToolManager::getInstance()->getSupportedTrackingSystems();

  mTrackerButtonGroup->setExclusive(true);
  foreach(QString string, trackingSystemList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mTrackerButtonGroup->addButton(box);
    mTrackerGroupBox->layout()->addWidget(box);
    connect(box, SIGNAL(stateChanged(int)), this, SLOT(filterToolsSlot()));
    connect(box, SIGNAL(stateChanged(int)), this, SLOT(toolSelectionChangedSlot()));
  }
}

void ToolConfigWidget::populateToolList(QStringList applicationFilter, QStringList trackingSystemFilter, QStringList absoluteToolFilePathsFilter)
{
//  std::cout << "Number of applicationFilter found: " << applicationFilter.size() << std::endl;
//  foreach(QString string, applicationFilter)
//  {
//    std::cout << string_cast(string) << std::endl;
//  }
//  std::cout << "Number of trackingSystemFilter found: " << trackingSystemFilter.size() << std::endl;
//  foreach(QString string, trackingSystemFilter)
//  {
//    std::cout << string_cast(string) << std::endl;
//  }

  mToolListWidget->clear();
  mToolListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  mToolListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QDir dir(DataLocations::getToolsPath());
  QStringList toolFiles = this->getToolFiles(dir);

  foreach(QString toolFilePath, toolFiles)
  {

    Tool::InternalStructure internal = this->getToolInternal(toolFilePath);

    QString trackerName = enum2string(internal.mTrackerType);
    if(!trackingSystemFilter.contains(trackerName, Qt::CaseInsensitive))
      continue;

    bool passedApplicationFilter = false;
    std::vector<ssc::CLINICAL_APPLICATION>::iterator it = internal.mClinicalApplications.begin();
    while(it != internal.mClinicalApplications.end() && !passedApplicationFilter)
    {
      QString applicationName = enum2string(*it);
      if(applicationFilter.contains(applicationName, Qt::CaseInsensitive))
      {
        passedApplicationFilter = true;
//        std::cout << "Filter passed, found: " << trackerName << " and " << applicationName << std::endl;
      }
      ++it;
    }
    if(!passedApplicationFilter)
      continue;

    //check tool agains filters
    if(!absoluteToolFilePathsFilter.isEmpty() && !absoluteToolFilePathsFilter.contains(toolFilePath))
      continue;

    std::cout << "toolFilePath: " << toolFilePath << std::endl;

    QFile file(toolFilePath);
    QFileInfo info(file);
    QListWidgetItem* item = new QListWidgetItem(/*QIcon, */info.dir().dirName());
    item->setData(Qt::ToolTipRole, info.absoluteFilePath()); //TODO Qt::UserRole???

    //add tool to list if it passed the filter
    mToolListWidget->addItem(item);
  }

  //debug start
//  std::cout << "Number of toolfiles found: " << toolFiles.size() << std::endl;
//  foreach(QString string, toolFiles)
//  {
//    std::cout << string_cast(string) << std::endl;
//  }
//
//  mToolListWidget->addItems(toolFiles);
  //debug stop
}

void ToolConfigWidget::populateReferenceComboBox()
{
  mSelectedReferenceComboBox->clear();

  int currentIndex = 0;
  if(mCurrentConfigFile == "<new config>")
  {
    QStringList selectedTools = this->getSelectedToolsFromToolList();
    foreach(QString string, selectedTools)
    {
      Tool::InternalStructure internal = getToolInternal(string);
      if(internal.mType == ssc::Tool::TOOL_REFERENCE)
      {
        currentIndex = this->addRefrenceSuggestion(string);
      }
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
      ssc::messageManager()->sendDebug("Could not determine the reference for configfile: "+configAbsoluteFilePath);
      return;
    }

    currentIndex = this->addRefrenceSuggestion(reference);
  }
  mSelectedReferenceComboBox->setCurrentIndex(currentIndex);
}

int ToolConfigWidget::addConfigurationToComboBox(QString displayName, QString absoluteFilePath)
{
  mConfigFilesComboBox->addItem(displayName);
  int index = mConfigFilesComboBox->findText(displayName);
  mConfigFilesComboBox->setItemData(index, absoluteFilePath, Qt::ToolTipRole);

  return index;
}

int ToolConfigWidget::addRefrenceSuggestion(QString absoluteRefereneFilePath)
{
  int index;

  QFile file(absoluteRefereneFilePath);
  QFileInfo info(file);

  mSelectedReferenceComboBox->addItem(info.dir().dirName());
  //    std::cout << "Added reference: " << info.dir().dirName() << std::endl;
  index = mSelectedReferenceComboBox->findText(info.dir().dirName());
  mSelectedReferenceComboBox->setItemData(index, info.absoluteFilePath(), Qt::ToolTipRole);

  return index;
}

void ToolConfigWidget::filterButtonGroup(QButtonGroup* group, QStringList filter)
{
  bool exclusive = group->exclusive();
  if(exclusive)
    group->setExclusive(false);

  QList<QAbstractButton*> buttons = group->buttons();
  foreach(QAbstractButton* button, buttons)
  {
    QString buttonText = button->text();
    button->setChecked(filter.contains(buttonText, Qt::CaseInsensitive));
  }

  if(exclusive)
    group->setExclusive(true);
}

QStringList ToolConfigWidget::getToolFiles(QDir& dir)
{
//  std::cout << "============================" << std::endl;
  QStringList retval;

  if(!dir.exists())
  {
    ssc::messageManager()->sendError("Dir "+dir.absolutePath()+" does not exits.");
    return retval;
  }
//  std::cout << "Dir: " << dir.absolutePath() << std::endl;

  //find xml files add and return
  dir.setFilter(QDir::Files);
  QStringList nameFilters;
  nameFilters << "*.xml";
  dir.setNameFilters(nameFilters);

  QStringList toolXmlFiles = dir.entryList();
  foreach(QString filename, toolXmlFiles)
  {
    QFile file(dir.absolutePath()+"/"+filename);
    QFileInfo info(file);
    retval << info.absoluteFilePath();
  }

//  std::cout << "Number of toolfiles found: " << retval.size() << std::endl;
//  foreach(QString string, retval)
//  {
//    std::cout << string_cast(string) << std::endl;
//  }

  //find dirs and recursivly check them
  dir.setFilter(QDir::AllDirs);

  //std::cout << "Dir filters: " << dir.filter() << std::endl;

  QStringList subdirs = dir.entryList();
//  std::cout << "Nr of subdirs found: " << subdirs.size() << std::endl;

  foreach(QString dirString, subdirs)
  {
    if(dirString == "." || dirString == "..")
      continue;
    if(dir.cd(dirString))
    {
//      std::cout << "After cd: " << dir.absolutePath() << std::endl;
      retval << this->getToolFiles(dir);
      dir.cdUp();
    }

  }
//  std::cout << "============================" << std::endl;
  return retval;
}

QList<Tool::InternalStructure> ToolConfigWidget::getToolInternals(QStringList toolAbsoluteFilePaths)
{
  QList<Tool::InternalStructure> retval;
  foreach(QString toolFilePath, toolAbsoluteFilePaths)
    retval << this->getToolInternal(toolFilePath);

  return retval;
}

Tool::InternalStructure ToolConfigWidget::getToolInternal(QString toolAbsoluteFilePath)
{
  Tool::InternalStructure retval;

  ToolFileParser parser(toolAbsoluteFilePath);
  retval = parser.getTool();

  return retval;
}

ConfigurationFileParser::Configuration ToolConfigWidget::getCurrentConfiguration()
{
  //TODO
  //only supports one tracker atm

  ConfigurationFileParser::Configuration retval;
  retval.mFileName = this->getConfigFileName();
  retval.mClinical_app = string2enum<ssc::CLINICAL_APPLICATION>(stateManager()->getApplication()->getActiveStateName());

  QStringList selectedTools = this->getSelectedToolsFromToolList();
  QString referencePath = mSelectedReferenceComboBox->itemData(mSelectedReferenceComboBox->currentIndex(), Qt::ToolTipRole).toString();

  ssc::TRACKING_SYSTEM selectedTracker = this->getSelectedTrackingSystem();
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
    tool.second = absoluteToolPath == referencePath;
    toolfilesAndRefVector.push_back(tool);
  }

  retval.mTrackersAndTools[selectedTracker] = toolfilesAndRefVector;

  return retval;
}

QString ToolConfigWidget::getConfigFileName()
{
  QString retval;

  if(mConfigFilesComboBox->currentText() == "<new config>")
  {
    //TODO use the user supplied name if any...
    retval = this->generateConfigName();
  }
  else
    retval = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex(), Qt::ToolTipRole).toString();

  return retval;
}

QString ToolConfigWidget::generateConfigName()
{
  QString retval;

  QStringList applicationFilter = this->getFilterFromButtonGroup(mApplicationButtonGroup);
  QStringList trackingsystemFilter = this->getFilterFromButtonGroup(mTrackerButtonGroup);
  QStringList absoluteToolFilePathsFilter = this->getSelectedToolsFromToolList();

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

  retval = absoluteDirPath+trackingSystems+tools+".xml";

  return retval;
}

QStringList ToolConfigWidget::getFilterFromButtonGroup(QButtonGroup* group)
{
  QStringList retval;

  QList<QAbstractButton*> buttons = group->buttons();
  foreach(QAbstractButton* button, buttons)
  {
    if(button->isChecked())
    retval << button->text();
  }

  return retval;
}

QStringList ToolConfigWidget::getSelectedToolsFromToolList()
{
  QStringList retval;

  QList<QListWidgetItem *> selectedToolItems = mToolListWidget->selectedItems();
  foreach(QListWidgetItem* item, selectedToolItems)
  {
    retval << item->data(Qt::ToolTipRole).toString();
  }

  return retval;
}

ssc::TRACKING_SYSTEM ToolConfigWidget::getSelectedTrackingSystem()
{
  ssc::TRACKING_SYSTEM retval;
  retval = string2enum<ssc::TRACKING_SYSTEM>(mTrackerButtonGroup->checkedButton()->text());

  return retval;
}

}//namespace cx
