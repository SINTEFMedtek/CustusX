#include "cxImagePreviewWidget.h"

#include <QFile>
#include <QPicture>
#include <QVBoxLayout>
#include "sscMessageManager.h"

namespace cx
{
ImagePreviewWidget::ImagePreviewWidget(QWidget* parent) :
		FileWatcherWidget(parent, "ImagePreviewWidget", "Image Preview"),
		mDisplayLabel(new QLabel())
{
	mDisplayLabel->setAlignment(Qt::AlignCenter);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mDisplayLabel);

}

ImagePreviewWidget::~ImagePreviewWidget()
{
}

QString ImagePreviewWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Image preview.</h3>"
			"<p>Lets you preview an image.</p>"
			"</html>";
}

void ImagePreviewWidget::setSize(int width, int height)
{
	mDisplayLabel->setFixedSize(width, height);
}

void ImagePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
  QString imageText = "";
  if(this->internalOpenNewFile(absoluteFilePath)){
  	QPixmap map;
  	map.load(absoluteFilePath);
  	mDisplayLabel->setPixmap(map.scaled(mDisplayLabel->width(), mDisplayLabel->height(), Qt::KeepAspectRatio));
  }else{
  	imageText = "Image missing!";
  }
	mDisplayLabel->setText(imageText);
	mCurrentFile->close();

}

} /* namespace cx */
