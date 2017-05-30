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
#include <QSplitter>

// ctkWidgets includes
#include "ctkDirectoryButton.h"
#include "ctkFileDialog.h"

// ctkDICOMCore includes
#include "ctkDICOMDatabase.h"
#include "ctkDICOMFilterProxyModel.h"
#include "ctkDICOMIndexer.h"
#include "cxDICOMModel.h"

// ctkDICOMWidgets includes
#include "cxDICOMAppWidget.h"
#include "cxDICOMThumbnailGenerator.h"
#include "ctkThumbnailLabel.h"
#include "ctkDICOMQueryResultsTabWidget.h"
#include "ctkDICOMQueryRetrieveWidget.h"
#include "ctkDICOMQueryWidget.h"
#include <QToolBar>
//#include "ctkDICOMThumbnailListWidget.h"
#include "cxDICOMThumbnailListWidget.h"


#include "cxDicomImporter.h"
#include "cxLogger.h"

//#include "ui_DICOMAppWidget.h"

//logger
#include <ctkLogger.h>
static ctkLogger logger("org.commontk.DICOM.Widgets.DICOMAppWidget");

Q_DECLARE_METATYPE(QPersistentModelIndex);


namespace cx
{
typedef DICOMModel ctkDICOMModel;

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
//  ctkDICOMThumbnailListWidget* ThumbnailsWidget;
  DICOMThumbnailListWidget* ThumbnailsWidget;
  QSlider* ThumbnailWidthSlider;
  QAction* ActionImport;
  QAction* ActionQuery;
  QAction* ActionRemove;
  QList<QAction*> mAdvancedActions;
  DicomImporter Importer;

  ctkDICOMQueryRetrieveWidget* QueryRetrieveWidget;
  ctkDICOMQueryRetrieveWidget* getQueryRetrieveWidget();

  QSharedPointer<ctkDICOMDatabase> DICOMDatabase;
  QSharedPointer<ctkDICOMThumbnailGenerator> ThumbnailGenerator;
  ctkDICOMModel mDICOMModel;
  ctkDICOMFilterProxyModel DICOMProxyModel;
  QProgressDialog *UpdateSchemaProgress;

  void showUpdateSchemaDialog();
  std::map<ctkDICOMModel::IndexType, QStringList> getSelection() const;

  void removeSelection();
};

//----------------------------------------------------------------------------
// DICOMAppWidgetPrivate methods

DICOMAppWidgetPrivate::DICOMAppWidgetPrivate(DICOMAppWidget* parent):
    q_ptr(parent),
    TopLayout(NULL),
    TreeView(NULL),
    ToolBar(NULL),
    ThumbnailsWidget(NULL),
    ThumbnailWidthSlider(NULL),
    ActionImport(NULL),
    ActionQuery(NULL),
    ActionRemove(NULL),
    QueryRetrieveWidget(NULL)
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

  if (QueryRetrieveWidget)
	  QueryRetrieveWidget->deleteLater();
}

