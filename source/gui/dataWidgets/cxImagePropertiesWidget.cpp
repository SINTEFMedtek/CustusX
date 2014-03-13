// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxImagePropertiesWidget.h"

#include <QComboBox>
#include <QVBoxLayout>
#include "sscDataManager.h"
#include <sscImage.h>
#include "cxLegacySingletons.h"

namespace cx
{

ImagePropertiesWidget::ImagePropertiesWidget(QWidget* parent) :
		BaseWidget(parent, "ImagePropertiesWidget", "Image Properties")
{
	mInterpolationType = new QComboBox(this);
	mInterpolationType->insertItem(0, "Nearest");
	mInterpolationType->insertItem(1, "Linear");
	mInterpolationType->insertItem(2, "Cubic");

	connect(mInterpolationType, SIGNAL(currentIndexChanged(int)), this, SLOT(interpolationTypeChanged(int)));

	mActiveImageProxy = ActiveImageProxy::New(dataService());
	connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));

	QLabel* interpolationTypeLabel = new QLabel("Volume interpolation type", this);

	QHBoxLayout* layout =  new QHBoxLayout(this);
	layout->addWidget(interpolationTypeLabel);
	layout->addWidget(mInterpolationType);
}

void ImagePropertiesWidget::interpolationTypeChanged(int index)
{
	ImagePtr image = dataManager()->getActiveImage();
	if (image)
		image->setInterpolationType(index);
}

void ImagePropertiesWidget::activeImageChangedSlot()
{
	ImagePtr activeImage = dataManager()->getActiveImage();
	if (activeImage)
	{
		mInterpolationType->setCurrentIndex(activeImage->getInterpolationType());
	}
}

QString ImagePropertiesWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Image properties</h3>"
		"<p>"
		"Set image (volume) properties."
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

}//end namespace cx
