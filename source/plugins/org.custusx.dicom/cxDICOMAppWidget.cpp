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

// std includes
#include <iostream>

#include <dcmimage.h>

// Qt includes
#include <QAction>
#include <QCoreApplication>
#include <QCheckBox>
#include <QDebug>
#include <QMessageBox>
#include <QMetaType>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QProgressDialog>
#include <QSettings>
#include <QSlider>
#include <QTabBar>
#include <QTimer>
#include <QTreeView>
#include <QLabel>
#include <QHBoxLayout>

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMFilterProxyModel.h"
#include "ctkDICOMIndexer.h"
#include "ctkDICOMModel.h"

// ctkDICOMWidgets includes
#include "cxDICOMAppWidget.h"
#include "ctkDICOMThumbnailGenerator.h"
#include "ctkThumbnailLabel.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryWidget.h"
#include <QToolBar>
#include "ctkDICOMThumbnailListWidget.h"

//#include "ui_DICOMAppWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMAppWidget");

Q_DECLARE_METATYPE(QPersistentModelIndex);


namespace cx
{


//----------------------------------------------------------------------------
class DICOMAppWidgetPrivate: public QWidget
{
public:
  DICOMAppWidget* const q_ptr;
  Q_DECLARE_PUBLIC(DICOMAppWidget);

  DICOMAppWidgetPrivate(DICOMAppWidget* );
  ~DICOMAppWidgetPrivate();
  void setupUi(DICOMAppWidget* parent);

  QVBoxLayout* TopLayout;
  QTreeView* TreeView;
  QToolBar* ToolBar;
  ctkDICOMThumbnailListWidget* ThumbnailsWidget;
  QSlider* ThumbnailWidthSlider;
  //"ctkDICOMItemView" name="ImagePreview"
  QAction* ActionImport;
  QAction* ActionQuery;
  QAction* ActionRemove;

  ctkFileDialog* ImportDialog;
  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> ThumbnailGenerator;
  ctkDICOMModel DICOMModel;
  ctkDICOMFilterProxyModel DICOMProxyModel;
  QSharedPointer<ctkDICOMIndexer> DICOMIndexer;
  QProgressDialog *IndexerProgress;
  QProgressDialog *UpdateSchemaProgress;

  void showIndexerDialog();
  void showUpdateSchemaDialog();
  std::map<ctkDICOMModel::IndexType, QStringList> getSelection() const;

  // used when suspending the ctkDICOMModel
  QSqlDatabase EmptyDatabase;

//  QTimer* AutoPlayTimer;

  bool IsSearchWidgetPopUpMode;

