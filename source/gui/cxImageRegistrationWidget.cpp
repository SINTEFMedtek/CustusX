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
#include "cxRepManager.h"
#include "cxRegistrationManager.h"
#include "sscDataManager.h"
//#include "cxView3D.h"
//#include "cxView2D.h"
#include "sscProbeRep.h"

namespace cx
{
ImageRegistrationWidget::ImageRegistrationWidget(QWidget* parent) :
  RegistrationWidget(parent),
  mAddLandmarkButton(new QPushButton("Add landmark", this)),
  mEditLandmarkButton(new QPushButton("Resample landmark", this)),
  mRemoveLandmarkButton(new QPushButton("Remove landmark", this)),
  mThresholdLabel(new QLabel("Probing treshold:", this)),
  mThresholdSlider(new QSlider(Qt::Horizontal, this))
{
  //widget
  this->setObjectName("ImageRegistrationWidget");
  this->setWindowTitle("Image Registration");

  //pushbuttons
  mAddLandmarkButton->setDisabled(true);
  connect(mAddLandmarkButton, SIGNAL(clicked()),
          this, SLOT(addLandmarkButtonClickedSlot()));
  mEditLandmarkButton->setDisabled(true);
  connect(mEditLandmarkButton, SIGNAL(clicked()),
          this, SLOT(editLandmarkButtonClickedSlot()));
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()),
          this, SLOT(removeLandmarkButtonClickedSlot()));

  //slider
  connect(mThresholdSlider, SIGNAL(valueChanged(int)),
          this, SLOT(thresholdChangedSlot(int)));

  //layout
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
  mVerticalLayout->addWidget(mAddLandmarkButton);
  mVerticalLayout->addWidget(mEditLandmarkButton);
  mVerticalLayout->addWidget(mRemoveLandmarkButton);
  mVerticalLayout->addWidget(mThresholdLabel);
  mVerticalLayout->addWidget(mThresholdSlider);
}

ImageRegistrationWidget::~ImageRegistrationWidget()
{
}

void ImageRegistrationWidget::activeImageChangedSlot()
{
  RegistrationWidget::activeImageChangedSlot();

  if(mCurrentImage)
  {
    //set a default treshold
    mThresholdSlider->setRange(mCurrentImage->getPosMin(), mCurrentImage->getPosMax());
    ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
    mThresholdSlider->setValue(probeRep->getThreshold());
  }
  //enable the add point button
  mAddLandmarkButton->setEnabled(mCurrentImage!=0);
}

void ImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to add the landmark to.");
    return;
  }

  std::string uid = ssc::dataManager()->addLandmark();
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = mCurrentImage->get_rMd().inv().coord(pos_r);
  std::cout << "ImageRegistrationWidget::addLandmarkButtonClickedSlot()" << uid << ", " << pos_r << "ci=" << mCurrentImage.get() << std::endl;
  mCurrentImage->setLandmark(ssc::Landmark(uid, pos_d));

  this->nextRow();
}

void ImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to edit the landmark for.");
    return;
  }
  std::string uid = mActiveLandmark;
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

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  connect(this, SIGNAL(thresholdChanged(int)),
          probeRep.get(), SLOT(setThresholdSlot(int)));
}

void ImageRegistrationWidget::hideEvent(QHideEvent* event)
{
  RegistrationWidget::hideEvent(event);

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  disconnect(this, SIGNAL(thresholdChanged(const int)),
          probeRep.get(), SLOT(setThresholdSlot(const int)));
}

void ImageRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  RegistrationWidget::populateTheLandmarkTableWidget(image);

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //update buttons
  mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.empty());
  mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.empty());
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
  if (!mCurrentImage)
    return;
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = registrationManager()->getMasterImage();
  if(!masterImage)
    registrationManager()->setMasterImage(mCurrentImage);

  registrationManager()->doImageRegistration(mCurrentImage);

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
