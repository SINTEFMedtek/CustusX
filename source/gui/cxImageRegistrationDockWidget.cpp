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
#include "cxRegistrationManager.h"
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
  mEditLandmarkButton(new QPushButton("Resample landmark", mGuiContainer)),
  mRemoveLandmarkButton(new QPushButton("Remove landmark", mGuiContainer)),
  mRepManager(RepManager::getInstance()),
  mDataManager(DataManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
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
  mEditLandmarkButton->setDisabled(true);
  connect(mEditLandmarkButton, SIGNAL(clicked()),
          this, SLOT(editLandmarkButtonClickedSlot()));
  mRemoveLandmarkButton->setDisabled(true);
  connect(mRemoveLandmarkButton, SIGNAL(clicked()),
          this, SLOT(removeLandmarkButtonClickedSlot()));

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(landmarkSelectedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int,int)),
          this, SLOT(cellChangedSlot(int,int)));

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mAddLandmarkButton);
  mVerticalLayout->addWidget(mEditLandmarkButton);
  mVerticalLayout->addWidget(mRemoveLandmarkButton);
  mGuiContainer->setLayout(mVerticalLayout);

}
ImageRegistrationDockWidget::~ImageRegistrationDockWidget()
{}
void ImageRegistrationDockWidget::addLandmarkButtonClickedSlot()
{
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  if(volumetricRep.get() == NULL)
  {
    mMessageManager->sendError("Could not find a rep to add the landmark to.");
    return;
  }
  int index = mLandmarkTableWidget->rowCount()+1;
  volumetricRep->makePointPermanent(index);

  //TODO: find a better place for this?
  //make sure the masterImage is set
  ssc::ImagePtr masterImage = mRegistrationManager->getMasterImage();
  if(masterImage.get() == NULL)
    mRegistrationManager->setMasterImage(mCurrentImage);
}
void ImageRegistrationDockWidget::editLandmarkButtonClickedSlot()
{
  VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
  if(volumetricRep.get() == NULL)
  {
    mMessageManager->sendError("Could not find a rep to edit the landmark for.");
    return;
  }
  int index = mCurrentRow+1;
  volumetricRep->makePointPermanent(index);
}
void ImageRegistrationDockWidget::removeLandmarkButtonClickedSlot()
{
  if(mCurrentRow < 0 || mCurrentColumn < 0)
    return;

  int index = mCurrentRow+1;

  LandmarkRepPtr landmarkRep = mRepManager->getLandmarkRep("LandmarkRep_1");
  landmarkRep->removePermanentPoint(index);
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
    mMessageManager->sendError("Could not find the selected image in the DataManager: "+imageId);
    return;
  }

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  }

  //Set new current image
  mCurrentImage = image;
  connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
          this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);

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

  //TODO:
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

    //TODO:
    //bool allregistered = this->checkRegistrationStatus() -> send out warning if not all images are registrated?
    //if !allregistered -> send out warning, not registrated images disables nav. and us. workflow?
    // ...or don't send out warning/error until pat. nav. or us.???

    //clean up
    VolumetricRepPtr volumetricRep = mRepManager->getVolumetricRep("VolumetricRep_1");
    InriaRep2DPtr inriaRep2D_1 = mRepManager->getInria2DRep("InriaRep2D_1");
    InriaRep2DPtr inriaRep2D_2 = mRepManager->getInria2DRep("InriaRep2D_2");
    InriaRep2DPtr inriaRep2D_3 = mRepManager->getInria2DRep("InriaRep2D_3");
    disconnect(volumetricRep.get(), SIGNAL(pointPicked(double,double,double)),
            inriaRep2D_1.get(), SLOT(syncSetPosition(double,double,double)));
    disconnect(inriaRep2D_1.get(), SIGNAL(pointPicked(double,double,double)),
            volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_2.get(), SIGNAL(pointPicked(double,double,double)),
            volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));
    disconnect(inriaRep2D_3.get(), SIGNAL(pointPicked(double,double,double)),
            volumetricRep.get(), SLOT(showTemporaryPointSlot(double,double,double)));

    //update global pointset before exiting dockwidget, only if current image is master image
    ssc::ImagePtr masterImage = mRegistrationManager->getMasterImage();
    if(masterImage == mCurrentImage)
      mRegistrationManager->setGlobalPointSet(mCurrentImage->getLandmarks());
  }
}
void ImageRegistrationDockWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
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

  mImagesComboBox->setEnabled(true);

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  int listPosition = 1;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }
  //enable the add point button if any images was found
  mAddLandmarkButton->setEnabled(true);
}
void ImageRegistrationDockWidget::landmarkSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;

  mEditLandmarkButton->setEnabled(true);
}
void ImageRegistrationDockWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(2);
  QStringList headerItems(QStringList() << "Name" << "Landmark");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  //fill the table widget with rows for the landmarks
  int row = 0;
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark = landmarks->GetTuple(i);
    if(landmark[3] > mLandmarkTableWidget->rowCount())
      mLandmarkTableWidget->setRowCount(landmark[3]);
    QTableWidgetItem* columnOne;
    QTableWidgetItem* columnTwo;

    int rowToInsert = landmark[3]-1;
    int tempRow = -1;
    if(rowToInsert < row)
    {
      tempRow = row;
      row = rowToInsert;
    }
    for(; row <= rowToInsert; row++)
    {
      if(row == rowToInsert)
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
      }
      columnTwo->setFlags(Qt::ItemIsSelectable);
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
    }
    if(tempRow != -1)
      row = tempRow;
  }
  //fill in names
  typedef RegistrationManager::NameListType::iterator Iterator;
  for(Iterator it = nameList.begin(); it != nameList.end(); ++it)
  {
    std::string name = it->second.first;
    int index = it->first;
    int row = index-1;
    QTableWidgetItem* columnOne;

    if(index > mLandmarkTableWidget->rowCount())
    {
      mLandmarkTableWidget->setRowCount(index);
      columnOne = new QTableWidgetItem();
      QTableWidgetItem* columnTwo = new QTableWidgetItem();
      columnTwo->setFlags(Qt::ItemIsSelectable);
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
    }
    else
    {
      columnOne = mLandmarkTableWidget->item(row, 0);
      if(columnOne == NULL) //TODO: remove
        std::cout << "columnOne == NULL!!!" << std::endl;
    }
    columnOne->setText(QString(name.c_str()));
  }

  //highlight selected row
  if(mCurrentRow != -1 && mCurrentColumn != -1)
    mLandmarkTableWidget->setCurrentCell(mCurrentRow, mCurrentColumn);

  //update buttons
  if(numberOfLandmarks == 0)
  {
    mRemoveLandmarkButton->setDisabled(true);
    mEditLandmarkButton->setDisabled(true);
  }
  else
  {
    mRemoveLandmarkButton->setEnabled(true);
    if(mCurrentRow != -1 && mCurrentColumn != -1)
      mEditLandmarkButton->setEnabled(true);
  }
}
void ImageRegistrationDockWidget::cellChangedSlot(int row,int column)
{
  if(column != 0)
    return;

  std::string name = mLandmarkTableWidget->item(row, column)->text().toStdString();

  int index = row+1;
  mRegistrationManager->setGlobalPointsNameSlot(index, name);
}
}//namespace cx
