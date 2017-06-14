#include "cxImportWidget.h"

#include <QFileDialog>
#include <QPushButton>
#include "cxForwardDeclarations.h"
#include "cxFileReaderWriterService.h"
#include "cxVisServices.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxProfile.h"

namespace cx
{

ImportWidget::ImportWidget(cx::FileManagerServicePtr filemanager, cx::VisServicesPtr services) :
	BaseWidget(NULL, "ImportDataWidget", "Import Data"),
	mFileManager(filemanager),
	mVisServices(services)
{

	QVBoxLayout * topLayout = new QVBoxLayout();
	this->setLayout(topLayout);
	QPushButton *importButton = new QPushButton("Import");
	connect(importButton, &QPushButton::clicked, this, &ImportWidget::importButtonClicked);
	topLayout->addWidget(importButton);
}

void ImportWidget::importButtonClicked()
{
	QStringList filenames = this->openFileBrowserForSelectingFiles();

	//TODO display list of selected files?

	QString filename;
	foreach (filename, filenames)
	{
		std::vector<DataPtr> imported_data = mFileManager->read(filename);
		for(int i=0; i<imported_data.size(); ++i)
		{
			//TODO ImportDataTypeWidget
			DataPtr data = imported_data[i];
			if(data)
				mVisServices->patient()->insertData(data);
		}
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


}