void DICOMAppWidgetPrivate::setupUi(DICOMAppWidget* parent)
{
	Q_Q(DICOMAppWidget);

	TopLayout = new QVBoxLayout(parent);
	TopLayout->setMargin(0);

	ToolBar = new QToolBar;
	TopLayout->addWidget(ToolBar);

	ActionImport = new QAction("Open file(s)", this);
	QString helpText = "Open a DICOM file or folder and make it ready for import";
	ActionImport->setToolTip(helpText);
	ActionImport->setStatusTip(helpText);
	ActionImport->setIcon(QIcon(":/icons/open_icon_library/document-open-7.png"));
	q->connect(ActionImport, SIGNAL(triggered()), &Importer, SLOT(openImportDialog()));
	ToolBar->addAction(ActionImport);

	ActionRemove = new QAction("Remove", this);
	helpText = "Remove the selected series";
	ActionRemove->setToolTip(helpText);
	ActionRemove->setStatusTip(helpText);
	ActionRemove->setIcon(QIcon(":/icons/open_icon_library/dialog-close.png"));
	q->connect(ActionRemove, SIGNAL(triggered()), q, SLOT(onRemoveAction()));
	mAdvancedActions.append(ActionRemove);
	ToolBar->addAction(ActionRemove);

	ActionQuery = new QAction("Query", this);
	helpText = "Query and Retrieve DICOM studies from a DICOM database";
	ActionQuery->setToolTip(helpText);
	ActionQuery->setStatusTip(helpText);
	ActionQuery->setIcon(QIcon(":/icons/message_levels/dialog-information-4.png"));
	q->connect(ActionQuery, SIGNAL(triggered()), q, SLOT(openQueryDialog()));
	mAdvancedActions.append(ActionQuery);
	ToolBar->addAction(ActionQuery);

	QSplitter* splitter = new QSplitter;
	splitter->setOrientation(Qt::Vertical);
	TopLayout->addWidget(splitter);

	TreeView = new QTreeView;
	TreeView->setAlternatingRowColors(true);
	splitter->addWidget(TreeView);

	QWidget* ThumbnailsFullWidget = new QWidget;
	splitter->addWidget(ThumbnailsFullWidget);
	QVBoxLayout* ThumbnailsFullWidgetLayout = new QVBoxLayout(ThumbnailsFullWidget);
	ThumbnailsFullWidgetLayout->setMargin(0);

	ThumbnailsWidget = new DICOMThumbnailListWidget;
	ThumbnailsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	ThumbnailsWidget->setMinimumSize(QSize(0,200));
	ThumbnailsFullWidgetLayout->addWidget(ThumbnailsWidget);

	ThumbnailWidthSlider = new QSlider;
	ThumbnailWidthSlider->setMinimum(64);
	ThumbnailWidthSlider->setMaximum(256);
	ThumbnailWidthSlider->setValue(64);
	ThumbnailWidthSlider->setOrientation(Qt::Horizontal);
	connect(ThumbnailWidthSlider, SIGNAL(valueChanged(int)), q, SLOT(onThumbnailWidthSliderValueChanged(int)));
	ThumbnailsFullWidgetLayout->addWidget(ThumbnailWidthSlider);
}

void DICOMAppWidgetPrivate::showUpdateSchemaDialog()
{
  Q_Q(DICOMAppWidget);

	ctkDICOMDatabase* db = DICOMDatabase.data();
	q->connect(db, SIGNAL(schemaUpdateStarted(int)), q, SLOT(schemaUpdateStarted(int)));
	q->connect(db, SIGNAL(schemaUpdateProgress(int)), q, SLOT(schemaUpdateProgress(int)));
	q->connect(db, SIGNAL(schemaUpdateProgress(QString)), q, SLOT(schemaUpdateProgress(QString)));
	q->connect(db, SIGNAL(schemaUpdated()), q, SLOT(schemaUpdated()));

//  if (UpdateSchemaProgress == 0)
//    {
//    //
//    // Set up the Update Schema Progress Dialog
//    //
//    UpdateSchemaProgress = new QProgressDialog(
//        q->tr("DICOM Schema Update"), "Cancel", 0, 100, q,
//         Qt::WindowTitleHint | Qt::WindowSystemMenuHint);

//	// We don't want the progress dialog to resize itself, so we bypass the label
//	// by creating our own
//	QLabel* progressLabel = new QLabel(q->tr("Initialization..."));
//	UpdateSchemaProgress->setLabel(progressLabel);
//	UpdateSchemaProgress->setWindowModality(Qt::ApplicationModal);
//	UpdateSchemaProgress->setMinimumDuration(0);
//	UpdateSchemaProgress->setValue(0);

//	//q->connect(UpdateSchemaProgress, SIGNAL(canceled()),
//	 //       DICOMIndexer.data(), SLOT(cancel()));
//	ctkDICOMDatabase* db = DICOMDatabase.data();

//	q->connect(db, SIGNAL(schemaUpdateStarted(int)), UpdateSchemaProgress, SLOT(setMaximum(int)));
//	q->connect(db, SIGNAL(schemaUpdateProgress(int)), UpdateSchemaProgress, SLOT(setValue(int)));
//	q->connect(db, SIGNAL(schemaUpdateProgress(QString)), progressLabel, SLOT(setText(QString)));
//	// close the dialog
////	q->connect(db, SIGNAL(schemaUpdated()), UpdateSchemaProgress, SLOT(close()));
//	// reset the database to show new data
//	q->connect(db, SIGNAL(schemaUpdated()), &mDICOMModel, SLOT(reset()));
//	// reset the database if canceled
//	q->connect(UpdateSchemaProgress, SIGNAL(canceled()), &mDICOMModel, SLOT(reset()));


//	 q->connect(db, SIGNAL(schemaUpdateStarted(int)), q, SLOT(schemaUpdateStarted(int)));
//	 q->connect(db, SIGNAL(schemaUpdateProgress(int)), q, SLOT(schemaUpdateProgress(int)));
//	 q->connect(db, SIGNAL(schemaUpdateProgress(QString)), q, SLOT(schemaUpdateProgress(QString)));
//	 q->connect(db, SIGNAL(schemaUpdated()), q, SLOT(schemaUpdated()));
//  }
//  UpdateSchemaProgress->show();
}

