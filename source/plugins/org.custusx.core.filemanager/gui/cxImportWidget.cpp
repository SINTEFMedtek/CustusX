#include "cxImportWidget.h"

#include <QTreeWidgetItem>

#include <QFileDialog>
#include <QPushButton>
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

namespace cx
{
/*
 * TODOS:
 * * create point metric group widget, where you select parent for one data and the rest is also set
 *
 * * remove dialog for pointmetric parent selection -> set dummyParent1 and dummyParent2
 * * add header in table to show size of point metric group
 * * implement coordinate system guess
 * * implement parent guess
 * * implement auto RAS to LPS conversion
 * * add not imported data into list of parents
 */

ImportWidget::ImportWidget(cx::FileManagerServicePtr filemanager, cx::VisServicesPtr services) :
	BaseWidget(NULL, "ImportWidget", "Import"),
	mFileManager(filemanager),
	mVisServices(services),
	mSelectedIndexInTable(0){

	//see https://wiki.qt.io/How_to_Use_QTableWidget
	mTableWidget = new QTableWidget();
	mTableWidget->setRowCount(0);
	mTableWidget->setColumnCount(3);
	mTableHeader<<"Status"<<"Filename"<<"Remove?";
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	//mTableWidget->horizontalHeader()->setStretchLastSection(true);
	mTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	mTableWidget->verticalHeader()->setVisible(false);
	mTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	mTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	mTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	mTableWidget->setShowGrid(false);
	mTableWidget->setStyleSheet("QTableView {selection-background-color: red;}");
	mTableWidget->setGeometry(QApplication::desktop()->screenGeometry());
	connect(mTableWidget, &QTableWidget::cellClicked, this, &ImportWidget::tableItemSelected);

	mStackedWidget = new QStackedWidget;

	mTopLayout = new QVBoxLayout();
	this->setLayout(mTopLayout);
	QPushButton *addMoreFilesButton = new QPushButton("Add more files...");
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	QPushButton *importButton = new QPushButton("Import");
	QPushButton *cancelButton = new QPushButton("Cancel");
	buttonLayout->addWidget(importButton);
	buttonLayout->addWidget(cancelButton);
	mTopLayout->addWidget(addMoreFilesButton);
	mTopLayout->addWidget(mTableWidget);
	mTopLayout->addWidget(mStackedWidget);
	mTopLayout->addStretch();
	mTopLayout->addLayout(buttonLayout);

	connect(addMoreFilesButton, &QPushButton::clicked, this, &ImportWidget::addMoreFilesButtonClicked);
	connect(importButton, &QPushButton::clicked, this, &ImportWidget::importButtonClicked);
	connect(cancelButton, &QPushButton::clicked, this, &ImportWidget::cancelButtonClicked);
	connect(this, &ImportWidget::finishedImporting, this, &ImportWidget::cleanUpAfterImport);
}

void ImportWidget::insertDataIntoTable(QString filename, std::vector<DataPtr> data)
{
	int newRowIndex = mTableWidget->rowCount();
	mTableWidget->setRowCount(newRowIndex+1);
	mTableWidget->selectRow(mSelectedIndexInTable);
	QString status;
	bool allDataOk = true;
	for(int i=0; i<data.size(); ++i)
	{
		if(!data[i])
			allDataOk = false;
	}
	status = allDataOk ? "ok" : "error";
	QFileInfo fileInfo(filename);
	filename = fileInfo.fileName();
	mTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(status));
	mTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(filename));
	mTableWidget->setItem(newRowIndex, 2, new QTableWidgetItem("TODO"));

	return;
}

