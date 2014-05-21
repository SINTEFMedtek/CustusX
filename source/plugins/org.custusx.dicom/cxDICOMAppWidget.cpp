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
#include "cxDicomImporter.h"

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
  QAction* ActionImport;
  QAction* ActionQuery;
  QAction* ActionRemove;
  DicomImporter Importer;

  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> ThumbnailGenerator;
  ctkDICOMModel DICOMModel;
  ctkDICOMFilterProxyModel DICOMProxyModel;
  QProgressDialog *UpdateSchemaProgress;

  void showUpdateSchemaDialog();
  std::map<ctkDICOMModel::IndexType, QStringList> getSelection() const;

  // used when suspending the ctkDICOMModel
  QSqlDatabase EmptyDatabase;
};

//----------------------------------------------------------------------------
// DICOMAppWidgetPrivate methods

DICOMAppWidgetPrivate::DICOMAppWidgetPrivate(DICOMAppWidget* parent): q_ptr(parent)
{
  DICOMDatabase = QSharedPointer<ctkDICOMDatabase> (new ctkDICOMDatabase);
  ThumbnailGenerator = QSharedPointer <ctkDICOMThumbnailGenerator> (new ctkDICOMThumbnailGenerator);
  DICOMDatabase->setThumbnailGenerator(ThumbnailGenerator.data());
  UpdateSchemaProgress = 0;
}

DICOMAppWidgetPrivate::~DICOMAppWidgetPrivate()
{
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
	q->connect(ActionImport, SIGNAL(triggered()), &Importer, SLOT(openImportDialog()));
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

  connect(&d->Importer, SIGNAL(indexingCompleted()),
		  &d->DICOMModel, SLOT(reset()));
  connect(&d->Importer, SIGNAL(directoryImported()),
		  this, SIGNAL(directoryImported()));

  //Enable sorting in tree view
  d->TreeView->setSortingEnabled(true);
  d->TreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->DICOMProxyModel.setSourceModel(&d->DICOMModel);
  d->TreeView->setModel(&d->DICOMModel);

  d->ThumbnailsWidget->setThumbnailSize(
    QSize(d->ThumbnailWidthSlider->value(), d->ThumbnailWidthSlider->value()));

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
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onModelSelected(QModelIndex)));

  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), d->QueryRetrieveWidget, SLOT(hide()) );
  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), this, SLOT(onQueryRetrieveFinished()) );
}

//----------------------------------------------------------------------------
DICOMAppWidget::~DICOMAppWidget()
{
  Q_D(DICOMAppWidget);
  d->QueryRetrieveWidget->deleteLater();
}

//----------------------------------------------------------------------------
bool DICOMAppWidget::displayImportSummary()
{
  Q_D(DICOMAppWidget);
  return d->Importer.displayImportSummary();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::setDisplayImportSummary(bool onOff)
{
  Q_D(DICOMAppWidget);
	return d->Importer.setDisplayImportSummary(onOff);
}

//----------------------------------------------------------------------------
int DICOMAppWidget::patientsAddedDuringImport()
{
  Q_D(DICOMAppWidget);
  return d->Importer.patientsAddedDuringImport();
}

//----------------------------------------------------------------------------
int DICOMAppWidget::studiesAddedDuringImport()
{
  Q_D(DICOMAppWidget);
	return d->Importer.studiesAddedDuringImport();
}

//----------------------------------------------------------------------------
int DICOMAppWidget::seriesAddedDuringImport()
{
  Q_D(DICOMAppWidget);
	return d->Importer.seriesAddedDuringImport();
}

//----------------------------------------------------------------------------
int DICOMAppWidget::instancesAddedDuringImport()
{
  Q_D(DICOMAppWidget);
	return d->Importer.instancesAddedDuringImport();
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
  d->Importer.setDatabase(d->DICOMDatabase);

  d->QueryRetrieveWidget->setRetrieveDatabase(d->DICOMDatabase);
  d->ThumbnailsWidget->setDatabaseDirectory(directory);
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

////----------------------------------------------------------------------------
void DICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(DICOMAppWidget);
	d->Importer.onImportDirectory(directory);
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
