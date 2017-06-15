#include "cxImportDataTypeWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "cxOptionsWidget.h"
#include "cxFileReaderWriterService.h"
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxImage.h"

namespace cx
{

ImportDataTypeWidget::ImportDataTypeWidget(QWidget *parent, FileManagerServicePtr filemanager, VisServicesPtr services, QString filename) :
	BaseWidget(parent, "ImportDataTypeWidget", "Import"),
	mFileManager(filemanager),
	mServices(services),
	mFileName(filename)
{
	mData = mFileManager->read(filename);

	//gui
	QVBoxLayout *topLayout = new QVBoxLayout(this);
	this->setLayout(topLayout);

	mImportAllButton = new QPushButton("Import All");
	connect(mImportAllButton, &QPushButton::clicked, this, &ImportDataTypeWidget::importAllButtonClicked);

	QHBoxLayout *hLayout = new QHBoxLayout();
	topLayout->addLayout(hLayout);
	hLayout->addWidget(new QLabel(mFileName));
	hLayout->addWidget(mImportAllButton);
	topLayout->addWidget(this->createHorizontalLine());
}

void ImportDataTypeWidget::setInitialGuessForParentFrame(DataPtr data)
{
	if(!data)
		return;

	QString base = qstring_cast(data->getName()).split(".")[0];

	std::map<QString, DataPtr> all = mServices->patient()->getDatas();
	for (std::map<QString, DataPtr>::iterator iter=all.begin(); iter!=all.end(); ++iter)
	{
		if (iter->second==data)
			continue;
		QString current = qstring_cast(iter->second->getName()).split(".")[0];
		if (base.indexOf(current)>=0)
		{
			data->get_rMd_History()->setParentSpace(iter->first);
			break;
		}
	}
}

void ImportDataTypeWidget::importAllButtonClicked()
{
	CX_LOG_DEBUG() << "IMPORTING ALL " << mFileName;

	for(int i=0; i<mData.size(); ++i)
	{
		DataPtr data = mData[i];
		if(data)
		{
			mServices->patient()->insertData(data);
		}
	}
	emit finishedImporting(this);
}

}