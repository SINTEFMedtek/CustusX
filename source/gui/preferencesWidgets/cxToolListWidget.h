/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