  // local count variables to keep track of the number of items
  // added to the database during an import operation
  bool DisplayImportSummary;
  int PatientsAddedDuringImport;
  int StudiesAddedDuringImport;
  int SeriesAddedDuringImport;
  int InstancesAddedDuringImport;
};

//----------------------------------------------------------------------------
// DICOMAppWidgetPrivate methods

DICOMAppWidgetPrivate::DICOMAppWidgetPrivate(DICOMAppWidget* parent): q_ptr(parent)
{
  DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  ThumbnailGenerator = QSharedPointer <ctkDICOMThumbnailGenerator> (new ctkDICOMThumbnailGenerator);
  DICOMDatabase->setThumbnailGenerator(ThumbnailGenerator.data());
  DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
  IndexerProgress = 0;
  UpdateSchemaProgress = 0;
  DisplayImportSummary = true;
  PatientsAddedDuringImport = 0;
  StudiesAddedDuringImport = 0;
  SeriesAddedDuringImport = 0;
  InstancesAddedDuringImport = 0;
}

DICOMAppWidgetPrivate::~DICOMAppWidgetPrivate()
{
  if ( IndexerProgress )
    {
    delete IndexerProgress;
    }
  if ( UpdateSchemaProgress )
    {
    delete UpdateSchemaProgress;
    }
}


void DICOMAppWidgetPrivate::setupUi(DICOMAppWidget* parent)
{
	Q_Q(DICOMAppWidget);

	QHBoxLayout* layout = new QHBoxLayout(parent);
	layout->setMargin(0);
	layout->addWidget(this);

	TopLayout = new QVBoxLayout(this);

	ToolBar = new QToolBar;
	TopLayout->addWidget(ToolBar);

	ActionImport = new QAction("Import", this);
	ActionImport->setToolTip("Import a DICOM file or folder");
	q->connect(ActionImport, SIGNAL(triggered()), q, SLOT(openImportDialog()));
	ToolBar->addAction(ActionImport);

	ActionQuery = new QAction("Query", this);
	ActionQuery->setToolTip("Query and Retrieve DICOM studies from a DICOM node");
	q->connect(ActionQuery, SIGNAL(triggered()), q, SLOT(openQueryDialog()));
	ToolBar->addAction(ActionQuery);

	ActionRemove = new QAction("Remove", this);
	ActionRemove->setToolTip("Remove from database");
	q->connect(ActionRemove, SIGNAL(triggered()), q, SLOT(onRemoveAction()));
	ToolBar->addAction(ActionRemove);

	TreeView = new QTreeView;
	TreeView->setAlternatingRowColors(true);
	TopLayout->addWidget(TreeView);

	ThumbnailsWidget = new ctkDICOMThumbnailListWidget;
	ThumbnailsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ThumbnailsWidget->setMinimumSize(QSize(0,200));
	TopLayout->addWidget(ThumbnailsWidget);

	ThumbnailWidthSlider = new QSlider;
	ThumbnailWidthSlider->setMinimum(64);
	ThumbnailWidthSlider->setMaximum(256);
	ThumbnailWidthSlider->setValue(64);
	ThumbnailWidthSlider->setOrientation(Qt::Horizontal);
	connect(ThumbnailWidthSlider, SIGNAL(valueChanged(int)), q, SLOT(onThumbnailWidthSliderValueChanged(int)));

		TopLayout->addWidget(ThumbnailWidthSlider);
}

void DICOMAppWidgetPrivate::showUpdateSchemaDialog()
{
  Q_Q(DICOMAppWidget);
  if (UpdateSchemaProgress == 0)
    {
    //
    // Set up the Update Schema Progress Dialog
    //
    UpdateSchemaProgress = new QProgressDialog(
        q->tr("DICOM Schema Update"), "Cancel", 0, 100, q,
         Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label
    // by creating our own
    QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
    UpdateSchemaProgress->setLabel(progressLabel);
    UpdateSchemaProgress->setWindowModality(Qt::ApplicationModal);
    UpdateSchemaProgress->setMinimumDuration(0);
    UpdateSchemaProgress->setValue(0);

    //q->connect(UpdateSchemaProgress, SIGNAL(canceled()), 
     //       DICOMIndexer.data(), SLOT(cancel()));

    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateStarted(int)),
            UpdateSchemaProgress, SLOT(setMaximum(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(int)),
            UpdateSchemaProgress, SLOT(setValue(int)));
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdateProgress(QString)),
            progressLabel, SLOT(setText(QString)));

    // close the dialog
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()),
            UpdateSchemaProgress, SLOT(close()));
    // reset the database to show new data
    q->connect(DICOMDatabase.data(), SIGNAL(schemaUpdated()),
            &DICOMModel, SLOT(reset()));
    // reset the database if canceled
    q->connect(UpdateSchemaProgress, SIGNAL(canceled()), 
            &DICOMModel, SLOT(reset()));
    }
  UpdateSchemaProgress->show();
}

