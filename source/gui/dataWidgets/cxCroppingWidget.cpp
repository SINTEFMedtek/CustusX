#include "cxCroppingWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include "sscStringDataAdapter.h"
#include "sscLabeledComboBoxWidget.h"
#include "sscDefinitionStrings.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMessageManager.h"
#include "sscRegistrationTransform.h"
#include "cxInteractiveCropper.h"
#include "cxPatientData.h"
#include <vtkImageData.h>
#include "sscImageAlgorithms.h"
#include "cxPatientService.h"

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

  mXRange = new SliderRangeGroupWidget(this);
  mXRange->setName("X");
  mXRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mXRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mXRange);

  mYRange = new SliderRangeGroupWidget(this);
  mYRange->setName("Y");
  mYRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mYRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mYRange);

  mZRange = new SliderRangeGroupWidget(this);
  mZRange->setName("Z");
  mZRange->setRange(ssc::DoubleRange(-2000, 2000, 1));
  connect(mZRange, SIGNAL(valueChanged(double,double)), this, SLOT(boxValuesChanged()));
  layout->addWidget(mZRange);


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
  std::pair<double,double> x = mXRange->getValue();
  std::pair<double,double> y = mYRange->getValue();
  std::pair<double,double> z = mZRange->getValue();
  ssc::DoubleBoundingBox3D box(x.first, x.second, y.first, y.second, z.first, z.second);
  mInteractiveCropper->setBoundingBox(box);
}

void CroppingWidget::cropperChangedSlot()
{
  mUseCropperCheckBox->setChecked(mInteractiveCropper->getUseCropping());
  mShowBoxCheckBox->setChecked(mInteractiveCropper->getShowBoxWidget());

  mXRange->blockSignals(true);
  mYRange->blockSignals(true);
  mZRange->blockSignals(true);

  ssc::DoubleBoundingBox3D range =  mInteractiveCropper->getMaxBoundingBox();
  mXRange->setRange(ssc::DoubleRange(range.begin()[0], range.begin()[1], 1));
  mYRange->setRange(ssc::DoubleRange(range.begin()[2], range.begin()[3], 1));
  mZRange->setRange(ssc::DoubleRange(range.begin()[4], range.begin()[5], 1));

  ssc::DoubleBoundingBox3D box =  mInteractiveCropper->getBoundingBox();
  mXRange->setValue(std::make_pair(box.begin()[0], box.begin()[1]));
  mYRange->setValue(std::make_pair(box.begin()[2], box.begin()[3]));
  mZRange->setValue(std::make_pair(box.begin()[4], box.begin()[5]));

  mXRange->blockSignals(false);
  mYRange->blockSignals(false);
  mZRange->blockSignals(false);
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
