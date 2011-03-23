#include <cxToolConfigWidget.h>

#include <QDir>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxDataLocations.h"
#include "cxStateMachineManager.h"
#include "cxToolManager.h"

namespace cx
{

ToolConfigWidget::ToolConfigWidget(QWidget* parent) :
    WhatsThisWidget(parent),
    mConfigFilesComboBox(new QComboBox()),
    mTrackingGroup(new QGroupBox()),
    mTrackingButtonGroup(new QButtonGroup()),
    mToolGroup(new QGroupBox()),
    mToolFilterComboBox(new QComboBox())
{
  QGridLayout* layout = new QGridLayout(this);

  layout->addWidget(new QLabel("Config files:"), 0, 0);
  layout->addWidget(mConfigFilesComboBox, 0, 1);

  layout->addWidget(new QLabel("Application filter:"), 1, 0);
  layout->addWidget(mToolFilterComboBox, 1, 1);

  layout->addWidget(mTrackingGroup, 2, 0, 1, 2);

  connect(stateManager()->getApplication().get(), SIGNAL(activeStateChanged()), this, SLOT(applicationStateChangedSlot()));

  this->populateConfigComboBox();
  this->populateApplicationFilterComboBox();
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

void ToolConfigWidget::populateApplicationFilterComboBox()
{
  QStringList list = stateManager()->getApplication()->getAllApplicationNames();
  list.push_front("All");

  mToolFilterComboBox->clear();
  mToolFilterComboBox->addItems(list);

  int currentIndex = mToolFilterComboBox->findText(stateManager()->getApplication()->getActiveStateName());
  mToolFilterComboBox->setCurrentIndex(currentIndex);
}

void ToolConfigWidget::populateTrackingSystems()
{
  mTrackingGroup->setTitle("Tracking systems");
  QHBoxLayout* layout = new QHBoxLayout();
  mTrackingGroup->setLayout(layout);

  QStringList trackingSystemList = ToolManager::getInstance()->getSupportedTrackingSystems();

  mTrackingButtonGroup->setExclusive(true);
  foreach(QString string, trackingSystemList)
  {
    if(string.isEmpty())
      continue;

    string = string.toLower();
    string[0] = string[0].toUpper();

    QCheckBox* box = new QCheckBox(string);
    mTrackingButtonGroup->addButton(box);
    layout->addWidget(box);
  }
}

void ToolConfigWidget::populateToolList()
{
  mToolGroup->setTitle("Tools");

  QDir dir(DataLocations::getToolsPath());
  QStringList toolFiles = this->getToolFiles(dir);

  //debug start
  std::cout << "Number of toolfiles found: " << toolFiles.size() << std::endl;
  foreach(QString string, toolFiles)
  {
    std::cout << string_cast(string) << std::endl;
  }
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
  foreach(QString string, retval)
  {
    std::cout << string_cast(string) << std::endl;
  }

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
}//namespace cx
