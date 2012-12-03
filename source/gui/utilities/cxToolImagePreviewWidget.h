#ifndef CXTOOLIMAGEPREVIEWWIDGET_H_
#define CXTOOLIMAGEPREVIEWWIDGET_H_

#include "cxImagePreviewWidget.h"
namespace cx
{
/**
 * \class ToolImagePreviewWidget
 *
 * \brief Widget for displaying a tools image.
 *
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class ToolImagePreviewWidget : public ImagePreviewWidget
{
	Q_OBJECT

public:
	ToolImagePreviewWidget(QWidget* parent);
	virtual ~ToolImagePreviewWidget();

	virtual QString defaultWhatsThis() const;

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath);
};
} //namespace cx
#endif /* CXTOOLIMAGEPREVIEWWIDGET_H_ */
