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
#ifndef CXDATAVIEWSELECTIONWIDGET_H_
#define CXDATAVIEWSELECTIONWIDGET_H_

#include "cxGuiExport.h"

#include <QListWidget>
#include "cxForwardDeclarations.h"
class QLabel;

namespace cx
{
/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/**
 * Base class for displaying Data items.
 */
class cxGui_EXPORT DataListWidget : public QListWidget
{
  Q_OBJECT

public:
  DataListWidget(PatientModelServicePtr patientModelService, QWidget* parent = NULL);
  virtual ~DataListWidget();
  virtual QSize sizeHint() const;

signals:
  void userChangedList(); ///< emitted whenever the user changes the list
  void listSizeChanged(); ///< emitted whenever the count changes

protected:
  void populate(QStringList dataUids);
  void populateData(QString uid, bool indent=false, QListWidgetItem* after = NULL);

  PatientModelServicePtr mPatientModelService;

private slots:
  void itemSelectionChangedSlot();
};

//---------------------------------------------------------------------------------------------------------------------

class cxGui_EXPORT AllDataListWidget : public DataListWidget
{
  Q_OBJECT

public:
  AllDataListWidget(PatientModelServicePtr patientModelService, QWidget* parent = NULL);
  virtual ~AllDataListWidget();

protected:
  void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private slots:
  void populateAllDataList();

private:
  QPoint startPos;
};

//---------------------------------------------------------------------------------------------------------------------

class cxGui_EXPORT SelectedDataListWidget : public DataListWidget
{
  Q_OBJECT

public:
  SelectedDataListWidget(PatientModelServicePtr patientModelService, QWidget* parent = NULL);
  virtual ~SelectedDataListWidget();
  void setViewGroupData(ViewGroupDataPtr viewGroupData);

  virtual void dropEvent(QDropEvent* event);
  QStringList getData(); ///< get absolute file path to all tools currently in the list

public slots:
  void populateList();
  void userChangedListSlot();

private slots:
  void deleteSlot();
  void deleteItemSlot(QListWidgetItem* item);
  void contextMenuSlot(const QPoint & point);
  void keyPressEvent(QKeyEvent* event);

protected:
	QMap<int, QVariant> convertFromCustomQtMimeFormat(const QMimeData* mimeData) const;

private:
  QListWidgetItem* mItemToDelete;
  ViewGroupDataPtr mViewGroupData;
  QPoint startPos;
};

/**Widget for selecting which data items to show in a given view group,
 * and how to order them.
 *
 */
class cxGui_EXPORT DataViewSelectionWidget : public QWidget
{
  Q_OBJECT
public:
  DataViewSelectionWidget(PatientModelServicePtr patientModelService, VisualizationServicePtr visualizationService, QWidget* parent = NULL);
  virtual ~DataViewSelectionWidget();

private slots:
  void viewGroupChangedSlot();

private:
  SelectedDataListWidget* mSelectedDataListWidget;
  AllDataListWidget* mAllDataListWidget;
  QLabel* mVisibleLabel;
  VisualizationServicePtr mVisualizationService;
};

/**
 * @}
 */
}

#endif /* CXDATAVIEWSELECTIONWIDGET_H_ */
