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

#include "cxWirePhantomWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QFrame>
#include "cxBinaryThresholdImageFilterWidget.h"
#include "cxDataInterface.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscTypeConversions.h"
#include "cxColorSelectButton.h"
#include "sscDataManager.h"
#include "sscMesh.h"
#include "cxRegistrationManager.h"

namespace cx
{
//------------------------------------------------------------------------------
WirePhantomWidget::WirePhantomWidget(RegistrationManagerPtr regManager, QWidget* parent) :
		RegistrationBaseWidget(regManager, parent, "WirePhantomWidget", "Wire Phantom")
{
	mLayout = new QVBoxLayout(this);
	mSegmentationWidget = new BinaryThresholdImageFilterWidget(this);
	mCenterlineWidget = new CenterlineWidget(this);

	mUSImageInput = SelectImageStringDataAdapter::New();
	mUSImageInput->setValueName("US Image: ");
	mUSImageInput->setHelp("Select an US volume acquired from the wire phantom.");
	connect(mUSImageInput.get(), SIGNAL(imageChanged(QString)), mSegmentationWidget, SLOT(setImageInputSlot(QString)));

	mSegmentationOutput = SelectImageStringDataAdapter::New();
	mSegmentationOutput->setValueName("Output: ");
	connect(mSegmentationOutput.get(), SIGNAL(imageChanged(QString)), mCenterlineWidget,
			SLOT(setImageInputSlot(QString)));

	mCenterlineOutput = SelectDataStringDataAdapter::New();
	mCenterlineOutput->setValueName("Output: ");
	connect(mCenterlineOutput.get(), SIGNAL(dataChanged(QString)), this, SLOT(setImageSlot(QString)));

	this->setColorSlot(QColor("green"));

	ColorSelectButton* colorButton = new ColorSelectButton("Color");
	colorButton->setColor(QColor("green"));
	colorButton->setToolTip("Select color to use when generating surfaces and centerlines.");
	connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(setColorSlot(QColor)));

	mMeasureButton = new QPushButton("Execute");
	mMeasureButton->setToolTip("Measure deviation of input volume from nominal wire cross.");
	connect(mMeasureButton, SIGNAL(clicked()), this, SLOT(measureSlot()));

	QLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(mMeasureButton);

	mLayout->addWidget(colorButton);
	mLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mUSImageInput));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addWidget(
			this->createMethodWidget(mSegmentationWidget, new ssc::LabeledComboBoxWidget(this, mSegmentationOutput),
					"Segmentation"));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addWidget(
			this->createMethodWidget(mCenterlineWidget, new ssc::LabeledComboBoxWidget(this, mCenterlineOutput),
					"Centerline"));
	mLayout->addWidget(this->createHorizontalLine());
	mLayout->addLayout(buttonsLayout);
	mLayout->addStretch();

//  connect(mResampleWidget, SIGNAL(outputImageChanged(QString)), this , SLOT(resampleOutputArrived(QString)));
	connect(mSegmentationWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(segmentationOutputArrived(QString)));
	connect(mCenterlineWidget, SIGNAL(outputImageChanged(QString)), this, SLOT(centerlineOutputArrived(QString)));

}

void WirePhantomWidget::setColorSlot(QColor color)
{
	mCenterlineWidget->setDefaultColor(color);
	mSegmentationWidget->setDefaultColor(color);

	ssc::MeshPtr mesh;

	mesh = ssc::dataManager()->getMesh(mCenterlineOutput->getValue());
	if (mesh)
		mesh->setColor(color);
	mesh = ssc::dataManager()->getMesh(mSegmentationOutput->getValue());
	if (mesh)
		mesh->setColor(color);
}


WirePhantomWidget::~WirePhantomWidget()
{
}

QString WirePhantomWidget::defaultWhatsThis() const
{
	return "<html>"
			"<h3>Segmentation and centerline extraction for the i2i registration.</h3>"
			"<p><i>Segment out blood vessels from the selected image, then extract the centerline."
			"When finished, set the result as moving or fixed data in the registration tab.</i></p>"
			"<p><b>Tip:</b> The centerline extraction can take a <b>long</b> time.</p>"
			"</html>";
}

void WirePhantomWidget::setImageSlot(QString uid)
{
}

//void WirePhantomWidget::resampleOutputArrived(QString uid)
//{
//  mResampleOutput->setValue(uid);
//}

void WirePhantomWidget::segmentationOutputArrived(QString uid)
{
	mSegmentationOutput->setValue(uid);
}

void WirePhantomWidget::centerlineOutputArrived(QString uid)
{
	mCenterlineOutput->setValue(uid);
}

void WirePhantomWidget::measureSlot()
{
	std::cout << "using centerline" << mCenterlineOutput->getValue() << std::endl;;
}


//------------------------------------------------------------------------------
}//namespace cx