void DICOMAppWidget::schemaUpdateStarted(int)
{
//	report("DICOM schema update started...");
}
void DICOMAppWidget::schemaUpdateProgress(QString val)
{
}
void DICOMAppWidget::schemaUpdateProgress(int val)
{
}
void DICOMAppWidget::schemaUpdated()
{
//	report("DICOM schema updated");
}

void DICOMAppWidgetPrivate::removeSelection()
{
	QModelIndexList selection = TreeView->selectionModel()->selectedIndexes();
	QModelIndex index;

	foreach(index,selection)
	{
		if (index.column()!=0)
			continue;
		mDICOMModel.removeRows(index.row(), 1, index.parent());
//		this->remove(index);
	}
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
	  ctkDICOMModel::IndexType type = static_cast<ctkDICOMModel::IndexType>(mDICOMModel.data(index0,ctkDICOMModel::TypeRole).toInt());
	  QString uid = mDICOMModel.data(index0,ctkDICOMModel::UIDRole).toString();

	  retval[type] << uid;
	  retval[type].removeDuplicates();
	}

	return retval;
}

//----------------------------------------------------------------------------
ctkDICOMQueryRetrieveWidget* DICOMAppWidgetPrivate::getQueryRetrieveWidget()
{
	if (!QueryRetrieveWidget)
	{
		Q_Q(DICOMAppWidget);
		//Initialize Q/R widget
		QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
		QueryRetrieveWidget->setWindowModality(Qt::ApplicationModal);

		connect(QueryRetrieveWidget, SIGNAL(canceled()), QueryRetrieveWidget, SLOT(hide()) );
		connect(QueryRetrieveWidget, SIGNAL(canceled()), q, SLOT(onQueryRetrieveFinished()) );

		QueryRetrieveWidget->setRetrieveDatabase(DICOMDatabase);
	}

	return QueryRetrieveWidget;
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
		  &d->mDICOMModel, SLOT(reset()));
  connect(&d->Importer, SIGNAL(directoryImported()),
		  this, SIGNAL(directoryImported()));

  //Enable sorting in tree view
  d->TreeView->setSortingEnabled(true);
  d->TreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
  d->TreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  d->DICOMProxyModel.setSourceModel(&d->mDICOMModel);
  d->TreeView->setModel(&d->mDICOMModel);

  d->ThumbnailsWidget->setThumbnailSize(
    QSize(d->ThumbnailWidthSlider->value(), d->ThumbnailWidthSlider->value()));

  // Treeview signals
  connect(d->TreeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(onTreeCollapsed(QModelIndex)));
  connect(d->TreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(onTreeExpanded(QModelIndex)));

  //Set ToolBar button style
  d->ToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

