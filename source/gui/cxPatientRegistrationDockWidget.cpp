#include "cxPatientRegistrationDockWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <vtkDoubleArray.h>

#include "sscDataManager.h"
#include "cxVolumetricRep.h"
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"

/**
 * cxPatientRegistrationDockWidget.cpp
 *
 * \brief
 *
 * \date Feb 3, 2009
 * \author: Janne Beate Bakeng, SINTEF
 * \author Geir Arne Tangen, SINTEF
 */

namespace cx
{
PatientRegistrationDockWidget::PatientRegistrationDockWidget() :
  mGuiContainer(new QWidget(this)),
  mVerticalLayout(new QVBoxLayout(mGuiContainer)),
  mImagesComboBox(new QComboBox(mGuiContainer)),
  mLandmarkTableWidget(new QTableWidget(mGuiContainer)),
  mToolSampleButton(new QPushButton("Sample Tool", mGuiContainer)),
  mDoRegistrationButton(new QPushButton("Do Registration", mGuiContainer)),
  mAccuracyLabel(new QLabel(QString(" "),mGuiContainer)),
  mDataManager(DataManager::getInstance()),
  mRegistrationManager(RegistrationManager::getInstance()),
  mToolManager(ToolManager::getInstance()),
  mMessageManager(MessageManager::getInstance()),
  mViewManager(ViewManager::getInstance()),
  mRepManager(RepManager::getInstance()),
  mCurrentRow(-1),
  mCurrentColumn(-1)
{
  //Dock widget
  this->setWindowTitle("Patient Registration");
  this->setWidget(mGuiContainer);
  connect(this, SIGNAL(visibilityChanged(bool)),
          this, SLOT(visibilityOfDockWidgetChangedSlot(bool)));

  //combobox
  mImagesComboBox->setEditable(false);
  mImagesComboBox->setEnabled(false);
  connect(mImagesComboBox, SIGNAL(currentIndexChanged(const QString& )),
          this, SLOT(imageSelectedSlot(const QString& )));

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int, int)),
          this, SLOT(cellChangedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(rowSelectedSlot(int, int)));

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()),
          this, SLOT(toolSampleButtonClickedSlot()));
  mDoRegistrationButton->setDisabled(true);
  connect(mDoRegistrationButton, SIGNAL(clicked()),
          this, SLOT(doRegistrationButtonClickedSlot()));

  //toolmanager
  connect(mToolManager, SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
  connect(mToolManager, SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
  connect(mToolManager, SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

  //layout
  mVerticalLayout->addWidget(mImagesComboBox);
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mDoRegistrationButton);
  mVerticalLayout->addWidget(mAccuracyLabel);

  ssc::ToolPtr dominantTool = mToolManager->getDominantTool();
  if(dominantTool.get() != NULL)
    this->dominantToolChangedSlot(dominantTool->getUid());
}
PatientRegistrationDockWidget::~PatientRegistrationDockWidget()
{}
void PatientRegistrationDockWidget::imageSelectedSlot(const QString& comboBoxText)
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
  //mLandmarkTableWidget = mRegistrationManager->getActivePointsVector();

  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(image);

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
}
void PatientRegistrationDockWidget::visibilityOfDockWidgetChangedSlot(bool visible)
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

    //TODO
    //update the activevector in registration manager
  }
}
void PatientRegistrationDockWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
{
  int numberOfToolSamples = mToolManager->getToolSamples()->GetNumberOfTuples();
  int numberOfActiveToolSamples = 0;
  std::vector<bool>::iterator it = mLandmarkActiveVector.begin();
  while(it != mLandmarkActiveVector.end())
  {
    if((*it))
      numberOfActiveToolSamples++;
  }
  if(numberOfActiveToolSamples >= 3 && numberOfToolSamples >= 3)
  {
    this->doPatientRegistration();
    this->updateAccuracy();
  }
}
void PatientRegistrationDockWidget::toolVisibleSlot(bool visible)
{
  if(visible)
    mToolSampleButton->setEnabled(true);
  else
    mToolSampleButton->setEnabled(false);
}
void PatientRegistrationDockWidget::toolSampleButtonClickedSlot()
{
  ssc::Transform3DPtr lastTransform = mToolToSample->getLastTransform();
  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform->matrix();
  double x = lastTransformMatrix->GetElement(0,3);
  double y = lastTransformMatrix->GetElement(1,3);
  double z = lastTransformMatrix->GetElement(2,3);

  unsigned int index = mCurrentRow+1;
  mToolManager->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationDockWidget::doRegistrationButtonClickedSlot()
{
  this->doPatientRegistration();
  this->updateAccuracy();
}
void PatientRegistrationDockWidget::rowSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
}
void PatientRegistrationDockWidget::populateTheImageComboBox()
{
  mImagesComboBox->clear();

  //find out if the master image is set
  ssc::ImagePtr masterImage = mRegistrationManager->getMasterImage();

  //get a list of images from the datamanager
  std::map<std::string, ssc::ImagePtr> images = mDataManager->getImages();
  if(images.size() == 0 || masterImage.get() == NULL)
  {
    mImagesComboBox->insertItem(1, QString("First do Image Registration..."));
    mImagesComboBox->setEnabled(false);
    return;
  }

  //add these to the combobox
  typedef std::map<std::string, ssc::ImagePtr>::iterator iterator;
  int listPosition = 1;
  for(iterator i = images.begin(); i != images.end(); ++i)
  {
    mImagesComboBox->insertItem(listPosition, QString(i->first.c_str()));
    listPosition++;
  }

  //set the master image as the selected on
  std::string uid = masterImage->getUid();
  int comboboxIndex = mImagesComboBox->findText(QString(uid.c_str()));
  if (comboboxIndex < 0)
    return;

  mImagesComboBox->setCurrentIndex(comboboxIndex);
}
void PatientRegistrationDockWidget::cellChangedSlot(int row, int column)
{
  if (column!=0)
    return;

  Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
  mLandmarkActiveVector.push_back(state);

}
void PatientRegistrationDockWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample.get() != NULL && mToolToSample->getUid() == uid)
    return;

  ToolPtr newTool = ToolPtr(dynamic_cast<Tool*>(mToolManager->getTool(uid).get()));
  if(mToolToSample.get() != NULL)
  {
    if(newTool.get() == NULL)
      return;

    disconnect(mToolToSample.get(), SIGNAL(toolVisible()),
                this, SLOT(toolVisibleSlot()));
  }

  mToolToSample = newTool;
  connect(mToolToSample.get(), SIGNAL(toolVisible()),
              this, SLOT(toolVisibleSlot()));

  //update button
  mToolSampleButton->setEnabled(mToolToSample->getVisible());
}
void PatientRegistrationDockWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  mLandmarkActiveVector.clear();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(3);
  QStringList headerItems(QStringList() << "Active" << "Name" << "Accuracy");
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
    QTableWidgetItem* columnThree;

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
        columnThree = new QTableWidgetItem();
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
      }
      //TODO
      //check the mLandmarkActiveVector...
      columnOne->setCheckState(Qt::Checked);
      //columnThree->setFlags(Qt::ItemIsSelectable);
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 1, columnThree);
      mLandmarkActiveVector.push_back(true);
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
    QTableWidgetItem* columnTwo;

    if(index > mLandmarkTableWidget->rowCount())
    {
      mLandmarkTableWidget->setRowCount(index);
      columnTwo = new QTableWidgetItem();
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
    }
    else
    {
      columnTwo = mLandmarkTableWidget->item(row, 1);
      if(columnTwo == NULL) //TODO: remove
        std::cout << "columnTwo == NULL!!!" << std::endl;
    }
    columnTwo->setText(QString(name.c_str()));
  }
}
void PatientRegistrationDockWidget::updateAccuracy()
{
  //ssc:Image masterImage = mRegistrationManager->getMasterImage();
  vtkDoubleArrayPtr globalPointset = mRegistrationManager->getGlobalPointSet();
  vtkDoubleArrayPtr toolPointset = mToolManager->getToolSamples();
  
  ssc::Transform3DPtr rMpr = mToolManager->get_rMpr();
}
void PatientRegistrationDockWidget::doPatientRegistration()
{
  mRegistrationManager->doPatientRegistration();
}
}//namespace cx
