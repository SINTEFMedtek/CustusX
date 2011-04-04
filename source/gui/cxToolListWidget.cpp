#include "cxToolListWidget.h"

#include <QListWidgetItem>
#include <QDir>
#include <QDropEvent>
#include <QMimeData>
#include "sscEnumConverter.h"
#include "sscMessageManager.h"
#include "cxDataLocations.h"
#include "cxToolConfigurationParser.h"

namespace cx
{

ToolListWidget::ToolListWidget(QWidget* parent) :
    QListWidget(parent)
{
  connect(this, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(toolClickedSlot(QListWidgetItem*)));

  this->setSelectionBehavior(QAbstractItemView::SelectItems);
  this->setSelectionMode(QAbstractItemView::SingleSelection);
}

ToolListWidget::~ToolListWidget()
{}

void ToolListWidget::populate(QStringList toolsAbsoluteFilePath)
{
  this->clear();

  foreach(QString tool, toolsAbsoluteFilePath)
  {
    QFile file(tool);
    QFileInfo info(file);
    QListWidgetItem* item = new QListWidgetItem(/*QIcon, */info.dir().dirName());
    item->setData(Qt::ToolTipRole, info.absoluteFilePath());
    this->addItem(item);
  }
  emit listSizeChanged();
}

Tool::InternalStructure ToolListWidget::getToolInternal(QString toolAbsoluteFilePath)
{
  Tool::InternalStructure retval;

  ToolFileParser parser(toolAbsoluteFilePath);
  retval = parser.getTool();

  return retval;
}

void ToolListWidget::toolClickedSlot(QListWidgetItem* item)
{
  QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
  emit toolSelected(absoluteFilePath);
}

//---------------------------------------------------------------------------------------------------------------------

FilteringToolListWidget::FilteringToolListWidget(QWidget* parent) :
    ToolListWidget(parent)
{
  this->setDragDropMode(QAbstractItemView::DragOnly);
  this->setDragEnabled(true);
}

FilteringToolListWidget::~FilteringToolListWidget()
{}

void FilteringToolListWidget::filterSlot(QStringList applicationsFilter, QStringList trackingsystemsFilter)
{
  QDir toolDir(DataLocations::getToolsPath());
  QStringList allTools = this->getAbsoluteFilePathToAllTools(toolDir);
  QStringList filteredTools = this->filter(allTools, applicationsFilter, trackingsystemsFilter);

  this->populate(filteredTools);
}

QStringList FilteringToolListWidget::getAbsoluteFilePathToAllTools(QDir dir)
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
        retval << this->getAbsoluteFilePathToAllTools(dir);
        dir.cdUp();
      }

    }
  //  std::cout << "============================" << std::endl;
    return retval;
}

QStringList FilteringToolListWidget::filter(QStringList toolsToFilter, QStringList applicationsFilter, QStringList trackingsystemsFilter)
{
  QStringList retval;

  foreach(QString toolFilePath, toolsToFilter)
  {
    //get internal tool
    Tool::InternalStructure internal = this->getToolInternal(toolFilePath);

    //check tracking systems
    QString trackerName = enum2string(internal.mTrackerType);
    if(!trackingsystemsFilter.contains(trackerName, Qt::CaseInsensitive))
      continue;

    //check applications
    bool passedApplicationFilter = false;
    std::vector<ssc::CLINICAL_APPLICATION>::iterator it = internal.mClinicalApplications.begin();
    while(it != internal.mClinicalApplications.end() && !passedApplicationFilter)
    {
      QString applicationName = enum2string(*it);
      if(applicationsFilter.contains(applicationName, Qt::CaseInsensitive))
      {
        passedApplicationFilter = true;
  //        std::cout << "Filter passed, found: " << trackerName << " and " << applicationName << std::endl;
      }
      ++it;
    }
    if(!passedApplicationFilter)
      continue;

    //add if filters passed
    retval << toolFilePath;
  }

  return retval;
}
//---------------------------------------------------------------------------------------------------------------------

ConfigToolListWidget::ConfigToolListWidget(QWidget* parent) :
    ToolListWidget(parent)
{
  //TODO how to delete items from the list
  this->setDefaultDropAction(Qt::CopyAction);
  this->setDragDropMode(QAbstractItemView::DropOnly);
//  this->setDragDropOverwriteMode(true);
//  this->setDragEnabled(false);
}

ConfigToolListWidget::~ConfigToolListWidget()
{}

void ConfigToolListWidget::dropEvent(QDropEvent* event)
{
  QListWidget::dropEvent(event);

  //TODO should prevent duplication of items... reimplement the drop slot???
  std::cout << "something was dropped..." << std::endl;

  emit userChangedList();
  emit listSizeChanged();
}

QStringList ConfigToolListWidget::getTools()
{
  QStringList retval;

  for(int i=0; i < this->count(); ++i)
  {
    QListWidgetItem* item = this->item(i);
    retval << item->data(Qt::ToolTipRole).toString();
  }

  return retval;
}

void ConfigToolListWidget::configSlot(QStringList toolsAbsoluteFilePath)
{
  this->populate(toolsAbsoluteFilePath);
}

void ConfigToolListWidget::filterSlot(QStringList trackingsystemFilter)
{
  for(int i=0; i < this->count(); ++i)
  {
    QListWidgetItem* item = this->item(i);
    QString absoluteFilePath = item->data(Qt::ToolTipRole).toString();
    ToolFileParser parser(absoluteFilePath);
    QBrush brush = item->foreground();
    QString toolTrackingSystem = enum2string(parser.getTool().mTrackerType);
    if(!trackingsystemFilter.contains(toolTrackingSystem, Qt::CaseInsensitive))
      brush.setColor(Qt::red);
    else
      brush.setColor(Qt::black);

    item->setForeground(brush);
  }
}

} //namespace cx
