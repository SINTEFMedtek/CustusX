/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDicomImporter.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLabel>
#include <QFileDialog>
// ctkWidgets includes
// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

#include "cxReporter.h"

namespace cx
{

DicomImporter::DicomImporter(QObject* parent): QObject(parent)
{
	DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
	IndexerProgress = 0;
	DisplayImportSummary = true;
	PatientsAddedDuringImport = 0;
	StudiesAddedDuringImport = 0;
	SeriesAddedDuringImport = 0;
	InstancesAddedDuringImport = 0;
}

DicomImporter::~DicomImporter()
{
	delete IndexerProgress;
}

void DicomImporter::setDatabase(QSharedPointer<ctkDICOMDatabase> database)
{
	if (DICOMDatabase)
	{
		disconnect(DICOMDatabase.data(), SIGNAL(patientAdded(int,QString,QString,QString)), this,
				   SLOT(onPatientAdded(int,QString,QString,QString)));
		disconnect(DICOMDatabase.data(), SIGNAL(studyAdded(QString)), this, SLOT(onStudyAdded(QString)));
		disconnect(DICOMDatabase.data(), SIGNAL(seriesAdded(QString)), this, SLOT(onSeriesAdded(QString)));
		disconnect(DICOMDatabase.data(), SIGNAL(instanceAdded(QString)), this, SLOT(onInstanceAdded(QString)));
	}

	DICOMDatabase = database;

	if (DICOMDatabase)
	{
		connect(DICOMDatabase.data(), SIGNAL(patientAdded(int,QString,QString,QString)), this,
				SLOT(onPatientAdded(int,QString,QString,QString)));
		connect(DICOMDatabase.data(), SIGNAL(studyAdded(QString)), this, SLOT(onStudyAdded(QString)));
		connect(DICOMDatabase.data(), SIGNAL(seriesAdded(QString)), this, SLOT(onSeriesAdded(QString)));
		connect(DICOMDatabase.data(), SIGNAL(instanceAdded(QString)), this, SLOT(onInstanceAdded(QString)));
	}
}

void DicomImporter::showIndexerDialog()
{
  if (IndexerProgress == 0)
	{
	//
	// Set up the Indexer Progress Dialog
	//
	IndexerProgress = new QProgressDialog( "DICOM Import", "Cancel", 0, 100, NULL,
		 Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

	// We don't want the progress dialog to resize itself, so we bypass the label
	// by creating our own
	QLabel* progressLabel = new QLabel("Initialization...");
	IndexerProgress->setLabel(progressLabel);
	IndexerProgress->setWindowModality(Qt::ApplicationModal);
	IndexerProgress->setMinimumDuration(0);
	IndexerProgress->setValue(0);

	connect(IndexerProgress, SIGNAL(canceled()),
				 DICOMIndexer.data(), SLOT(cancel()));

	connect(DICOMIndexer.data(), SIGNAL(progress(int)),
			IndexerProgress, SLOT(setValue(int)));
	connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
			progressLabel, SLOT(setText(QString)));
	connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
			this, SIGNAL(fileIndexed(QString)));
	connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
			this, SLOT(onFileIndexed(QString)));

	// close the dialog
	connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
			IndexerProgress, SLOT(close()));
	// reset the database to show new data
	connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
			this, SIGNAL(indexingCompleted()));
	// stop indexing and reset the database if canceled
	connect(IndexerProgress, SIGNAL(canceled()),
			DICOMIndexer.data(), SLOT(cancel()));
	connect(IndexerProgress, SIGNAL(canceled()),
			this, SIGNAL(indexingCompleted()));

	// allow users of this widget to know that the process has finished
	connect(IndexerProgress, SIGNAL(canceled()),
			this, SIGNAL(directoryImported()));
	connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
			this, SIGNAL(directoryImported()));
	}
  IndexerProgress->show();
}

bool DicomImporter::displayImportSummary()
{
  return DisplayImportSummary;
}

void DicomImporter::setDisplayImportSummary(bool onOff)
{
  DisplayImportSummary = onOff;
}

int DicomImporter::patientsAddedDuringImport()
{
  return PatientsAddedDuringImport;
}

int DicomImporter::studiesAddedDuringImport()
{
  return StudiesAddedDuringImport;
}

int DicomImporter::seriesAddedDuringImport()
{
  return SeriesAddedDuringImport;
}

int DicomImporter::instancesAddedDuringImport()
{
  return InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DicomImporter::onFileIndexed(const QString& filePath)
{
  // Update the progress dialog when the file name changes
  // - also allows for cancel button
  QCoreApplication::instance()->processEvents();
//  qDebug() << "Indexing \n\n\n\n" << filePath <<"\n\n\n";

}

//----------------------------------------------------------------------------
void DicomImporter::openImportDialog()
{
	QString folder = QFileDialog::getExistingDirectory(NULL, "Import DICOM files from directory ...", "", QFileDialog::ShowDirsOnly);
	if (!folder.isEmpty())
		onImportDirectory(folder);
	else
		reportWarning("No DICOM folder selected");
}

//----------------------------------------------------------------------------
void DicomImporter::onPatientAdded(int databaseID, QString patientID, QString patientName, QString patientBirthDate )
{
  Q_UNUSED(databaseID);
  Q_UNUSED(patientID);
  Q_UNUSED(patientName);
  Q_UNUSED(patientBirthDate);
  ++PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
void DicomImporter::onStudyAdded(QString studyUID)
{
  Q_UNUSED(studyUID);
  ++StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DicomImporter::onSeriesAdded(QString seriesUID)
{
  Q_UNUSED(seriesUID);
  ++SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DicomImporter::onInstanceAdded(QString instanceUID)
{
  Q_UNUSED(instanceUID);
  ++InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DicomImporter::onImportDirectory(QString directory)
{
  if (QDir(directory).exists())
	{
	// reset counts
	PatientsAddedDuringImport = 0;
	StudiesAddedDuringImport = 0;
	SeriesAddedDuringImport = 0;
	InstancesAddedDuringImport = 0;

	// show progress dialog and perform indexing
	showIndexerDialog();
	DICOMIndexer->addDirectory(*DICOMDatabase,directory);

	// display summary result
	if (DisplayImportSummary)
	  {
	  QString message = "Directory import completed.\n\n";
	  message += QString("%1 New Patients\n").arg(QString::number(PatientsAddedDuringImport));
	  message += QString("%1 New Studies\n").arg(QString::number(StudiesAddedDuringImport));
	  message += QString("%1 New Series\n").arg(QString::number(SeriesAddedDuringImport));
	  message += QString("%1 New Instances\n").arg(QString::number(InstancesAddedDuringImport));
	  QMessageBox::information(NULL,"DICOM Directory Import", message);
	  }
  }
}

} // namespace cx


