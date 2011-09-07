#include "cxLandmarkImageRegistrationWidget.h"

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
LandmarkImageRegistrationWidget::LandmarkImageRegistrationWidget(RegistrationManagerPtr regManager, QWidget* parent, QString objectName, QString windowTitle) :
  LandmarkRegistrationWidget(regManager, parent, objectName, windowTitle),
  mThresholdLabel(new QLabel("Probing treshold:", this)),
  mThresholdSlider(new QSlider(Qt::Horizontal, this))
{
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

  mRemoveLandmarkButton = new QPushButton("Clear", this);
  mRemoveLandmarkButton->setToolTip("Clear selected landmark");
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()), this, SLOT(removeLandmarkButtonClickedSlot()));

  //slider
  connect(mThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(thresholdChangedSlot(int)));

  //layout
  mVerticalLayout->addWidget(new ssc::LabeledComboBoxWidget(this, mActiveImageAdapter));
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);

  QHBoxLayout* landmarkButtonsLayout = new QHBoxLayout;
  landmarkButtonsLayout->addWidget(mAddLandmarkButton);
  landmarkButtonsLayout->addWidget(mEditLandmarkButton);
  landmarkButtonsLayout->addWidget(mRemoveLandmarkButton);
  mVerticalLayout->addLayout(landmarkButtonsLayout);

  mVerticalLayout->addWidget(mThresholdLabel);
  mVerticalLayout->addWidget(mThresholdSlider);
}

LandmarkImageRegistrationWidget::~LandmarkImageRegistrationWidget()
{
}

QString LandmarkImageRegistrationWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>Landmark based image registration.</h3>"
      "<p>Sample landmarks in the data set. </p>"
      "<p><i>Click the volume and either add or resample landmarks.</i></p>"
      "<p>Landmark image registration will move the active image to the fixed image</p>"
      "</html>";
}

void LandmarkImageRegistrationWidget::activeImageChangedSlot()
{
  LandmarkRegistrationWidget::activeImageChangedSlot();

  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();

  if(image)
  {
    //set a default treshold
    mThresholdSlider->setRange(image->getMin(), image->getMax());
    ssc::ProbeRepPtr probe = this->getProbeRep();
    if (probe)
      mThresholdSlider->setValue(probe->getThreshold());

    if(!mManager->getFixedData())
    	mManager->setFixedData(image);
  }

  //enable the add point button
  mAddLandmarkButton->setEnabled(image!=0);
}

ssc::ProbeRepPtr LandmarkImageRegistrationWidget::getProbeRep()
{
  if (!viewManager()->get3DView(0,0))
    return ssc::ProbeRepPtr();

  return RepManager::findFirstRep<ssc::ProbeRep>(viewManager()->get3DView(0,0)->getReps());
}

void LandmarkImageRegistrationWidget::addLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to add the landmark to.");
    return;
  }

  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
  	return;

  QString uid = ssc::dataManager()->addLandmark();
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
  //std::cout << "LandmarkImageRegistrationWidget::addLandmarkButtonClickedSlot()" << uid << ", " << pos_r << "ci=" << mCurrentImage.get() << std::endl;
  image->setLandmark(ssc::Landmark(uid, pos_d));

  this->nextRow();
}

void LandmarkImageRegistrationWidget::editLandmarkButtonClickedSlot()
{
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(!probeRep)
  {
    ssc::messageManager()->sendError("Could not find a rep to edit the landmark for.");
    return;
  }

  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
  	return;


  QString uid = mActiveLandmark;
  ssc::Vector3D pos_r = probeRep->getPosition();
  ssc::Vector3D pos_d = image->get_rMd().inv().coord(pos_r);
  image->setLandmark(ssc::Landmark(uid, pos_d));

  this->nextRow();
}

void LandmarkImageRegistrationWidget::removeLandmarkButtonClickedSlot()
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
  	return;

  image->removeLandmark(mActiveLandmark);

  this->nextRow();
}

void LandmarkImageRegistrationWidget::cellClickedSlot(int row, int column)
{
  LandmarkRegistrationWidget::cellClickedSlot(row, column);

  mEditLandmarkButton->setEnabled(true);
  mRemoveLandmarkButton->setEnabled(true);
}

void LandmarkImageRegistrationWidget::showEvent(QShowEvent* event)
{
  LandmarkRegistrationWidget::showEvent(event);

  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(probeRep)
  {
    connect(this, SIGNAL(thresholdChanged(int)), probeRep.get(), SLOT(setThresholdSlot(int)));
  }

  viewManager()->setRegistrationMode(ssc::rsIMAGE_REGISTRATED);
}

void LandmarkImageRegistrationWidget::hideEvent(QHideEvent* event)
{
  LandmarkRegistrationWidget::hideEvent(event);

//  ssc::ProbeRepPtr probeRep = repManager()->getProbeRep("ProbeRep_1");
  ssc::ProbeRepPtr probeRep = this->getProbeRep();
  if(probeRep)
    disconnect(this, SIGNAL(thresholdChanged(const int)), probeRep.get(), SLOT(setThresholdSlot(const int)));
  viewManager()->setRegistrationMode(ssc::rsNOT_REGISTRATED);
}

void LandmarkImageRegistrationWidget::populateTheLandmarkTableWidget()
{
  LandmarkRegistrationWidget::populateTheLandmarkTableWidget();

  std::vector<ssc::Landmark> landmarks =  this->getAllLandmarks();

  //update buttons
  mRemoveLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
  mEditLandmarkButton->setEnabled(!landmarks.empty() && !mActiveLandmark.isEmpty());
}

ssc::LandmarkMap LandmarkImageRegistrationWidget::getTargetLandmarks() const
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
  	return ssc::LandmarkMap();

  return image->getLandmarks();
}

void LandmarkImageRegistrationWidget::thresholdChangedSlot(const int value)
{
  emit thresholdChanged(value);

  QString text = "Probing threshold: ";
  QString valueText;
  valueText.setNum(value);
  text.append(valueText);
  mThresholdLabel->setText(text);
}

/** Return transform from target space to reference space
 *
 */
ssc::Transform3D LandmarkImageRegistrationWidget::getTargetTransform() const
{
  ssc::ImagePtr image = ssc::dataManager()->getActiveImage();
  if (!image)
    return ssc::Transform3D::Identity();
  return image->get_rMd();
}


}//namespace cx
