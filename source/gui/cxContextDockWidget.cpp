#include "cxContextDockWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include "sscProbeRep.h"
#include "sscMessageManager.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView3D.h"
#include "cxView2D.h"

namespace cx
{
ContextDockWidget::ContextDockWidget(QWidget* parent) :
  QDockWidget(parent),
  mGuiWidget(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout()),
  mImagesComboBox(new QComboBox(mGuiWidget)),
  mTabWidget(new QTabWidget(mGuiWidget))
{
  //QMainWindow::saveState() needs a way of identifying the ContextDockWidget
  this->setObjectName("ContextDockWidget");
  //dock widget
  this->setWidget(mGuiWidget);
  connect(this, SIGNAL(visibilityChanged(bool)),
          this, SLOT(visibilityOfDockWidgetChangedSlot(bool)));

  //combobox
  mImagesComboBox->setEditable(false);
  mImagesComboBox->setEnabled(false);
  connect(mImagesComboBox, SIGNAL(currentIndexChanged(const QString& )),
          this, SLOT(imageSelectedSlot(const QString& )));

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mTabWidget);
  mGuiWidget->setLayout(mVerticalLayout);

  // Change current tab index
  connect(this, SIGNAL(changeTabIndex(int)),
          mTabWidget, SLOT(setCurrentIndex(int)));
  
  // Delete image
  connect(this, SIGNAL(deleteImage(ssc::ImagePtr)),
          dataManager(), SLOT(deleteImageSlot(ssc::ImagePtr)));
  connect(dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),
          viewManager(), SLOT(deleteImageSlot(ssc::ImagePtr)));
  
  //listen for active image changed from the datamanager
  connect(dataManager(), SIGNAL(activeImageChanged(std::string)),
          this, SLOT(activeImageChangedSlot()));
}
ContextDockWidget::~ContextDockWidget()
{}
int ContextDockWidget::addTab(QWidget * page, const QString & label)
{
  int tabIndex = mTabWidget->addTab(page, label);
  emit changeTabIndex(tabIndex);
  return tabIndex;
}
void ContextDockWidget::removeTab(int tabIndex)
{
  mTabWidget->removeTab(tabIndex);
}

void ContextDockWidget::deleteCurrentImageSlot()
{
  if (mCurrentImage.use_count() == 0)
  {
    ssc::messageManager()->sendWarning("Can't delete image, no current Image!");
    return;
  }
  emit deleteImage(mCurrentImage);
}

void ContextDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
{
  if(visible)
  {
    connect(dataManager(), SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBoxSlot()));
    connect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
            this, SLOT(populateTheImageComboBoxSlot()));
    this->populateTheImageComboBoxSlot();
  }
  else
  {
    disconnect(dataManager(), SIGNAL(dataLoaded()),
               this, SLOT(populateTheImageComboBoxSlot()));
    disconnect(viewManager(), SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
               this, SLOT(populateTheImageComboBoxSlot()));
  }
}
void ContextDockWidget::populateTheImageComboBoxSlot()
{
  mImagesComboBox->blockSignals(true);
  mImagesComboBox->clear();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = dataManager()->getImages();
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
  
void ContextDockWidget::imageSelectedSlot(const QString& comboBoxText)
{
  //messageMan()->sendInfo("New image selected: "+comboBoxText.toStdString());
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("...") 
     || comboBoxText.endsWith(">"))
  {
    // Create empty current image
    mCurrentImage.reset();
    dataManager()->setActiveImage(mCurrentImage);

    //emit currentImageChanged(mCurrentImage); //TODO remove
    return;
  }

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = dataManager()->getImage(imageId);
  if(!image)
  {
    ssc::messageManager()->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  if(mCurrentImage == image)
    return;

  //Set new current image
  mCurrentImage = image;
  dataManager()->setActiveImage(mCurrentImage);

  //emit currentImageChanged(mCurrentImage); //TODO remove
}
void ContextDockWidget::activeImageChangedSlot()
{
  ssc::ImagePtr activeImage = dataManager()->getActiveImage();
  if(mCurrentImage == activeImage || !activeImage)
    return;

  const QString& qUid(activeImage->getUid().c_str());
  this->imageSelectedSlot(qUid);

  //find the index in the combobox and set it
  int index = mImagesComboBox->findText(qUid);
  mImagesComboBox->setCurrentIndex(index);
}
/*void ContextDockWidget::activeImageChangedSlot(std::string uid)
{
  const QString& qUid(uid.c_str());
  this->imageSelectedSlot(qUid);

  //find the index in the combobox and set it
  int index = mImagesComboBox->findText(qUid);
  mImagesComboBox->setCurrentIndex(index);
}*/
}//namespace cx
