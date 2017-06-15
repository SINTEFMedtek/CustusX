#ifndef CXIMPORTDATATYPEWIDGET_H
#define CXIMPORTDATATYPEWIDGET_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxBaseWidget.h"
#include "cxForwardDeclarations.h"
#include "cxLogger.h"
#include  <QPushButton>
#include "cxVisServices.h"
#include "cxFileManagerService.h"
#include "cxPatientModelService.h"
#include "cxRegistrationTransform.h"

namespace cx
{

class org_custusx_core_filemanager_EXPORT ImportDataTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	ImportDataTypeWidget(QWidget *parent, FileManagerServicePtr filemanager, VisServicesPtr services, QString filename);

protected slots:
	virtual void importAllButtonClicked();

signals:
	void finishedImporting(QWidget *self);

private:
	/** Use heuristics to guess a parent frame,
	 *  based on similarities in name.
	 */
	void setInitialGuessForParentFrame(DataPtr data);

	QString mFileName;
	VisServicesPtr mServices;
	FileManagerServicePtr mFileManager;

	std::vector<QCheckBox*> mCheckboxes;
	QPushButton *mImportAllButton;
	std::vector<DataPtr> mData;

};

}
#endif // CXIMPORTDATATYPEWIDGET_H