//  //Initialize Q/R widget
//  d->QueryRetrieveWidget = new ctkDICOMQueryRetrieveWidget();
//  d->QueryRetrieveWidget->setWindowModality ( Qt::ApplicationModal );

  connect(d->TreeView->selectionModel(),
		  &QItemSelectionModel::selectionChanged,
		  this,
		  &DICOMAppWidget::onSelectionChanged);
  connect(d->TreeView->selectionModel(),
		  &QItemSelectionModel::currentChanged,
		  this,
		  &DICOMAppWidget::onCurrentChanged);

  //connect signal and slots
  connect(d->TreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(onModelSelected(QModelIndex)));

//  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), d->QueryRetrieveWidget, SLOT(hide()) );
//  connect(d->QueryRetrieveWidget, SIGNAL(canceled()), this, SLOT(onQueryRetrieveFinished()) );
}

//----------------------------------------------------------------------------
DICOMAppWidget::~DICOMAppWidget()
{
	Q_D(DICOMAppWidget);

//  d->QueryRetrieveWidget->deleteLater();
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

	CX_LOG_CHANNEL_INFO("dicom") << "Open Database " << databaseFileName.toStdString() << ", open= " << d->DICOMDatabase->isOpen();
//	std::cout << "open database " << databaseFileName.toStdString() << ", open= " << d->DICOMDatabase->isOpen() << std::endl;
	}
  catch (std::exception& e)
    {
	  CX_LOG_CHANNEL_ERROR("dicom") << "Database error: " << qPrintable(d->DICOMDatabase->lastError());
//    std::cerr << "Database error: " << qPrintable(d->DICOMDatabase->lastError()) << "\n";
    d->DICOMDatabase->closeDatabase();
    return;
    }

  // update the database schema if needed and provide progress
  this->updateDatabaseSchemaIfNeeded();

  d->mDICOMModel.setDatabase(d->DICOMDatabase);
  d->mDICOMModel.setEndLevel(ctkDICOMModel::SeriesType);
  d->TreeView->resizeColumnToContents(0);
  d->Importer.setDatabase(d->DICOMDatabase);

//  d->getQueryRetrieveWidget()->setRetrieveDatabase(d->DICOMDatabase);
  d->ThumbnailsWidget->setDatabaseDirectory(directory);
  d->ThumbnailsWidget->setDatabase(d->DICOMDatabase);
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

  d->getQueryRetrieveWidget()->show();
  d->getQueryRetrieveWidget()->raise();

}

//----------------------------------------------------------------------------
void DICOMAppWidget::onQueryRetrieveFinished()
{
  Q_D(DICOMAppWidget);
  d->mDICOMModel.reset();
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
	d->removeSelection();
}

//----------------------------------------------------------------------------
void DICOMAppWidget::resumeModel()
{
  Q_D(DICOMAppWidget);
  d->mDICOMModel.setDatabase(d->DICOMDatabase);
}

//----------------------------------------------------------------------------
void DICOMAppWidget::resetModel()
{
  Q_D(DICOMAppWidget);
  d->mDICOMModel.reset();
}

////----------------------------------------------------------------------------
void DICOMAppWidget::onImportDirectory(QString directory)
{
  Q_D(DICOMAppWidget);
	d->Importer.onImportDirectory(directory);
}

void DICOMAppWidget::onSelectionChanged(const QItemSelection&, const QItemSelection&)
{
}

void DICOMAppWidget::onCurrentChanged(const QModelIndex& next, const QModelIndex& last)
{
	Q_D(DICOMAppWidget);
	d->ThumbnailsWidget->addThumbnails(next);
}

void DICOMAppWidget::onModelSelected(const QModelIndex &index)
{
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
	d->TreeView->resizeColumnToContents(1);
}

//----------------------------------------------------------------------------
void DICOMAppWidget::onThumbnailWidthSliderValueChanged(int val)
{
  Q_D(DICOMAppWidget);
  d->ThumbnailsWidget->setThumbnailSize(QSize(val, val));
}

void DICOMAppWidget::addActionToToolbar(QAction* action)
{
	Q_D(DICOMAppWidget);
	d->ToolBar->addAction(action);
}

QList<QAction*> DICOMAppWidget::getAdvancedActions() const
{
	return d_ptr->mAdvancedActions;
}



} // namespace cx