void ImportWidget::addMoreFilesButtonClicked()
{
	QStringList filenames = this->openFileBrowserForSelectingFiles();

	ImportDataTypeWidget *widget = NULL;
	foreach (QString filename, filenames)
	{
		std::vector<DataPtr> newData = mFileManager->read(filename);
		mData.insert(mData.end(), newData.begin(), newData.end());

		this->insertDataIntoTable(filename, newData);
		mParentCandidates = this->generateParentCandidates(mData);
		widget = new ImportDataTypeWidget(NULL, mVisServices, newData, mParentCandidates);
		mStackedWidget->addWidget(widget);


		/*
		std::set<QString, std::vector<DataPtr>> pointMetricSets;
		for(int i=0; i<newData.size(); ++i)
		{
			DataPtr data = newData[i];
			QString type = data->getType();


			if(type == "pointMetric")
			{
				std::vector<DataPtr> parentData = pointMetricSets[data->getParentSpace()];

				//widget = new ImportPointMetricsWidget(NULL, mVisServices, newData);

			}
			else
			{
				insertDataIntoTable(data);
				widget = new ImportDataTypeWidget(NULL, mVisServices, data);
				mStackedWidget->addWidget(widget);
			}

		}
		*/
	}
}

void ImportWidget::importButtonClicked()
{
	CX_LOG_DEBUG() << "IMPORTING: ";

	for(int i=0; i<mData.size(); ++i)
	{
		if(mData[i])
		{
			CX_LOG_DEBUG() << mData[i]->getFilename();
			mVisServices->patient()->insertData(mData[i]);
		}
	}
	emit finishedImporting();
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

void ImportWidget::tableItemSelected(int row, int column)
{
	if(row == mSelectedIndexInTable)
		return;
	CX_LOG_DEBUG() << "New row selected: " << row;
	mStackedWidget->setCurrentIndex(row);
	mSelectedIndexInTable = row;
}

void ImportWidget::cleanUpAfterImport()
{
	//clear data
	mData.clear();
	//reset table
	mTableWidget->clear();
	mTableWidget->setRowCount(0);
	mTableWidget->setHorizontalHeaderLabels(mTableHeader);
	//clear the stacked widget
	for(int i=mStackedWidget->count()-1; i>=0; --i)
	{
		QWidget *widgetToRemove = mStackedWidget->widget(i);
		mStackedWidget->removeWidget(widgetToRemove);
	}
	CX_LOG_DEBUG() << "StackedWidget count after remove: " << mStackedWidget->count();
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

QString ImportWidget::generateFileTypeFilter()
{
	QString file_type_filter;
	std::vector<FileReaderWriterServicePtr> mesh_readers = mVisServices->file()->getImportersForDataType("mesh");
	std::vector<FileReaderWriterServicePtr> image_readers = mVisServices->file()->getImportersForDataType("image");
	std::vector<FileReaderWriterServicePtr> point_metric_readers = mVisServices->file()->getImportersForDataType("pointMetric");
	std::vector<FileReaderWriterServicePtr> readers;
	readers.insert( readers.end(), mesh_readers.begin(), mesh_readers.end() );
	readers.insert( readers.end(), image_readers.begin(), image_readers.end() );
	readers.insert( readers.end(), point_metric_readers.begin(), point_metric_readers.end() );

	file_type_filter = "Image/Mesh/PointMetrics (";
	for(int i=0; i<readers.size(); ++i)
	{
		QString suffix = readers[i]->getFileSuffix();
		if(!suffix.isEmpty())
		file_type_filter.append("*."+suffix+" ");
	}
	while(file_type_filter.endsWith( ' ' ))
		file_type_filter.chop(1);
	file_type_filter.append(")");
	CX_LOG_DEBUG() << "IMPORT FILTER: " << file_type_filter;

	return file_type_filter;
}

std::vector<DataPtr> ImportWidget::generateParentCandidates(std::vector<DataPtr> notLoadedData) const
{
	std::vector<DataPtr> parentCandidates;
	for(int i=0; i<notLoadedData.size(); ++i)
	{
		if(notLoadedData[i]->getType() != "pointMetric")
			parentCandidates.push_back(notLoadedData[i]);
	}
	std::map<QString, DataPtr> loadedData = mVisServices->patient()->getDatas();
	std::map<QString, DataPtr>::iterator it = loadedData.begin();
	for(it; it!=loadedData.end(); ++it)
	{
		parentCandidates.push_back(it->second);
	}
	return parentCandidates;
}


}