#ifndef CXIMPORTWIDGET_H
#define CXIMPORTWIDGET_H

#include "cxBaseWidget.h"
#include "cxFileManagerService.h"
#include "org_custusx_core_filemanager_Export.h"

namespace cx
{

class org_custusx_core_filemanager_EXPORT ImportWidget  : public BaseWidget
{
	Q_OBJECT

public:
	ImportWidget(FileManagerServicePtr filemanager, VisServicesPtr services);

private slots:
	void importButtonClicked();
	void removeWidget(QWidget *widget);

private:
	QStringList openFileBrowserForSelectingFiles();
	QString generateFileTypeFilter();

	QString generateUid(QString filename) const;

	FileManagerServicePtr mFileManager;
	VisServicesPtr mVisServices;

	QVBoxLayout * mTopLayout;

};

}
#endif // CXIMPORTWIDGET_H