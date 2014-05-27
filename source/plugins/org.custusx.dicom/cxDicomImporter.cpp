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

#include "cxDicomImporter.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QProgressDialog>
#include <QLabel>
// ctkWidgets includes
#include "ctkFileDialog.h"
// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMIndexer.h"

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

	//Initialize import widget
	ImportDialog = new ctkFileDialog();
	QCheckBox* importCheckbox = new QCheckBox("Copy on import", ImportDialog);
	ImportDialog->setBottomWidget(importCheckbox);
	ImportDialog->setFileMode(QFileDialog::Directory);
	ImportDialog->setLabelText(QFileDialog::Accept,"Import");
	ImportDialog->setWindowTitle("Import DICOM files from directory ...");
	ImportDialog->setWindowModality(Qt::ApplicationModal);

	connect(ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));
}

DicomImporter::~DicomImporter()
{
	delete IndexerProgress;
	ImportDialog->deleteLater();
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
			this, SLOT(indexingCompleted()));

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
  qDebug() << "Indexing \n\n\n\n" << filePath <<"\n\n\n";

}

//----------------------------------------------------------------------------
void DicomImporter::openImportDialog()
{
  ImportDialog->show();
  ImportDialog->raise();
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
	QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(ImportDialog->bottomWidget());
	QString targetDirectory;
	if (copyOnImport->checkState() == Qt::Checked)
	  {
	  targetDirectory = DICOMDatabase->databaseDirectory();
	  }

	// reset counts
	PatientsAddedDuringImport = 0;
	StudiesAddedDuringImport = 0;
	SeriesAddedDuringImport = 0;
	InstancesAddedDuringImport = 0;

	// show progress dialog and perform indexing
	showIndexerDialog();
	DICOMIndexer->addDirectory(*DICOMDatabase,directory,targetDirectory);

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


