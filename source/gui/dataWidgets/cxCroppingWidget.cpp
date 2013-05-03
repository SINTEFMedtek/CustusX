#include "cxCroppingWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <vtkImageData.h>
#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDefinitionStrings.h"
#include "sscUtilHelpers.h"
#include "sscMessageManager.h"
#include "sscRegistrationTransform.h"
#include "sscImageAlgorithms.h"
#include "cxInteractiveCropper.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxDataManager.h"

namespace cx
{


CroppingWidget::CroppingWidget(QWidget* parent) : 
  BaseWidget(parent, "CroppingWidget", "Crop")
{
  mInteractiveCropper = viewManager()->getCropper();
  connect(mInteractiveCropper.get(), SIGNAL(changed()), this, SLOT(cropperChangedSlot()));

  QVBoxLayout* layout = new QVBoxLayout(this);

  this->setToolTip(this->defaultWhatsThis());

  QGroupBox* activeGroupBox = new QGroupBox("Interactive cropper");
  activeGroupBox->setToolTip(this->defaultWhatsThis());
  layout->addWidget(activeGroupBox);
  QVBoxLayout* activeLayout = new QVBoxLayout(activeGroupBox);

  mUseCropperCheckBox = new QCheckBox("Use Cropper");
  mUseCropperCheckBox->setToolTip("Turn on cropping for the active volume.");
  connect(mUseCropperCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(useCropping(bool)));
  activeLayout->addWidget(mUseCropperCheckBox);

  mShowBoxCheckBox = new QCheckBox("Show box (i)");
  mShowBoxCheckBox->setToolTip("Show crop box in 3D view. Press 'i' in the view to do the same.");
  connect(mShowBoxCheckBox, SIGNAL(toggled(bool)), mInteractiveCropper.get(), SLOT(showBoxWidget(bool)));
  activeLayout->addWidget(mShowBoxCheckBox);

  mBoundingBoxDimensions = new QLabel("?, ?, ?");
  mBoundingBoxDimensions->setToolTip("The dimensions of the croppers boundingbox.");
  activeLayout->addWidget(mBoundingBoxDimensions);

  mBBWidget = new BoundingBoxWidget(this);
  layout->addWidget(mBBWidget);
  connect(mBBWidget, SIGNAL(changed()), this, SLOT(boxValuesChanged()));

  QPushButton* cropClipButton = new QPushButton("Create new cropped volume");
  cropClipButton->setToolTip("Create a new volume containing only the volume inside the crop box.");
  connect(cropClipButton, SIGNAL(clicked()), this, SLOT(cropClipButtonClickedSlot()));
  layout->addWidget(cropClipButton);

  layout->addStretch();

  this->cropperChangedSlot();
}

QString CroppingWidget::defaultWhatsThis() const
{
	return "<html>"
		"<h3>Functionality for cropping a volume.</h3>"
		"<p>"
		"Lets you crop a volume by defining a bounding box along the volume "
		"axis. Everything outside the box is not shown. To make the crop permanent, "
		"press the button to create a new volume from the crop."
		"</p>"
		"<p><i></i></p>"
		"</html>";
}

void CroppingWidget::boxValuesChanged()
{
  mInteractiveCropper->setBoundingBox(mBBWidget->getValue());
}

void CroppingWidget::cropperChangedSlot()
{
	std::vector<int> dims = mInteractiveCropper->getDimensions();
	QString dimensionText = "Dimensions: "+qstring_cast(dims.at(0))+", "+qstring_cast(dims.at(1))+", "+qstring_cast(dims.at(2));
	mBoundingBoxDimensions->setText(dimensionText);
	mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
	mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());

	mBBWidget->setValue(mInteractiveCropper->getBoundingBox(), mInteractiveCropper->getMaxBoundingBox());
}

ssc::ImagePtr CroppingWidget::cropClipButtonClickedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  QString outputBasePath = patientService()->getPatientData()->getActivePatientFolder();

  ssc::ImagePtr retval = cropImage(image);
  ssc::dataManager()->loadData(retval);
  ssc::dataManager()->saveImage(retval, outputBasePath);
  return retval;
}

}