void DICOMAppWidgetPrivate::showIndexerDialog()
{
  Q_Q(DICOMAppWidget);
  if (IndexerProgress == 0)
    {
    //
    // Set up the Indexer Progress Dialog
    //
    IndexerProgress = new QProgressDialog( q->tr("DICOM Import"), "Cancel", 0, 100, q,
         Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

    // We don't want the progress dialog to resize itself, so we bypass the label
    // by creating our own
    QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
    IndexerProgress->setLabel(progressLabel);
    IndexerProgress->setWindowModality(Qt::ApplicationModal);
    IndexerProgress->setMinimumDuration(0);
    IndexerProgress->setValue(0);

    q->connect(IndexerProgress, SIGNAL(canceled()), 
                 DICOMIndexer.data(), SLOT(cancel()));

    q->connect(DICOMIndexer.data(), SIGNAL(progress(int)),
            IndexerProgress, SLOT(setValue(int)));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
            progressLabel, SLOT(setText(QString)));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingFilePath(QString)),
            q, SLOT(onFileIndexed(QString)));

    // close the dialog
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
            IndexerProgress, SLOT(close()));
    // reset the database to show new data
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
            &DICOMModel, SLOT(reset()));
    // stop indexing and reset the database if canceled
    q->connect(IndexerProgress, SIGNAL(canceled()), 
            DICOMIndexer.data(), SLOT(cancel()));
    q->connect(IndexerProgress, SIGNAL(canceled()), 
            &DICOMModel, SLOT(reset()));

    // allow users of this widget to know that the process has finished
    q->connect(IndexerProgress, SIGNAL(canceled()), 
            q, SIGNAL(directoryImported()));
    q->connect(DICOMIndexer.data(), SIGNAL(indexingComplete()),
            q, SIGNAL(directoryImported()));
    }
  IndexerProgress->show();
}

std::map<ctkDICOMModel::IndexType, QStringList> DICOMAppWidgetPrivate::getSelection() const
{
//	Q_Q(DICOMAppWidget);
	std::map<ctkDICOMModel::IndexType, QStringList> retval;
	QModelIndexList selection = TreeView->selectionModel()->selectedIndexes();
	QModelIndex index;

	foreach(index,selection)
	{
	  QModelIndex index0 = index.sibling(index.row(), 0);
	  ctkDICOMModel::IndexType type = static_cast<ctkDICOMModel::IndexType>(DICOMModel.data(index0,ctkDICOMModel::TypeRole).toInt());
	  QString uid = DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();

	  if (!retval.count(type))
		  retval[type] << uid;
	}

	return retval;
}


//----------------------------------------------------------------------------
// DICOMAppWidget methods

//----------------------------------------------------------------------------
DICOMAppWidget::DICOMAppWidget(QWidget* _parent):Superclass(_parent),
    d_ptr(new DICOMAppWidgetPrivate(this))
{
  Q_D(DICOMAppWidget);

  d->setupUi(this);

  //Enable sorting in tree view
  d->TreeView->setSortingEnabled(true);
  d->TreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->DICOMProxyModel.setSourceModel(&d->DICOMModel);
  d->TreeView->setModel(&d->DICOMModel);

  d->ThumbnailsWidget->setThumbnailSize(
    QSize(d->ThumbnailWidthSlider->value(), d->ThumbnailWidthSlider->value()));

  // signals related to tracking inserts
  connect(d->DICOMDatabase.data(), SIGNAL(patientAdded(int,QString,QString,QString)), this,
                              SLOT(onPatientAdded(int,QString,QString,QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(studyAdded(QString)), this, SLOT(onStudyAdded(QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(seriesAdded(QString)), this, SLOT(onSeriesAdded(QString)));
  connect(d->DICOMDatabase.data(), SIGNAL(instanceAdded(QString)), this, SLOT(onInstanceAdded(QString)));

  // Treeview signals
  connect(d->TreeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onTreeCollapsed(QModelIndex)));
  connect(d->TreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onTreeExpanded(QModelIndex)));

  //Set ToolBar button style
  d->ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  //Initialize Q/R widget
  d->QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
  d->QueryRetrieveWidget->setWindowModality ( Qt::ApplicationModal );

  //TODO CA move outside class
//  //initialize directory from settings, then listen for changes
//  QSettings settings;
//  if ( settings.value("DatabaseDirectory", "") == "" )
//    {
//    QString directory = QString("./ctkDICOM-Database");
//    settings.setValue("DatabaseDirectory", directory);
//    settings.sync();
//    }
//  QString databaseDirectory = settings.value("DatabaseDirectory").toString();
//  this->setDatabaseDirectory(databaseDirectory);
//  d->DirectoryButton->setDirectory(databaseDirectory);

//  connect(d->DirectoryButton, SIGNAL(directoryChanged(QString)), this, SLOT(setDatabaseDirectory(QString)));

  //Initialize import widget
  d->ImportDialog = new ctkFileDialog();
  QCheckBox* importCheckbox = new QCheckBox("Copy on import", d->ImportDialog);
  d->ImportDialog->setBottomWidget(importCheckbox);
  d->ImportDialog->setFileMode(QFileDialog::Directory);
  d->ImportDialog->setLabelText(QFileDialog::Accept,"Import");
  d->ImportDialog->setWindowTitle("Import DICOM files from directory ...");
  d->ImportDialog->setWindowModality(Qt::ApplicationModal);

  connect(d->TreeView->selectionModel(),
		  SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		  this,
		  SLOT(onSelectionChanged(const QItemSelection&, const QItemSelection&)));
  connect(d->TreeView->selectionModel(),
		  SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
		  this,
		  SLOT(onCurrentChanged(const QModelIndex&, const QModelIndex&)));

  //connect signal and slots
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), d->ThumbnailsWidget, SLOT(addThumbnails(QModelIndex)));
//  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), d->ImagePreview, SLOT(onModelSelected(QModelIndex)));
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onModelSelected(QModelIndex)));

