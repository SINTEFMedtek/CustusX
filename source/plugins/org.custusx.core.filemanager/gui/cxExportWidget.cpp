#include "cxExportWidget.h"

#include <QVBoxLayout>
#include <QString>
#include <QStringList>
#include "cxExportDataTypeWidget.h"
#include "cxStringPropertySelectPointMetric.h"
#include "cxVisServices.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxPointMetric.h"

namespace cx
{

ExportWidget::ExportWidget(FileManagerServicePtr filemanager, VisServicesPtr services) :
	BaseWidget(NULL, "export_data_widget", "Export Data"),
	mFileManager(filemanager),
	mVisServices(services)
{	
	QVBoxLayout * topLayout = new QVBoxLayout();

	ExportDataTypeWidget<Image> *exportImageWidget = new ExportDataTypeWidget<Image>(this, filemanager, mVisServices, StringPropertySelectImage::New(services->patient()));
	ExportDataTypeWidget<Mesh> *exportMeshWidget = new ExportDataTypeWidget<Mesh>(this, filemanager, mVisServices, StringPropertySelectMesh::New(services->patient()));
	ExportDataTypeWidget<PointMetric> *exportPointMetricWidget = new ExportDataTypeWidget<PointMetric>(this, filemanager, mVisServices, StringPropertySelectPointMetric::New(services->patient()));

	this->setLayout(topLayout);
	topLayout->addWidget(exportImageWidget);
	topLayout->addWidget(exportMeshWidget);
	topLayout->addWidget(exportPointMetricWidget);
	topLayout->addStretch();
}


}
