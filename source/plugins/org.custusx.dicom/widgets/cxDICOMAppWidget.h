/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __cxDICOMAppWidget_h
#define __cxDICOMAppWidget_h

#include "org_custusx_dicom_Export.h"

// Qt includes 
#include <QWidget>

class ctkThumbnailLabel;
class QModelIndex;
class ctkDICOMDatabase;
class QItemSelection;

namespace cx
{
class DICOMAppWidgetPrivate;


/**
 * Adapted for use in CustusX from ctkDICOMAppWidget
 *
 * \ingroup org_custusx_dicom
 *
 * \date 2014-05-20
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DICOMAppWidget : public QWidget
{
  Q_OBJECT

public:
  typedef QWidget Superclass;
  explicit DICOMAppWidget(QWidget* parent=0);
  virtual ~DICOMAppWidget();

  /// Directory being used to store the dicom database
  QString databaseDirectory() const;

  /// See ctkDICOMDatabase for description - these accessors
  /// delegate to the corresponding routines of the internal
  /// instance of the database.
  /// @see ctkDICOMDatabase
  void setTagsToPrecache(const QStringList tags);
  const QStringList tagsToPrecache();

  /// Updates schema of loaded database to match the one
  /// coded by the current version of ctkDICOMDatabase.
  /// Also provides a dialog box for progress
  void updateDatabaseSchemaIfNeeded();

  QStringList getSelectedPatients();
  QStringList getSelectedStudies();
  QStringList getSelectedSeries();

  ctkDICOMDatabase* database();

  /// Option to show or not import summary dialog.
  /// Since the summary dialog is modal, we give the option
  /// of disabling it for batch modes or testing.
  void setDisplayImportSummary(bool);
  bool displayImportSummary();
  /// Accessors to status of last directory import operation
  int patientsAddedDuringImport();
  int studiesAddedDuringImport();
  int seriesAddedDuringImport();
  int instancesAddedDuringImport();
  void addActionToToolbar(QAction* action);

  QList<QAction*> getAdvancedActions() const;

public Q_SLOTS:
  void setDatabaseDirectory(const QString& directory);
  void onSelectionChanged(const QItemSelection&, const QItemSelection&);

  void openQueryDialog();
  void onRemoveAction();

//  void suspendModel();
  void resumeModel();
  void resetModel();

  /// Import a directory - this is used when the user selects a directory
  /// from the Import Dialog, but can also be used externally to trigger
  /// an import (i.e. for testing or to support drag-and-drop)
  void onImportDirectory(QString directory);

  void onCurrentChanged(const QModelIndex&, const QModelIndex&);

Q_SIGNALS:
  void databaseDirectoryChanged(const QString&);
  void queryRetrieveFinished();
  void directoryImported();

protected:
    QScopedPointer<DICOMAppWidgetPrivate> d_ptr;
protected Q_SLOTS:
    void onModelSelected(const QModelIndex& index);
    void onQueryRetrieveFinished();
    void onTreeCollapsed(const QModelIndex& index);
    void onTreeExpanded(const QModelIndex& index);
    void onThumbnailWidthSliderValueChanged(int val);




private Q_SLOTS:

	void schemaUpdateStarted(int);
	void schemaUpdateProgress(QString);
	void schemaUpdateProgress(int);
	void schemaUpdated();

private:
  Q_DECLARE_PRIVATE(DICOMAppWidget);
  Q_DISABLE_COPY(DICOMAppWidget);

};

} // namespace cx


#endif
