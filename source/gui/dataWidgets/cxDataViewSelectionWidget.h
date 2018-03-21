/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
  DataViewSelectionWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent = NULL);
  virtual ~DataViewSelectionWidget();

private slots:
  void viewGroupChangedSlot();

private:
  SelectedDataListWidget* mSelectedDataListWidget;
  AllDataListWidget* mAllDataListWidget;
  QLabel* mVisibleLabel;
  ViewServicePtr mViewService;
};

/**
 * @}
 */
}

#endif /* CXDATAVIEWSELECTIONWIDGET_H_ */
