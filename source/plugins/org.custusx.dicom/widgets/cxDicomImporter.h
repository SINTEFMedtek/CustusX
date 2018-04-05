/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXDICOMIMPORTER_H
#define CXDICOMIMPORTER_H

#include "org_custusx_dicom_Export.h"

// Qt includes
#include <QString>
#include <QObject>
#include <QSharedPointer>

class ctkDICOMDatabase;
class ctkDICOMIndexer;
class QProgressDialog;

namespace cx
{

/** 
 *
 *
 * \ingroup org_custusx_dicom
 * \date 2014-05-21
 * \author Christian Askeland
 */
class org_custusx_dicom_EXPORT DicomImporter : public QObject
{
	Q_OBJECT
public:
	DicomImporter(QObject* parent=NULL);
	void setDatabase(QSharedPointer<ctkDICOMDatabase> database);
	~DicomImporter();

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


public slots:
	  /// Import a directory - this is used when the user selects a directory
	  /// from the Import Dialog, but can also be used externally to trigger
	  /// an import (i.e. for testing or to support drag-and-drop)
	  void onImportDirectory(QString directory);

signals:
	void directoryImported();
	void indexingCompleted();
	void fileIndexed(QString);


private:
	void showIndexerDialog();

	QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
	QSharedPointer<ctkDICOMIndexer> DICOMIndexer;
	QProgressDialog *IndexerProgress;

	// local count variables to keep track of the number of items
	// added to the database during an import operation
	bool DisplayImportSummary;
	int PatientsAddedDuringImport;
	int StudiesAddedDuringImport;
	int SeriesAddedDuringImport;
	int InstancesAddedDuringImport;

private slots:
	void onFileIndexed(const QString& filePath);
	void openImportDialog();

	/// slots to capture status updates from the database during an
	/// import operation
	void onPatientAdded(int, QString, QString, QString);
	void onStudyAdded(QString);
	void onSeriesAdded(QString);
	void onInstanceAdded(QString);

};


} // namespace cx


#endif // CXDICOMIMPORTER_H
