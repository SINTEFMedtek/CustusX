#include "cxImageRegistrationWidget.h"

#include <sstream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QSlider>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscProbeRep.h"
#include "sscLabeledComboBoxWidget.h"
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "cxViewManager.h"
#include "cxSettings.h"
#include "cxView3D.h"

namespace cx
{
ImageRegistrationWidget::ImageRegistrationWidget(QWidget* parent, QString objectName, QString windowTitle) :
  RegistrationWidget(parent, objectName, windowTitle),
  mThresholdLabel(new QLabel("Probing treshold:", this)),
  mThresholdSlider(new QSlider(Qt::Horizontal, this))
{
  mFixedDataAdapter = RegistrationFixedImageStringDataAdapter::New();
  mActiveImageAdapter = ActiveImageStringDataAdapter::New();

  //pushbuttons
  mAddLandmarkButton = new QPushButton("Add", this);
  mAddLandmarkButton->setToolTip("Add landmark");
  mAddLandmarkButton->setDisabled(true);
  connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));

  mEditLandmarkButton = new QPushButton("Resample", this);
  mEditLandmarkButton->setToolTip("Resample landmark");
  mEditLandmarkButton->setDisabled(true);
  connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));

  mRemoveLandmarkButton = new QPushButton("Remove", this);
  mRemoveLandmarkButton->setToolTip("Remove landmark");
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

  bool autoReg = settings()->value("autoLandmarkRegistration").toBool();
  mRegisterButton = new QPushButton("Register", this);
  mRegisterButton->setToolTip("Perform registration");
  mRegisterButton->setEnabled(!autoReg);
  connect(mRegisterButton, SIGNAL(clicked()), this, SLOT(registerSlot()));

  mAutoRegisterCheckBox = new QCheckBox("Auto", this);
  mAutoRegisterCheckBox->setToolTip("Automatic registration whenever a landmark has changed");
  connect(mAutoRegisterCheckBox, SIGNAL(clicked(bool)), this, SLOT(autoRegisterSlot(bool)));
  mAutoRegisterCheckBox->setChecked(autoReg);

  //slider
  connect(mThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdChangedSlot(int)));

  //layout
  //moved to help text   mVerticalLayout->addWidget(new QLabel("Landmark image registration will move the active image to the fixed image."));
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFixedDataAdapter));
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mActiveImageAdapter));
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);

  QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
  landmarkButtonsLayout->addWidget(mAddLandmarkButton);
  landmarkButtonsLayout->addWidget(mEditLandmarkButton);
  landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
  mVerticalLayout->addLayout(landmarkButtonsLayout);

  QHBoxLayout* regLayout = new QHBoxLayout;
  regLayout->addWidget(mAutoRegisterCheckBox);
  regLayout->addWidget(mRegisterButton);
  mVerticalLayout->addLayout(regLayout);

  mVerticalLayout->addWidget(mThresholdLabel);
  mVerticalLayout->addWidget(mThresholdSlider);
}

void ImageRegistrationWidget::registerSlot()
{
  this->internalPerformRegistration(true);
}

void ImageRegistrationWidget::autoRegisterSlot(bool checked)
{
  settings()->setValue("autoLandmarkRegistration", checked);
  mRegisterButton->setEnabled(!checked);
}

ImageRegistrationWidget::~ImageRegistrationWidget()
{
}

QString ImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based image registration.</h3>"
      "<p>Sample landmarks in the data set. </p>"
      "<p><i>Click the volume and either add or resample landmarks.</i></p>"
      "<p>Landmark image registration will move the active image to the fixed image</p>"
      "</html>";
}

void ImageRegistrationWidget::activeImageChangedSlot()
{
  RegistrationWidget::activeImageChangedSlot();

  if(mCurrentImage)
  {
    //set a default treshold
    mThresholdSlider->setRange(mCurrentImage->getMin(), mCurrentImage->getMax());
    ssc::ProbeRepPtr probe = this->getProbeRep();
    if (probe)
      mThresholdSlider->setValue(probe->getThreshold());
  }
  //enable the add point button
  mAddLandmarkButton->setEnabled(mCurrentImage!=0);
}

ssc::ProbeRepPtr ImageRegistrationWidget::getProbeRep()
{
  if (!viewManager()->get3DView(0,0))
    return ssc::ProbeRepPtr();
  return repManager()->findFirstRep<ssc::ProbeRep>(viewManager()->get3DView(0,0)->getReps());
}

void ImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to add the landmark to.");
    return;
  }

  QString uid = ssc::dataManager()->addLandmark();
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = mCurrentImage->get_rMd().inv().coord(pos_r);
  //std::cout << "ImageRegistrationWidget::addLandmarkButtonClickedSlot()" << uid << ", " << pos_r << "ci=" << mCurrentImage.get() << std::endl;
  mCurrentImage->setLandmark(ssc::Landmark(uid, pos_d));

  this->nextRow();
}

void ImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to edit the landmark for.");
    return;
  }
  QString uid = mActiveLandmark;
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = mCurrentImage->get_rMd().inv().coord(pos_r);
  mCurrentImage->setLandmark(ssc::Landmark(uid, pos_d));

  this->nextRow();
}

void ImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
  if(mCurrentImage)
    mCurrentImage->removeLandmark(mActiveLandmark);

  this->nextRow();
}

void ImageRegistrationWidget::cellClickedSlot(int row, int column)
{
  RegistrationWidget::cellClickedSlot(row, column);

  mEditLandmarkButton->setEnabled(true);
  mRemoveLandmarkButton->setEnabled(true);
}

void ImageRegistrationWidget::showEvent(QShowEvent* event)
{
  RegistrationWidget::showEvent(event);

  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(probeRep)
  {
    connect(this, SIGNAL(thresholdChanged(int)), probeRep.get(), SLOT(setThresholdSlot(int)));
  }
  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);
}

void ImageRegistrationWidget::hideEvent(QHideEvent* event)
{
  RegistrationWidget::hideEvent(event);

//  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(probeRep)
    disconnect(this, SIGNAL(thresholdChanged(const int)), probeRep.get(), SLOT(setThresholdSlot(const int)));
  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void ImageRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  RegistrationWidget::populateTheLandmarkTableWidget(image);

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //update buttons
  mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
  mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

ssc::LandmarkMap ImageRegistrationWidget::getTargetLandmarks() const
{
  if(mCurrentImage)
    return mCurrentImage->getLandmarks();
  else
    return ssc::LandmarkMap();
}

void ImageRegistrationWidget::thresholdChangedSlot(const int value)
{
  emit thresholdChanged(value);

  QString text = "Probing threshold: ";
  QString valueText;
  valueText.setNum(value);
  text.append(valueText);
  mThresholdLabel->setText(text);
}

void ImageRegistrationWidget::performRegistration()
{
  bool autoReg = settings()->value("autoLandmarkRegistration").toBool();
  this->internalPerformRegistration(autoReg);
}

void ImageRegistrationWidget::internalPerformRegistration(bool doIt)
{
  if (doIt && mCurrentImage)
  {
    //make sure the fixedData is set
    ssc::DataPtr fixedData = registrationManager()->getFixedData();
    if(!fixedData)
      registrationManager()->setFixedData(mCurrentImage);

    registrationManager()->doImageRegistration(mCurrentImage);
  }

  this->updateAvarageAccuracyLabel();
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D ImageRegistrationWidget::getTargetTransform() const
{
  if (!mCurrentImage)
    return ssc::Transform3D();
  return mCurrentImage->get_rMd();
}


}//namespace cx
