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

  mFixedDataAdapter = RegistrationFixedImageStringDataAdapter::New();
  mActiveImageAdapter = ActiveImageStringDataAdapter::New();

  //pushbuttons
  mAddLandmarkButton->setDisabled(true);
  connect(mAddLandmarkButton, SIGNAL(clicked()), this, SLOT(addLandmarkButtonClickedSlot()));
  mEditLandmarkButton->setDisabled(true);
  connect(mEditLandmarkButton, SIGNAL(clicked()), this, SLOT(editLandmarkButtonClickedSlot()));
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

  //slider
  connect(mThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdChangedSlot(int)));

  //layout
  mVerticalLayout->addWidget(new QLabel("Landmark image registration will move the active image to the fixed image."));
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mFixedDataAdapter));
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mActiveImageAdapter));
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

QString ImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based image registration.</h3>"
      "<p>Sample landmarks in the data set. </p>"
      "<p><i>Click the volume and either add or resample landmarks.</i></p>"
      "</html>";
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

  QString uid = ssc::dataManager()->addLandmark();
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = mCurrentImage->get_rMd().inv().coord(pos_r);
  //std::cout << "ImageRegistrationWidget::addLandmarkButtonClickedSlot()" << uid << ", " << pos_r << "ci=" << mCurrentImage.get() << std::endl;
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

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  connect(this, SIGNAL(thresholdChanged(int)), probeRep.get(), SLOT(setThresholdSlot(int)));
  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);
}

void ImageRegistrationWidget::hideEvent(QHideEvent* event)
{
  RegistrationWidget::hideEvent(event);

  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
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
  if (!mCurrentImage)
    return;

  //make sure the fixedData is set
  ssc::DataPtr fixedData = registrationManager()->getFixedData();
  if(!fixedData)
    registrationManager()->setFixedData(mCurrentImage);

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
