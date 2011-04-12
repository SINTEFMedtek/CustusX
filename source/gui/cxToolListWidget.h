#ifndef CXTOOLLISTWIDGET_H_
#define CXTOOLLISTWIDGET_H_

#include <QListWidget>

#include "cxTool.h"
class QDir;
class QListWidgetItem;

namespace cx
{
/**
 * ToolListWidget
 *
 * \brief Class for displaying tool files that can be dragged and dropped
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class ToolListWidget : public QListWidget
{
  Q_OBJECT

public:
  ToolListWidget(QWidget* parent = NULL);
  virtual ~ToolListWidget();

signals:
  void toolSelected(QString absoluteFilePath);
  void userChangedList(); ///< emitted whenever the user changes the list
  void listSizeChanged(); ///< emitted whenever the count changes

protected:
  void populate(QStringList toolsAbsoluteFilePath);
  void addTool(QString absoluteFilePath);

  Tool::InternalStructure getToolInternal(QString toolAbsoluteFilePath);

private slots:
  void selectionChangedSlot();
  void toolSelectedSlot(QListWidgetItem* item);
};

//---------------------------------------------------------------------------------------------------------------------

class FilteringToolListWidget : public ToolListWidget
{
  Q_OBJECT

public:
  FilteringToolListWidget(QWidget* parent = NULL);
  virtual ~FilteringToolListWidget();

public slots:
  void filterSlot(QStringList applicationsFilter, QStringList trackingsystemsFilter);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void startDrag();

private:
  QStringList getAbsoluteFilePathToAllTools(QDir dir); ///< get absolute file path to all tool.xml files in folder dir and all subfolders
  QStringList filter(QStringList toolsToFilter, QStringList applicationsFilter, QStringList trackingsystemsFilter); ///<
  QPoint startPos;
};

//---------------------------------------------------------------------------------------------------------------------

class ConfigToolListWidget : public ToolListWidget
{
  Q_OBJECT

public:
  ConfigToolListWidget(QWidget* parent = NULL);
  virtual ~ConfigToolListWidget();

  virtual void dropEvent(QDropEvent* event);
  QStringList getTools(); ///< get absolute file path to all tools currently in the list

public slots:
  void configSlot(QStringList toolsAbsoluteFilePath); ///< adds all input tools to the list
  void filterSlot(QStringList trackingsystemFilter); ///< filters the tools on tracking system

private slots:
  void deleteSlot();
  void deleteItemSlot(QListWidgetItem* item);
  void contextMenuSlot(const QPoint & point);

protected:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);

private:
  QListWidgetItem* mItemToDelete;
};

} //namespace cx
#endif /* CXTOOLLISTWIDGET_H_ */