//  connect(d->ThumbnailsWidget, SIGNAL(selected(ctkThumbnailLabel)), this, SLOT(onThumbnailSelected(ctkThumbnailLabel)));
//  connect(d->ThumbnailsWidget, SIGNAL(doubleClicked(ctkThumbnailLabel)), this, SLOT(onThumbnailDoubleClicked(ctkThumbnailLabel)));
  connect(d->ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(onImportDirectory(QString)));

  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), d->QueryRetrieveWidget, SLOT(hide()) );
  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), this, SLOT(onQueryRetrieveFinished()) );

//  connect(d->ImagePreview, SIGNAL(requestNextImage()), this, SLOT(onNextImage()));
//  connect(d->ImagePreview, SIGNAL(requestPreviousImage()), this, SLOT(onPreviousImage()));
//  connect(d->ImagePreview, SIGNAL(imageDisplayed(int,int)), this, SLOT(onImagePreviewDisplayed(int,int)));

//  connect(d->SearchOption, SIGNAL(parameterChanged()), this, SLOT(onSearchParameterChanged()));

//  connect(d->PlaySlider, SIGNAL(valueChanged(int)), d->ImagePreview, SLOT(displayImage(int)));
}

//----------------------------------------------------------------------------
DICOMAppWidget::~DICOMAppWidget()
{
  Q_D(DICOMAppWidget);

  d->QueryRetrieveWidget->deleteLater();
  d->ImportDialog->deleteLater();
}

