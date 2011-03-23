#include <cxToolConfigWidget.h>

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

namespace cx
{

ToolConfigWidget::ToolConfigWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mConfigFilesComboBox(new QComboBox()),
    mTrackerGroupBox(new QGroupBox()),
    mTrackerButtonGroup(new QButtonGroup()),
    mToolGroup(new QGroupBox()),
    mToolListWidget(new QListWidget()),
    mApplicationFilterGroupBox(new QGroupBox()),
    mApplicationFilterButtonGroup(new QButtonGroup())
{
  //layout
  QGridLayout* layout = new QGridLayout(this);

  layout->addWidget(new QLabel("Config files:"), 0, 0);
  layout->addWidget(mConfigFilesComboBox, 0, 1);

  layout->addWidget(mApplicationFilterGroupBox, 1, 0, 1, 2);

  layout->addWidget(mTrackerGroupBox, 2, 0, 1, 2);

  layout->addWidget(mToolGroup, 3, 0, 1, 2);

  mApplicationFilterGroupBox->setTitle("Applications");
  QHBoxLayout* applicationlayout = new QHBoxLayout();
  mApplicationFilterGroupBox->setLayout(applicationlayout);

  mTrackerGroupBox->setTitle("Tracking systems");
  QHBoxLayout* trackingsystemlayout = new QHBoxLayout();
  mTrackerGroupBox->setLayout(trackingsystemlayout);

  mToolGroup->setTitle("Tools");
  QVBoxLayout* toolslayout = new QVBoxLayout();
  mToolGroup->setLayout(toolslayout);
  toolslayout->addWidget(mToolListWidget);

  //connect
  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

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

void ToolConfigWidget::applicationStateChangedSlot()
{
  this->populateConfigComboBox();
}

void ToolConfigWidget::filterToolsSlot()
{
  QStringList applicationFilter;
  QList<QAbstractButton*> applicationButtonList = mApplicationFilterButtonGroup->buttons();
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

void ToolConfigWidget::populateConfigComboBox()
{
  mConfigFilesComboBox->blockSignals(true);
  QDir dir(DataLocations::getApplicationToolConfigPath());
  dir.setFilter(QDir::Files);

  QStringList nameFilters;
  nameFilters << "*.xml";
  dir.setNameFilters(nameFilters);

  QStringList list = dir.entryList();

  mConfigFilesComboBox->clear();
  mConfigFilesComboBox->addItems(list);

  int currentIndex = mConfigFilesComboBox->findText(mCurrentConfigFile);
  mConfigFilesComboBox->setCurrentIndex( currentIndex );
  mConfigFilesComboBox->blockSignals(false);
}

void ToolConfigWidget::populateApplicationFilter()
{
  QStringList applicationList = stateManager()->getApplication()->getAllApplicationNames();

  mApplicationFilterButtonGroup->setExclusive(false);
  foreach(QString string, applicationList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mApplicationFilterButtonGroup->addButton(box);
    mApplicationFilterGroupBox->layout()->addWidget(box);
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

void ToolConfigWidget::populateToolList(QStringList applicationFilter, QStringList trackingSystemFilter)
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

  mToolListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  mToolListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

  QDir dir(DataLocations::getToolsPath());
  QStringList toolFiles = this->getToolFiles(dir);

  foreach(QString toolFilePath, toolFiles)
  {
    //check tool agains filters
    Tool::InternalStructure internal = this->getToolInternal(toolFilePath);
    QString trackerName = enum2string(internal.mTrackerType);
    if(!trackingSystemFilter.contains(trackerName, Qt::CaseInsensitive))
      continue;

    bool passedFilter = false;
    for(std::vector<ssc::MEDICAL_DOMAIN>::iterator it = internal.mMedicalDomains.begin(); it != internal.mMedicalDomains.end(); ++it)
    {
      if(passedFilter)
        break;

      QString domainName = enum2string(*it);
      if(applicationFilter.contains(domainName, Qt::CaseInsensitive))
      {
        passedFilter = true;
        std::cout << "Filter passed, found: " << trackerName << " and " << domainName << std::endl;
      }
    }

    //add tool to list if it passed the filter
    QFile file(toolFilePath);
    QFileInfo info(file);
    QListWidgetItem* item = new QListWidgetItem(/*QIcon, */info.dir().dirName());
    item->setData(Qt::ToolTipRole, info.absoluteFilePath()); //TODO Qt::UserRole???
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

  //TODO
  //fix parser
  //ssc::messageManager()->sendDebug("TODO: Need to parse tool file: "+toolAbsoluteFilePath);

  return retval;
}

}//namespace cx
