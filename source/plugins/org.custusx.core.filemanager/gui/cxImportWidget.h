/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMPORTWIDGET_H
#define CXIMPORTWIDGET_H

#include <QDialog>
#include "cxBaseWidget.h"
#include "cxFileManagerService.h"
#include "org_custusx_core_filemanager_Export.h"

class QTableWidget;
class QStackedWidget;
class QPushButton;
class QProgressDialog;

namespace cx
{
class ImportDataTypeWidget;

class org_custusx_core_filemanager_EXPORT SimpleImportDataDialog : public QDialog
{
	Q_OBJECT
public:
	SimpleImportDataDialog(ImportDataTypeWidget *widget, QWidget* parent=NULL);
	virtual ~SimpleImportDataDialog(){};
private slots:
	void tableItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void cancelClicked();
private:
	ImportDataTypeWidget* mImportDataTypeWidget;
};

class org_custusx_core_filemanager_EXPORT ImportWidget  : public BaseWidget
{
	Q_OBJECT

public:
	ImportWidget(FileManagerServicePtr filemanager, VisServicesPtr services);

signals:
	void readyToImport();
	void finishedImporting();
	void parentCandidatesUpdated();

private slots:
	void addFilesForImportWithDialogTriggerend();
	void importButtonClicked();
	void cancelButtonClicked();
	ImportDataTypeWidget *addMoreFilesButtonClicked();
	void removeWidget(QWidget *widget);
	void removeRowFromTableAndRemoveFilenameFromImportList();

	void tableItemSelected(int currentRow, int currentColumn, int previousRow, int previousColumn);
	void cleanUpAfterImport();

private:
	QStringList openFileBrowserForSelectingFiles();
	QString generateFileTypeFilter() const;

	QString generateUid(QString filename) const;
	void generateParentCandidates();
	int insertDataIntoTable(QString filename, std::vector<DataPtr> data);
	void clearData();
	QStringList removeDirIfSubdirIsIncluded(QStringList importFiles);
	void showProgressDialog(QProgressDialog &progress);

	QTableWidget* mTableWidget;
	QStringList mTableHeader;
	int mSelectedIndexInTable;

	QStackedWidget *mStackedWidget;

	QStringList mFileNames;
	std::vector<DataPtr> mParentCandidates;
	std::vector<DataPtr> mNotImportedData;

	FileManagerServicePtr mFileManager;
	VisServicesPtr mVisServices;

	QVBoxLayout * mTopLayout;
};

}
#endif // CXIMPORTWIDGET_H
