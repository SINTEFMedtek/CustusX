/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXEXPORTDATATYPEWIDGET_H
#define CXEXPORTDATATYPEWIDGET_H

#include "org_custusx_core_filemanager_Export.h"
#include "cxBaseWidget.h"
#include <QPushButton>
#include <QCheckBox>
#include "cxForwardDeclarations.h"
#include "cxSelectDataStringProperty.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSessionStorageService.h"
#include "cxFileManagerService.h"

namespace cx
{

//Note: Workaround for: Template classes not supported by Q_OBJECT
class Q_OBJECTExportDataTypeWidget : public BaseWidget
{
	Q_OBJECT
public:
	Q_OBJECTExportDataTypeWidget(QWidget *parent, QString dataType, FileManagerServicePtr filemanager, VisServicesPtr services, SelectDataStringPropertyBasePtr property);

protected slots:
	virtual void exportButtonClicked();
	virtual void exportAllButtonClicked();

protected:
	SelectDataStringPropertyBasePtr mSelectedData;
	VisServicesPtr mServices;
	FileManagerServicePtr mFileManager;
	QString mDataType;

	std::vector<QCheckBox*> mCheckboxes;
	QPushButton *mExportButton;
	QPushButton *mExportAllButton;
};

//---------

template<typename TYPE>
class org_custusx_core_filemanager_EXPORT ExportDataTypeWidget : public Q_OBJECTExportDataTypeWidget
{
public:
	ExportDataTypeWidget(QWidget *parent, FileManagerServicePtr filemanager, VisServicesPtr services, SelectDataStringPropertyBasePtr property) :
		Q_OBJECTExportDataTypeWidget(parent, TYPE::getTypeName(), filemanager, services, property)
	{
		connect(mExportButton, &QPushButton::clicked, this, &ExportDataTypeWidget::exportButtonClicked);
		connect(mExportAllButton, &QPushButton::clicked, this, &ExportDataTypeWidget::exportAllButtonClicked);
	}

protected:
	void save(QString suffix, QString data_uid)
	{
		boost::shared_ptr<TYPE> data = boost::dynamic_pointer_cast<TYPE>(mServices->patient()->getData(data_uid));
		if(!data)
			CX_LOG_ERROR() << "Data with uid: " << data_uid << " not found";
		QString export_folder = mServices->session()->getSubFolder("Export");
		QString filename = export_folder+"/"+data_uid+"."+suffix;
		mFileManager->save(data,filename);
	}

	void saveAsSelected(QString uid)
	{
		CX_LOG_DEBUG() << "Saving " << uid;
		for(int i=0; i<mCheckboxes.size(); ++i)
		{
			if((mCheckboxes[i]->checkState() == Qt::Checked))
			{
				CX_LOG_DEBUG() << "		-> " <<  mCheckboxes[i]->text();
				QString suffix = mCheckboxes[i]->text();
				this->save(suffix, uid);
			}
		}
	}

	virtual void exportButtonClicked()
	{
		CX_LOG_DEBUG() << "EXPORTING	" << mSelectedData->getValue();
		QString uid = mSelectedData->getValue();
		this->saveAsSelected(uid);
	}

	virtual void exportAllButtonClicked()
	{
		CX_LOG_DEBUG() << "EXPORTING ALL " << mDataType;

		std::map<QString, boost::shared_ptr<TYPE> > all_data = mServices->patient()->getDataOfType<TYPE>();
		for (typename std::map<QString, boost::shared_ptr<TYPE> >::const_iterator iter = all_data.begin(); iter != all_data.end(); ++iter)
		{
			this->saveAsSelected(iter->first);
		}
	}

};

}

#endif // CXEXPORTDATATYPEWIDGET_H