//----------------------------------------------------------------------------
bool DICOMAppWidget::displayImportSummary()
{
  Q_D(DICOMAppWidget);

  return d->DisplayImportSummary;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::setDisplayImportSummary(bool onOff)
{
  Q_D(DICOMAppWidget);

  d->DisplayImportSummary = onOff;
}

//----------------------------------------------------------------------------
int DICOMAppWidget::patientsAddedDuringImport()
{
  Q_D(DICOMAppWidget);

  return d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
int DICOMAppWidget::studiesAddedDuringImport()
{
  Q_D(DICOMAppWidget);

  return d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
int DICOMAppWidget::seriesAddedDuringImport()
{
  Q_D(DICOMAppWidget);

  return d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
int DICOMAppWidget::instancesAddedDuringImport()
{
  Q_D(DICOMAppWidget);

  return d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::updateDatabaseSchemaIfNeeded()
{

  Q_D(DICOMAppWidget);

  d->showUpdateSchemaDialog();
  d->DICOMDatabase->updateSchemaIfNeeded();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::setDatabaseDirectory(const QString& directory)
{
  Q_D(DICOMAppWidget);

  QSettings settings;
  settings.setValue("DatabaseDirectory", directory);
  settings.sync();

  //close the active DICOM database
  d->DICOMDatabase->closeDatabase();

  //open DICOM database on the directory
  QString databaseFileName = directory + QString("/ctkDICOM.sql");
  try
    {
    d->DICOMDatabase->openDatabase( databaseFileName );
    }
  catch (std::exception e)
    {
    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
    }

  // update the database schema if needed and provide progress
  this->updateDatabaseSchemaIfNeeded();

  d->DICOMModel.setDatabase(d->DICOMDatabase->database());
  d->DICOMModel.setEndLevel(ctkDICOMModel::SeriesType);
  d->TreeView->resizeColumnToContents(0);

  //pass DICOM database instance to Import widget
  // d->ImportDialog->setDICOMDatabase(d->DICOMDatabase);
  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);

  // update the button and let any connected slots know about the change
//  d->DirectoryButton->setDirectory(directory);
  d->ThumbnailsWidget->setDatabaseDirectory(directory);
//  d->ImagePreview->setDatabaseDirectory(directory);
  emit databaseDirectoryChanged(directory);
}

//----------------------------------------------------------------------------
QString DICOMAppWidget::databaseDirectory() const
{
  QSettings settings;
  return settings.value("DatabaseDirectory").toString();
}

//------------------------------------------------------------------------------
void DICOMAppWidget::setTagsToPrecache( const QStringList tags)
{
  Q_D(DICOMAppWidget);
  d->DICOMDatabase->setTagsToPrecache(tags);
}

//------------------------------------------------------------------------------
const QStringList DICOMAppWidget::tagsToPrecache()
{
  Q_D(DICOMAppWidget);
  return d->DICOMDatabase->tagsToPrecache();
}



//----------------------------------------------------------------------------
ctkDICOMDatabase* DICOMAppWidget::database(){
  Q_D(DICOMAppWidget);
  return d->DICOMDatabase.data();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onFileIndexed(const QString& filePath)
{
  // Update the progress dialog when the file name changes
  // - also allows for cancel button
  QCoreApplication::instance()->processEvents();
  qDebug() << "Indexing \n\n\n\n" << filePath <<"\n\n\n";
  
}

//----------------------------------------------------------------------------
void DICOMAppWidget::openImportDialog()
{
  Q_D(DICOMAppWidget);

  d->ImportDialog->show();
  d->ImportDialog->raise();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::openQueryDialog()
{
  Q_D(DICOMAppWidget);

  d->QueryRetrieveWidget->show();
  d->QueryRetrieveWidget->raise();

}

//----------------------------------------------------------------------------
void DICOMAppWidget::onQueryRetrieveFinished()
{
  Q_D(DICOMAppWidget);
  d->DICOMModel.reset();
  emit this->queryRetrieveFinished();
}

QStringList DICOMAppWidget::getSelectedPatients()
{
	Q_D(DICOMAppWidget);
	return d->getSelection()[ctkDICOMModel::PatientType];
}
QStringList DICOMAppWidget::getSelectedStudies()
{
	Q_D(DICOMAppWidget);
	return d->getSelection()[ctkDICOMModel::StudyType];
}
QStringList DICOMAppWidget::getSelectedSeries()
{
	Q_D(DICOMAppWidget);
	return d->getSelection()[ctkDICOMModel::SeriesType];
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onRemoveAction()
{
  Q_D(DICOMAppWidget);

  //d->QueryRetrieveWidget->show();
  // d->QueryRetrieveWidget->raise();
  std::cout << "on remove" << std::endl;
  QModelIndexList selection = d->TreeView->selectionModel()->selectedIndexes();
  std::cout << selection.size() << std::endl;
  QModelIndex index;
  foreach(index,selection)
  {
    QModelIndex index0 = index.sibling(index.row(), 0);
    if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType))
    {
      QString seriesUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removeSeries(seriesUID);
    }
    else if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType))
    {
      QString studyUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removeStudy(studyUID);
    }
    else if ( d->DICOMModel.data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType))
    {
      QString patientUID = d->DICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();
      d->DICOMDatabase->removePatient(patientUID);
    }
  }
  d->DICOMModel.reset();
}


//----------------------------------------------------------------------------
void DICOMAppWidget::suspendModel()
{
  Q_D(DICOMAppWidget);

  d->DICOMModel.setDatabase(d->EmptyDatabase);
}

//----------------------------------------------------------------------------
void DICOMAppWidget::resumeModel()
{
  Q_D(DICOMAppWidget);

  d->DICOMModel.setDatabase(d->DICOMDatabase->database());
}

//----------------------------------------------------------------------------
void DICOMAppWidget::resetModel()
{
  Q_D(DICOMAppWidget);

  d->DICOMModel.reset();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onPatientAdded(int databaseID, QString patientID, QString patientName, QString patientBirthDate )
{
  Q_D(DICOMAppWidget);
  Q_UNUSED(databaseID);
  Q_UNUSED(patientID);
  Q_UNUSED(patientName);
  Q_UNUSED(patientBirthDate);
  ++d->PatientsAddedDuringImport;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onStudyAdded(QString studyUID)
{
  Q_D(DICOMAppWidget);
  Q_UNUSED(studyUID);
  ++d->StudiesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onSeriesAdded(QString seriesUID)
{
  Q_D(DICOMAppWidget);
  Q_UNUSED(seriesUID);
  ++d->SeriesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onInstanceAdded(QString instanceUID)
{
  Q_D(DICOMAppWidget);
  Q_UNUSED(instanceUID);
  ++d->InstancesAddedDuringImport;
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(DICOMAppWidget);
  if (QDir(directory).exists())
    {
    QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(d->ImportDialog->bottomWidget());
    QString targetDirectory;
    if (copyOnImport->checkState() == Qt::Checked)
      {
      targetDirectory = d->DICOMDatabase->databaseDirectory();
      }

    // reset counts
    d->PatientsAddedDuringImport = 0;
    d->StudiesAddedDuringImport = 0;
    d->SeriesAddedDuringImport = 0;
    d->InstancesAddedDuringImport = 0;

    // show progress dialog and perform indexing
    d->showIndexerDialog();
    d->DICOMIndexer->addDirectory(*d->DICOMDatabase,directory,targetDirectory);

    // display summary result
    if (d->DisplayImportSummary)
      {
      QString message = "Directory import completed.\n\n";
      message += QString("%1 New Patients\n").arg(QString::number(d->PatientsAddedDuringImport));
      message += QString("%1 New Studies\n").arg(QString::number(d->StudiesAddedDuringImport));
      message += QString("%1 New Series\n").arg(QString::number(d->SeriesAddedDuringImport));
      message += QString("%1 New Instances\n").arg(QString::number(d->InstancesAddedDuringImport));
      QMessageBox::information(this,"DICOM Directory Import", message);
      }
  }
}

void DICOMAppWidget::onSelectionChanged(const QItemSelection&, const QItemSelection&)
{
	std::cout << "selected patients:\n " << this->getSelectedPatients().join("\n").toStdString() << std::endl;
	std::cout << "selected studies:\n " << this->getSelectedStudies().join("\n").toStdString() << std::endl;
	std::cout << "selected series:\n " << this->getSelectedSeries().join("\n").toStdString() << std::endl;
}

void DICOMAppWidget::onCurrentChanged(const QModelIndex& next, const QModelIndex& last)
{
	Q_D(DICOMAppWidget);
	d->ThumbnailsWidget->selectThumbnailFromIndex(next);
}

void DICOMAppWidget::onModelSelected(const QModelIndex &index){
Q_D(DICOMAppWidget);

    ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    if(model)
      {
        QModelIndex index0 = index.sibling(index.row(), 0);

        d->ActionRemove->setEnabled(
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::SeriesType) ||
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::StudyType) ||
            model->data(index0,ctkDICOMModel::TypeRole) == static_cast<int>(ctkDICOMModel::PatientType) );
        }

      else
        {
        d->ActionRemove->setEnabled(false);
        }
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onTreeCollapsed(const QModelIndex &index){
    Q_UNUSED(index);
    Q_D(DICOMAppWidget);
    d->TreeView->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onTreeExpanded(const QModelIndex &index){
    Q_UNUSED(index);
    Q_D(DICOMAppWidget);
    d->TreeView->resizeColumnToContents(0);
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onThumbnailWidthSliderValueChanged(int val)
{
  Q_D(DICOMAppWidget);
  d->ThumbnailsWidget->setThumbnailSize(QSize(val, val));
}


} // namespace cx
