#ifndef CXIMAGEPREVIEWWIDGET_H_
#define CXIMAGEPREVIEWWIDGET_H_

#include "cxFileWatcherWidget.h"

namespace cx
{

/**
 * \class ImagePreviewWidget
 *
 * \brief Widget for displaying images.
 *
 * \date Nov 7, 2012
 * \author Janne Beate Bakeng, SINTEF
 */
class ImagePreviewWidget : public FileWatcherWidget
{
	Q_OBJECT

public:
	ImagePreviewWidget(QWidget* parent);
	virtual ~ImagePreviewWidget();

	virtual QString defaultWhatsThis() const;

	void setSize(int width, int height);

public slots:
	virtual void previewFileSlot(const QString& absoluteFilePath);

private:
	QLabel*		mDisplayLabel;
};

} /* namespace cx */
#endif /* CXIMAGEPREVIEWWIDGET_H_ */
