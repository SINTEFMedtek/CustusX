/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImportWidget.h"

#include <QTreeWidgetItem>

#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QDesktopWidget>
#include <QStackedWidget>
#include "cxForwardDeclarations.h"
#include "cxFileReaderWriterService.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxProfile.h"
#include "cxImportDataTypeWidget.h"
#include "cxSessionStorageService.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxPointMetric.h"

namespace cx
{
/*
 * TODOS:
 * * implement coordinate system guess
 * * implement parent guess
 * * implement auto RAS to LPS conversion
 */

ImportWidget::ImportWidget(cx::FileManagerServicePtr filemanager, cx::VisServicesPtr services) :
	BaseWidget(NULL, "import_widget", "Import"),
	mSelectedIndexInTable(0),
	mFileManager(filemanager),
	mVisServices(services)
{
	//see https://wiki.qt.io/How_to_Use_QTableWidget
	mTableWidget = new QTableWidget();
	mTableWidget->setRowCount(0);
	mTableWidget->setColumnCount(3);
	mTableHeader<<""<<"Status"<<"Filename";
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	mTableWidget->horizontalHeader()->setStretchLastSection(true);
	mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	mTableWidget->verticalHeader()->setVisible(false);
	mTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	mTableWidget->setShowGrid(false);
	mTableWidget->setStyleSheet("QTableView {selection-background-color: #ACCEF7;}");
	mTableWidget->setGeometry(QApplication::desktop()->screenGeometry());
	connect(mTableWidget, &QTableWidget::currentCellChanged, this, &ImportWidget::tableItemSelected);

	mStackedWidget = new QStackedWidget;

	mTopLayout = new QVBoxLayout();
	this->setLayout(mTopLayout);
	QPushButton *addMoreFilesButton = new QPushButton("Add more files...");
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	QPushButton *importButton = new QPushButton("Import");
	QPushButton *cancelButton = new QPushButton("Cancel");
	buttonLayout->addWidget(importButton);
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addStretch();
	mTopLayout->addWidget(addMoreFilesButton);
	mTopLayout->addWidget(new QLabel("Supports: "+this->generateFileTypeFilter()));
	mTopLayout->addWidget(mTableWidget);
	mTopLayout->addWidget(mStackedWidget);
	mTopLayout->addStretch();
	mTopLayout->addLayout(buttonLayout);

	connect(addMoreFilesButton, &QPushButton::clicked, this, &ImportWidget::addMoreFilesButtonClicked);
	connect(importButton, &QPushButton::clicked, this, &ImportWidget::importButtonClicked);
	connect(cancelButton, &QPushButton::clicked, this, &ImportWidget::cancelButtonClicked);
	connect(this, &ImportWidget::finishedImporting, this, &ImportWidget::cleanUpAfterImport);

	QAction* addMoreFilesButtonClickedAction = new QAction("AddMoreFilesButtonClickedAction", this);
	this->addAction(addMoreFilesButtonClickedAction);
	connect(addMoreFilesButtonClickedAction, &QAction::triggered, this, &ImportWidget::addMoreFilesButtonClicked);

	QAction* importButtonClickedAction = new QAction("ImportButtonClickedAction", this);
	this->addAction(importButtonClickedAction);
	connect(importButtonClickedAction, &QAction::triggered, this, &ImportWidget::importButtonClicked);
}

int ImportWidget::insertDataIntoTable(QString fullfilename, std::vector<DataPtr> data)
{
	int newRowIndex = mTableWidget->rowCount();
	mTableWidget->setRowCount(newRowIndex+1);
	mTableWidget->selectRow(mSelectedIndexInTable);
	QString status;
	bool allDataOk = true;
	for(unsigned i=0; i<data.size(); ++i)
	{
		if(!data[i])
			allDataOk = false;
	}
	status = allDataOk ? "ok" : "error";
	QFileInfo fileInfo(fullfilename);
	QString filename = fileInfo.fileName();

	QIcon trashcan(":/icons/open_icon_library/edit-delete-2.png");
	QPushButton *removeButton = new QPushButton(trashcan,"");
	connect(removeButton, &QPushButton::clicked, this, &ImportWidget::removeRowFromTableAndRemoveFilenameFromImportList);

	mTableWidget->setCellWidget(newRowIndex, 0, removeButton);
	mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(status));
	QTableWidgetItem *filenameItem = new QTableWidgetItem(filename);
	filenameItem->setData(Qt::ToolTipRole, fullfilename);
	mTableWidget->setItem(newRowIndex, 2, filenameItem);

	return newRowIndex;
}

int ImportWidget::findRowIndexContainingButton(QPushButton *button) const
{
	int retval = -1;
	for(int i=0; i<mTableWidget->rowCount(); ++i)
	{
		int buttonColoumn = 0;
		QTableWidgetItem *item = mTableWidget->item(i,buttonColoumn);
		QWidget *cellWidget = mTableWidget->cellWidget(i,buttonColoumn);
		if(button == cellWidget)
			retval = i;
	}
	return retval;
}

