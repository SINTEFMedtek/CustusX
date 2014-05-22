// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXDICOMIMPORTER_H
#define CXDICOMIMPORTER_H

// Qt includes
#include <QString>
#include <QObject>
#include <QSharedPointer>

class ctkDICOMDatabase;
class ctkDICOMIndexer;
class ctkFileDialog;
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
class DicomImporter : public QObject
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

	ctkFileDialog* ImportDialog;
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
