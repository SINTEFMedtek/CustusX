#include <cxToolConfigWidget.h>

#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QListWidget>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "sscEnumConverter.h"
#include "cxDataLocations.h"
#include "cxStateMachineManager.h"
#include "cxToolManager.h"
#include "cxToolConfigurationParser.h"
#include "cxFilePreviewWidget.h"

namespace cx
{

ToolConfigWidget::ToolConfigWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mConfigFilesComboBox(new QComboBox()),
    mCurrentConfigFile("<new config>"),
    mTrackerGroupBox(new QGroupBox()),
    mTrackerButtonGroup(new QButtonGroup()),
    mToolGroup(new QGroupBox()),
    mToolListWidget(new QListWidget()),
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

  //connect
  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));
  connect(mConfigFilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(configChangedSlot()));
  connect(mToolListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(toolClickedSlot(QListWidgetItem*)));
  connect(mToolListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(toolDoubleClickedSlot(QListWidgetItem*)));
  connect(this, SIGNAL(toolSelected(QString)), this, SLOT(fileSelectedSlot(QString)));

  //populate
  this->populateConfigComboBox();
  this->populateApplicationFilter();
  this->populateTrackingSystems();
  this->populateToolList();

}

ToolConfigWidget::~ToolConfigWidget()
{
}

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

  QString absoluteConfigFilePath = mConfigFilesComboBox->itemData(mConfigFilesComboBox->currentIndex()).toString();
  emit toolSelected(absoluteConfigFilePath);

  QStringList applicationFilter;
  QStringList trackingsystemFilter;
  QStringList absoluteToolFilePathsFilter;
  if(mCurrentConfigFile != "<new config>")
  {
    ConfigurationFileParser parser(absoluteConfigFilePath);
    //block signals???
    ssc::MEDICAL_DOMAIN domain = parser.getApplicationDomain();
    applicationFilter << enum2string(domain);

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
//      std::cout << "added " << absoluteToolFilePaths[i] << " to tool filter..." << std::endl;
    }
  }
  this->filterButtonGroup(mApplicationButtonGroup, applicationFilter);
  this->filterButtonGroup(mTrackerButtonGroup, trackingsystemFilter);
  this->populateToolList(applicationFilter, trackingsystemFilter, absoluteToolFilePathsFilter);
}

void ToolConfigWidget::toolClickedSlot(QListWidgetItem* item)
{
  QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
  emit toolSelected(absoluteFilePath);
}

void ToolConfigWidget::toolDoubleClickedSlot(QListWidgetItem* item)
{
  //TODO move to filepreviewwidget???
  QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
  QUrl url("file:/"+absoluteFilePath);
  QDesktopServices::openUrl(url);
}

void ToolConfigWidget::fileSelectedSlot(QString fileSelected)
{
  mCurrentlySelectedFile = fileSelected;
}

void ToolConfigWidget::populateConfigComboBox()
{
//  mConfigFilesComboBox->blockSignals(true);
  QDir dir(DataLocations::getApplicationToolConfigPath());
  dir.setFilter(QDir::Files);

  QStringList nameFilters;
  nameFilters << "*.xml";
  dir.setNameFilters(nameFilters);

  mConfigFilesComboBox->clear();
  mConfigFilesComboBox->addItem("<new config>", QVariant(ConfigurationFileParser::getTemplatesAbsoluteFilePath()));
  QStringList configlist = dir.entryList();
  foreach(QString filename, configlist)
  {
    QFile file(dir.absolutePath()+"/"+filename);
    QFileInfo info(file);
    mConfigFilesComboBox->addItem(filename, QVariant(info.absoluteFilePath()));
  }

  int currentIndex = mConfigFilesComboBox->findText(mCurrentConfigFile);
  mConfigFilesComboBox->setCurrentIndex(currentIndex);
//  mConfigFilesComboBox->blockSignals(false);
}

void ToolConfigWidget::populateApplicationFilter()
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
  }
}

void ToolConfigWidget::populateToolList(QStringList applicationFilter, QStringList trackingSystemFilter, QStringList absoluteToolFilePathsFilter)
{
//    std::cout << "Number of applicationFilter found: " << applicationFilter.size() << std::endl;
//    foreach(QString string, applicationFilter)
//    {
//      std::cout << string_cast(string) << std::endl;
//    }
//    std::cout << "Number of trackingSystemFilter found: " << trackingSystemFilter.size() << std::endl;
//    foreach(QString string, trackingSystemFilter)
//    {
//      std::cout << string_cast(string) << std::endl;
//    }

  mToolListWidget->clear();
  mToolListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  mToolListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QDir dir(DataLocations::getToolsPath());
  QStringList toolFiles = this->getToolFiles(dir);

  foreach(QString toolFilePath, toolFiles)
  {
    //check tool agains filters
    if(!absoluteToolFilePathsFilter.contains(toolFilePath))
      continue;

    Tool::InternalStructure internal = this->getToolInternal(toolFilePath);
    QString trackerName = enum2string(internal.mTrackerType);
    if(!trackingSystemFilter.contains(trackerName, Qt::CaseInsensitive))
      continue;

    bool passedApplicationFilter = false;
    for(std::vector<ssc::MEDICAL_DOMAIN>::iterator it = internal.mMedicalDomains.begin(); it != internal.mMedicalDomains.end(); ++it)
    {
      if(passedApplicationFilter)
        break;

      QString domainName = enum2string(*it);
      if(applicationFilter.contains(domainName, Qt::CaseInsensitive))
      {
        passedApplicationFilter = true;
        //std::cout << "Filter passed, found: " << trackerName << " and " << domainName << std::endl;
      }
    }

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

void ToolConfigWidget::filterButtonGroup(QButtonGroup* group, QStringList filter)
{
  QList<QAbstractButton*> buttons = group->buttons();
  foreach(QAbstractButton* button, buttons)
  {
    QString buttonText = button->text();
    button->setChecked(filter.contains(buttonText, Qt::CaseInsensitive));
  }
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
      //std::cout << "After cd: " << dir.absolutePath() << std::endl;
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

}//namespace cx