void ImportWidget::addMoreFilesButtonClicked()
{
	QStringList filenames = this->openFileBrowserForSelectingFiles();

	bool addedDICOM = false;

	ImportDataTypeWidget *widget = NULL;
	for(int i = 0; i < filenames.size(); ++i)
	{
		QString filename = filenames[i];
		QString fileType = mFileManager->getFileReaderName(filename);
		if(fileType == "DICOMReader")
		{
			//Only allow adding one dicom series to prevent duplicates, and long processing time
			if(addedDICOM)
			{
				//If import of multiple series is something we need:
				//All DICOM files can be added to a QStringList before sending them to the reader.
				//Then the reader can determine which files are from the same/different series
				CX_LOG_WARNING() << "Import of multiple DICOM series at once is not supported. Skipping series based on file: " << filename;
				continue;
			}
			addedDICOM = true;
		}

		mFileNames.push_back(filename);

		std::vector<DataPtr> newData = mFileManager->read(filename);
		if(newData.size() > 0)
		{
			int index = this->insertDataIntoTable(filename, newData);
			widget = new ImportDataTypeWidget(this, mVisServices, newData, mParentCandidates, filename);
			mStackedWidget->insertWidget(index, widget);
			mNotImportedData.insert(mNotImportedData.end(), newData.begin(), newData.end());//Update mNotImportedData with new data
		}
	}

	this->generateParentCandidates();
}

void ImportWidget::importButtonClicked()
{
	emit readyToImport();
	emit finishedImporting();

	mVisServices->session()->save();//AutoSave
}

void ImportWidget::cancelButtonClicked()
{
	emit finishedImporting();
}

void ImportWidget::removeWidget(QWidget *widget)
{
	mTopLayout->removeWidget(widget);
	widget->deleteLater();
}

void ImportWidget::removeRowFromTableAndRemoveFilenameFromImportList()
{
	QPushButton *button = qobject_cast<QPushButton*>(QObject::sender());
	int rowindex = this->findRowIndexContainingButton(button);
	int filenamecoloumn = 2;
	QString fullfilename = mTableWidget->item(rowindex, filenamecoloumn)->data(Qt::ToolTipRole).toString();
	if(rowindex != -1)
		mTableWidget->removeRow(rowindex);
	int numberOfRemovedEntries = mFileNames.removeAll(fullfilename);

	QWidget *widgetToRemove = mStackedWidget->widget(rowindex);
	mStackedWidget->removeWidget(widgetToRemove);
}

void ImportWidget::tableItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if(currentRow == mSelectedIndexInTable)
		return;

	mStackedWidget->setCurrentIndex(currentRow);
	mSelectedIndexInTable = currentRow;
}

void ImportWidget::cleanUpAfterImport()
{
	//clear data
	//mData.clear();
	mFileNames.clear();

	//reset table
	mTableWidget->clear();
	mTableWidget->setRowCount(0);
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	//clear the stacked widget
	for(int i=mStackedWidget->count()-1; i>=0; --i)
	{
		QWidget *widgetToRemove = mStackedWidget->widget(i);
		mStackedWidget->removeWidget(widgetToRemove);
		delete widgetToRemove;
	}
}

QStringList ImportWidget::openFileBrowserForSelectingFiles()
{
	QString file_type_filter = generateFileTypeFilter();

	QStringList fileName = QFileDialog::getOpenFileNames(this->parentWidget(), QString(tr("Select data file(s) for import")), profile()->getSessionRootFolder(), tr(file_type_filter.toStdString().c_str()));
	if (fileName.empty())
	{
		report("Import canceled");
	}

	return fileName;

}

QString ImportWidget::generateFileTypeFilter() const
{
	QString file_type_filter;
	std::vector<FileReaderWriterServicePtr> mesh_readers = mVisServices->file()->getImportersForDataType(Mesh::getTypeName());
	std::vector<FileReaderWriterServicePtr> image_readers = mVisServices->file()->getImportersForDataType(Image::getTypeName());
	std::vector<FileReaderWriterServicePtr> point_metric_readers = mVisServices->file()->getImportersForDataType(PointMetric::getTypeName());
	std::vector<FileReaderWriterServicePtr> readers;
	readers.insert( readers.end(), mesh_readers.begin(), mesh_readers.end() );
	readers.insert( readers.end(), image_readers.begin(), image_readers.end() );
	readers.insert( readers.end(), point_metric_readers.begin(), point_metric_readers.end() );

	file_type_filter = "Image/Mesh/PointMetrics (";
	for(unsigned i=0; i<readers.size(); ++i)
	{
		QString suffix = readers[i]->getFileSuffix();
		if(!suffix.isEmpty())
		file_type_filter.append("*."+suffix+" ");
	}
	while(file_type_filter.endsWith( ' ' ))
		file_type_filter.chop(1);
	file_type_filter.append(")");

	return file_type_filter;
}

void ImportWidget::generateParentCandidates()
{
	for(unsigned i=0; i<mNotImportedData.size(); ++i)
	{
		if(!mNotImportedData[i])
		{
			CX_LOG_WARNING() << "ImportWidget::generateParentCandidates(): No data";
			continue;
		}
		if(mNotImportedData[i]->getType() != PointMetric::getTypeName())
			mParentCandidates.push_back(mNotImportedData[i]);
	}
	std::map<QString, DataPtr> loadedData = mVisServices->patient()->getDatas();
	std::map<QString, DataPtr>::iterator it = loadedData.begin();
	for(; it!=loadedData.end(); ++it)
	{
		mParentCandidates.push_back(it->second);
	}
	emit parentCandidatesUpdated();
}


}
