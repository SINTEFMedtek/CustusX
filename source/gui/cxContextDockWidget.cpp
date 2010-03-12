#include "cxContextDockWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QComboBox>
//#include "sscVolumetricRep.h" //TODO REMOVE
#include "sscProbeRep.h"
#include "cxDataManager.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
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
  mTabWidget(new QTabWidget(mGuiWidget)),
  mDataManager(DataManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance())
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
          mDataManager, SLOT(deleteImageSlot(ssc::ImagePtr)));
  connect(mDataManager, SIGNAL(currentImageDeleted(ssc::ImagePtr)),
          mViewManager, SLOT(deleteImageSlot(ssc::ImagePtr)));
  
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
    messageMan()->sendWarning("Can't delete image, no current Image!");
    return;
  }
  emit deleteImage(mCurrentImage);
}

void ContextDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
{
  if(visible)
  {
    connect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBoxSlot()));
    connect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
            this, SLOT(populateTheImageComboBoxSlot()));
    this->populateTheImageComboBoxSlot();
  }
  else
  {
    disconnect(mDataManager, SIGNAL(dataLoaded()),
               this, SLOT(populateTheImageComboBoxSlot()));
    disconnect(mViewManager, SIGNAL(imageDeletedFromViews(ssc::ImagePtr)),
               this, SLOT(populateTheImageComboBoxSlot()));
  }
}
void ContextDockWidget::populateTheImageComboBoxSlot()
{
  mImagesComboBox->clear();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = mDataManager->getImages();
  if(images.size() == 0)
  {
    mImagesComboBox->insertItem(1, QString("Import an image to begin..."));
    mImagesComboBox->setEnabled(false);
    return;
  }

  mImagesComboBox->setEnabled(true);

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  int listPosition = 1;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }
}
  
void ContextDockWidget::imageSelectedSlot(const QString& comboBoxText)
{
  //messageMan()->sendInfo("New image selected: "+comboBoxText.toStdString());
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("..."))
  {
    // Create empty current image
    mCurrentImage.reset();
    emit currentImageChanged(mCurrentImage);
    return;
  }

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = mDataManager->getImage(imageId);
  if(!image)
  {
    messageMan()->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  //Set new current image
  mCurrentImage = image;
  emit currentImageChanged(mCurrentImage);
}
}//namespace cx
