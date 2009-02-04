#include "cxImageRegistrationDockWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <vtkDoubleArray.h>
#include <vtkImageData.h>
#include "sscDataManager.h"
#include "cxRepManager.h"
#include "cxViewManager.h"
#include "cxMessageManager.h"
#include "cxView3D.h"
#include "cxView2D.h"
#include "cxInriaRep2D.h"

/**
 * cxImageRegistrationDockWidget.cpp
 *
 * \brief
 *
 * \date Jan 27, 2009
 * \author: Janne Beate Bakeng, SINTEF
 */

namespace cx
{
ImageRegistrationDockWidget::ImageRegistrationDockWidget() :
  mGuiContainer(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout(mGuiContainer)),
  mImagesComboBox(new QComboBox(mGuiContainer)),
  mLandmarkTableWidget(new QTableWidget(mGuiContainer)),
  mAddLandmarkButton(new QPushButton("Add landmark", mGuiContainer)),
  mRemoveLandmarkButton(new QPushButton("Remove landmark", mGuiContainer)),
  mRepManager(RepManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1)
{
  //dock widget
  this->setWindowTitle("Image Registration");
  this->setWidget(mGuiContainer);
  connect(this, SIGNAL(visibilityChanged(bool)),
          this, SLOT(visibilityOfDockWidgetChangedSlot(bool)));

  //combobox
  mImagesComboBox->setEditable(false);
  connect(mImagesComboBox, SIGNAL(currentIndexChanged(const QString&)),
          this, SLOT(imageSelectedSlot(const QString&)));

  //pushbuttons
  mAddLandmarkButton->setDisabled(true);
  connect(mAddLandmarkButton, SIGNAL(clicked()),
          this, SLOT(addLandmarkButtonClickedSlot()));
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()),
          this, SLOT(removeLandmarkButtonClickedSlot()));

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(landmarkSelectedSlot(int, int)));

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAddLandmarkButton);
  mVerticalLayout->addWidget(mRemoveLandmarkButton);
  mGuiContainer->setLayout(mVerticalLayout);
}
ImageRegistrationDockWidget::~ImageRegistrationDockWidget()
{}
void ImageRegistrationDockWidget::addPointButtonClickedSlot()
{
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  if(volumetricRep.get() == NULL)
  {
    mMessageManager.sendError("Could not find a rep to add the landmark to.");
    return;
  }
  volumetricRep->makePointPermanent();
}
void ImageRegistrationDockWidget::removePointButtonClickedSlot()
{
  if(mCurrentRow < 0 || mCurrentColumn < 0)
    return;

  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  landmarkRep->removePermanentPoint(mCurrentRow+1);
}
void ImageRegistrationDockWidget::imageSelectedSlot(const QString& comboBoxText)
{
  if(comboBoxText.isEmpty() || comboBoxText.endsWith("..."))
    return;

  std::string imageId = comboBoxText.toStdString();

  //find the image
  ssc::ImagePtr image = mDataManager->getImage(imageId);
  if(image.get() == NULL)
  {
    mMessageManager.sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }
  if(mCurrentImage)
  {
    //disconnect from the old image
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double)));
  }

  //Set new current image
  mCurrentImage = image;
  connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double)));
  connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double)));

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);

  //TODO
  //link volumetricRep and inriaRepss

  //view3D
  View3D* view3D_1 = mViewManager->get3DView("View3D_1");
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  volumetricRep->setImage(mCurrentImage);
  landmarkRep->setImage(mCurrentImage);
  view3D_1->setRep(volumetricRep);
  view3D_1->addRep(landmarkRep);

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
  //TODO: ...or getBaseVtkImageData()???
  inriaRep2D_1->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::AXIAL_ID);
  inriaRep2D_2->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::CORONAL_ID);
  inriaRep2D_3->getVtkViewImage2D()->SetOrientation(vtkViewImage2D::SAGITTAL_ID);
  inriaRep2D_1->getVtkViewImage2D()->AddChild(inriaRep2D_2->getVtkViewImage2D());
  inriaRep2D_2->getVtkViewImage2D()->AddChild(inriaRep2D_3->getVtkViewImage2D());
  inriaRep2D_3->getVtkViewImage2D()->AddChild(inriaRep2D_1->getVtkViewImage2D());
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(mCurrentImage->getRefVtkImageData());
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
/*  connect(volumetricRep, SIGNAL(imageChanged()),
          this, SLOT(react()));
  connect(inriaRep2D, SIGNAL(imageChanged()),
            this, SLOT(react()));*/
}
/*void react()
{
  uid = volumetricRep->getImage()->getUid();
  image = Datamanager_>getImage(uid)
  updatae combobox
  update tablewidget
  inriaRep2D_1->getVtkViewImage2D()->SyncRemoveAllDataSet();
  inriaRep2D_1->getVtkViewImage2D()->SyncAddDataSet(image);
  inriaRep2D_1->getVtkViewImage2D()->SyncReset();
}*/

void ImageRegistrationDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
{
  if(visible)
  {
    connect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBox()));
    this->populateTheImageComboBox();
  }
  else
  {
    disconnect(mDataManager, SIGNAL(dataLoaded()),
            this, SLOT(populateTheImageComboBox()));
  }
}
void ImageRegistrationDockWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ)
{
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void ImageRegistrationDockWidget::populateTheImageComboBox()
{
  mImagesComboBox->clear();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = mDataManager->getImages();
  if(images.size() == 0)
  {
    mAddLandmarkButton->setDisabled(true);
    mImagesComboBox->insertItem(1, QString("Load an image to begin..."));
    mImagesComboBox->setEnabled(false);
    return;
  }
  else
  {
    mImagesComboBox->setEnabled(true);
  }

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  int listPosition = 1;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }
  //enable the add point button if any images was found
  mAddLandmarkButton->setDisabled(false);
}
void ImageRegistrationDockWidget::landmarkSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
}
void ImageRegistrationDockWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(numberOfLandmarks);
  mLandmarkTableWidget->setColumnCount(1);
  QStringList headerItems(QStringList() << "Image space");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->
    setResizeMode(QHeaderView::ResizeToContents);

  for(int row=1; row<=numberOfLandmarks; row++)
  {
    double* point = landmarks->GetTuple(row-1);

    QTableWidgetItem* columnOne = new QTableWidgetItem(tr("(%1, %2, %3)").arg(point[0]).arg(point[1]).arg(point[2]));
    columnOne->setFlags(Qt::ItemIsSelectable);

    mLandmarkTableWidget->setItem(row-1, 0, columnOne);
  }

  if(numberOfLandmarks == 0)
    mRemoveLandmarkButton->setDisabled(true);
  else
    mRemoveLandmarkButton->setDisabled(false);
}
}//namespace cx
