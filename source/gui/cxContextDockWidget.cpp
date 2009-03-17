#include "cxContextDockWidget.h"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include "sscDataManager.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"
#include "cxView3D.h"
#include "cxView2D.h"

namespace cx
{
ContextDockWidget::ContextDockWidget() :
  mGuiWidget(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout()),
  mImagesComboBox(new QComboBox(mGuiWidget)),
  mTabWidget(new QTabWidget(mGuiWidget)),
  mDataManager(DataManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance())
{
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
}
ContextDockWidget::~ContextDockWidget()
{}
int ContextDockWidget::addTab(QWidget * page, const QString & label)
{
  return mTabWidget->addTab(page, label);
}
void ContextDockWidget::removeTab(int tabIndex)
{
  mTabWidget->removeTab(tabIndex);
}
void ContextDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
{
  if(visible)
  {
    connect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBoxSlot()));
    this->populateTheImageComboBoxSlot();
  }
  else
  {
    disconnect(mDataManager, SIGNAL(dataLoaded()),
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
    mImagesComboBox->insertItem(1, QString("Load an image to begin..."));
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
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("..."))
    return;

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = mDataManager->getImage(imageId);
  if(image.get() == NULL)
  {
    mMessageManager->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  //Set new current image
  mCurrentImage = image;
  emit currentImageChanged(mCurrentImage);

  //view3D
  View3D* view3D_1 = mViewManager->get3DView("View3D_1");
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  //ProgressiveVolumetricRepPtr progressiveVolumetricRep = mRepManager->getProgressiveVolumetricRep("ProgressiveVolumetricRep_1");
  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  volumetricRep->setImage(mCurrentImage);
  //progressiveVolumetricRep->setImage(mCurrentImage);
  landmarkRep->setImage(mCurrentImage);
  view3D_1->setRep(volumetricRep);
  //view3D_1->setRep(progressiveVolumetricRep);
  view3D_1->addRep(landmarkRep);
  //TODO add ProbeRep

  //view2D
  View2D* view2D_1 = mViewManager->get2DView("View2D_1");
  View2D* view2D_2 = mViewManager->get2DView("View2D_2");
  View2D* view2D_3 = mViewManager->get2DView("View2D_3");
  InriaRep2DPtr inriaRep2D_1 = mRepManager->getInria2DRep("InriaRep2D_1");
  InriaRep2DPtr inriaRep2D_2 = mRepManager->getInria2DRep("InriaRep2D_2");
  InriaRep2DPtr inriaRep2D_3 = mRepManager->getInria2DRep("InriaRep2D_3");
  view2D_1->setRep(inriaRep2D_1);
  view2D_2->setRep(inriaRep2D_2);
  view2D_3->setRep(inriaRep2D_3);
  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  //TODO: ...or getBaseVtkImageData()???
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(mCurrentImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();

  //link volumetricRep and inriaReps
  connect(volumetricRep.get(), SIGNAL(pointPicked(double,double,double)),
          inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
  connect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
  connect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
          volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
}
}//namespace cx
