/*
 * cxVolumePropertiesWidget.cpp
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */
#include "cxVolumePropertiesWidget.h"


#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
//#include <QComboBox>
//#include "sscProbeRep.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
//#include "cxRegistrationManager.h"
//#include "sscToolManager.h"
#include "cxViewManager.h"
//#include "cxRepManager.h"
//#include "cxView3D.h"
//#include "cxView2D.h"

#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxShadingWidget.h"

namespace cx
{


ActiveVolumeWidget::ActiveVolumeWidget(QWidget* parent) :
  QWidget(parent),
  mImagesComboBox(new QComboBox(this))
{
  QVBoxLayout* layout = new QVBoxLayout(this);
  this->setObjectName("ActiveVolumeWidget");
  layout->setMargin(0);

  //combobox
  mImagesComboBox->setEditable(false);
  mImagesComboBox->setEnabled(false);
  connect(mImagesComboBox, SIGNAL(currentIndexChanged(const QString& )), this, SLOT(imageSelectedSlot(const QString& )));

  //layout
  layout->addWidget(mImagesComboBox);

  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SLOT(populateTheImageComboBoxSlot()));
  connect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)), this, SLOT(populateTheImageComboBoxSlot()));
  this->populateTheImageComboBoxSlot();

  // Delete image
//  connect(this, SIGNAL(deleteImage(ssc::ImagePtr)), ssc::dataManager(), SLOT(deleteImageSlot(ssc::ImagePtr)));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)), viewManager(), SLOT(deleteImageSlot(ssc::ImagePtr)));

  //listen for active image changed from the datamanager
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(std::string)), this, SLOT(activeImageChangedSlot()));
}

ActiveVolumeWidget::~ActiveVolumeWidget()
{
}

//void ActiveVolumeWidget::deleteCurrentImageSlot()
//{
//  if (mCurrentImage.use_count() == 0)
//  {
//    ssc::messageManager()->sendWarning("Can't delete image, no current Image!");
//    return;
//  }
//  emit deleteImage(mCurrentImage);
//}

void ActiveVolumeWidget::populateTheImageComboBoxSlot()
{
  mImagesComboBox->blockSignals(true);
  mImagesComboBox->clear();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = ssc::dataManager()->getImages();
  if(images.size() == 0)
  {
    mImagesComboBox->insertItem(1, QString("Import an image to begin..."));
    mImagesComboBox->setEnabled(false);
    return;
  }

  mImagesComboBox->setEnabled(true);

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  mImagesComboBox->insertItem(1, QString("<No image selected>"));
  int listPosition = 2;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }
  mImagesComboBox->blockSignals(false);
}

void ActiveVolumeWidget::imageSelectedSlot(const QString& comboBoxText)
{
  //messageMan()->sendInfo("New image selected: "+comboBoxText.toStdString());
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("...")
     || comboBoxText.endsWith(">"))
  {
    // Create empty current image
   // mCurrentImage.reset();
    ssc::dataManager()->setActiveImage(ssc::ImagePtr());

    //emit currentImageChanged(mCurrentImage); //TODO remove
    return;
  }

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = ssc::dataManager()->getImage(imageId);
  if(!image)
  {
    ssc::messageManager()->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  //Set new current image
  ssc::dataManager()->setActiveImage(image);
}

void ActiveVolumeWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = ssc::dataManager()->getActiveImage();

  QString uid;
  if (activeImage)
      uid = qstring_cast(activeImage->getUid());
  this->imageSelectedSlot(uid);

  //find the index in the combobox and set it
  int index = mImagesComboBox->findText(uid);
  mImagesComboBox->blockSignals(true);
  mImagesComboBox->setCurrentIndex(index);
  mImagesComboBox->blockSignals(false);
}


/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) : QWidget(parent)
{
  this->setObjectName("VolumePropertiesWidget");
  this->setWindowTitle("Volume Properties");

  QVBoxLayout* layout = new QVBoxLayout(this);

  layout->addWidget(new ActiveVolumeWidget(this));

  QTabWidget* tabWidget = new QTabWidget(this);
  layout->addWidget(tabWidget);
  tabWidget->addTab(new TransferFunctionWidget(this), "Transfer Functions");
  tabWidget->addTab(new ShadingWidget(this), "Shading");
  tabWidget->addTab(new CroppingWidget(this), "Cropping");
}

}
