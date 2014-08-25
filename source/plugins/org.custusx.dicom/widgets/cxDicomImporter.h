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
