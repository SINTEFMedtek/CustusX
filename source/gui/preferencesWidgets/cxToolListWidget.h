/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTOOLLISTWIDGET_H_
#define CXTOOLLISTWIDGET_H_

#include "cxGuiExport.h"

#include <QListWidget>
#include "cxForwardDeclarations.h"

class QDir;
class QListWidgetItem;

namespace cx
{
/**
 * ToolListWidget
 *
 * \brief Class for displaying tool files that can be dragged and dropped
 * \ingroup cx_gui
 *
 * Superclass, not used directly.
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT ToolListWidget : public QListWidget
{
  Q_OBJECT

public:
  ToolListWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
  virtual ~ToolListWidget();

signals:
  void toolSelected(QString absoluteFilePath);
  void userChangedList(); ///< emitted whenever the user changes the list
  void listSizeChanged(); ///< emitted whenever the count changes

protected:
  void populate(QStringList toolsAbsoluteFilePath);
  void addTool(QString absoluteFilePath);

  TrackingServicePtr mTrackingService;

private slots:
  void selectionChangedSlot();
  void toolSelectedSlot(QListWidgetItem* item);
};

//---------------------------------------------------------------------------------------------------------------------

/**
 * \brief Helper class for tool list display
 *
 * Helper class.
 *
 * Used inside ToolFilterGroupBox.
 *
 */
class cxGui_EXPORT FilteringToolListWidget : public ToolListWidget
{
  Q_OBJECT

public:
  FilteringToolListWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
  virtual ~FilteringToolListWidget();

  virtual QSize minimumSizeHint() const;

public slots:
  void filterSlot(QStringList applicationsFilter, QStringList trackingsystemsFilter);

protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void startDrag();

private:
  QStringList filter(QStringList toolsToFilter, QStringList applicationsFilter, QStringList trackingsystemsFilter); ///<
  QPoint startPos;
};

//---------------------------------------------------------------------------------------------------------------------

/**
 * \brief Helper class for tool config list display
 *
 * Helper class.
 *
 * Used inside ToolConfigureGroupBox.
 *
 */
class cxGui_EXPORT ConfigToolListWidget : public ToolListWidget
{
  Q_OBJECT

public:
  ConfigToolListWidget(TrackingServicePtr trackingService, QWidget* parent = NULL);
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
