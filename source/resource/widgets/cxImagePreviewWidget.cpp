/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImagePreviewWidget.h"

#include <QFile>
#include <QPicture>
#include <QVBoxLayout>
#include <QLabel>


namespace cx
{
ImagePreviewWidget::ImagePreviewWidget(QWidget* parent) :
		FileWatcherWidget(parent, "image_preview_widget", "Image Preview"),
		mDisplayLabel(new QLabel())
{
	this->setToolTip("Preview of an image file");
	mDisplayLabel->setAlignment(Qt::AlignCenter);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mDisplayLabel);

}

ImagePreviewWidget::~ImagePreviewWidget()
{
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
