#ifndef CXIMPORTWIDGET_H
#define CXIMPORTWIDGET_H

#include "cxBaseWidget.h"
#include "cxFileManagerService.h"
#include "org_custusx_core_filemanager_Export.h"

class QTableWidget;
class QStackedWidget;

namespace cx
{

class org_custusx_core_filemanager_EXPORT ImportWidget  : public BaseWidget
{
	Q_OBJECT

public:
	ImportWidget(FileManagerServicePtr filemanager, VisServicesPtr services);

signals:
	void finishedImporting();
	void parentCandidatesUpdated();

private slots:
	void importButtonClicked();
	void cancelButtonClicked();
	void addMoreFilesButtonClicked();
	void removeWidget(QWidget *widget);

	void tableItemSelected(int row, int column);
	void cleanUpAfterImport();

private:
	QStringList openFileBrowserForSelectingFiles();
	QString generateFileTypeFilter() const;

	QString generateUid(QString filename) const;
	std::vector<DataPtr> generateParentCandidates(std::vector<DataPtr> notLoadedData) const;
	int insertDataIntoTable(QString filename, std::vector<DataPtr> data);

	QTableWidget* mTableWidget;
	QStringList mTableHeader;
	int mSelectedIndexInTable;

	QStackedWidget *mStackedWidget;

	std::vector<DataPtr> mData;
	std::vector<DataPtr> mParentCandidates;

	FileManagerServicePtr mFileManager;
	VisServicesPtr mVisServices;

	QVBoxLayout * mTopLayout;

};

}
#endif // CXIMPORTWIDGET_H