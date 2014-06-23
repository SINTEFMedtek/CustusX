#include "cxToolImagePreviewWidget.h"

#include "cxReporter.h"
//#include "cxToolConfigurationParser.h"
#include "cxTrackerConfiguration.h"
#include "cxLegacySingletons.h"
#include "cxToolManager.h"

namespace cx
{
ToolImagePreviewWidget::ToolImagePreviewWidget(QWidget* parent) :
		ImagePreviewWidget(parent)
{
	ImagePreviewWidget::setSize(200, 200);
}

ToolImagePreviewWidget::~ToolImagePreviewWidget()
{}

QString ToolImagePreviewWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Tool image preview.</h3>"
			"<p>Lets you preview a tools image.</p>"
			"</html>";
}

void ToolImagePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
	TrackerConfigurationPtr config = toolManager()->getConfiguration();
	const QString imageAbsoluteFilePath = config->getToolPictureFilename(absoluteFilePath);

//	IgstkTool::InternalStructure tool;

//	ToolFileParser parser(absoluteFilePath);
//	tool = parser.getTool();

//	const QString imageAbsoluteFilePath = tool.mPictureFileName;
	ImagePreviewWidget::previewFileSlot(imageAbsoluteFilePath);
}
}//namespace cx
