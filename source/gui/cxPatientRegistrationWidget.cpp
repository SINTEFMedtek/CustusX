#include "cxPatientRegistrationWidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <vtkDoubleArray.h>
#include <sscVector3D.h>
#include <sscVolumetricRep.h>
#include "cxLandmarkRep.h"
#include "cxView2D.h"
#include "cxView3D.h"
#include "cxRegistrationManager.h"
#include "cxMessageManager.h"
#include "cxToolManager.h"
#include "cxViewManager.h"
#include "cxRepManager.h"

namespace cx
{
PatientRegistrationWidget::PatientRegistrationWidget(QWidget* parent) :
  QWidget(parent),
  mVerticalLayout(new QVBoxLayout(this)),
  mLandmarkTableWidget(new QTableWidget(this)),
  mToolSampleButton(new QPushButton("Sample Tool", this)),
  mAvarageAccuracyLabel(new QLabel(QString(" "), this)),
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

  //table widget
  connect(mLandmarkTableWidget, SIGNAL(cellChanged(int, int)),
          this, SLOT(cellChangedSlot(int, int)));
  connect(mLandmarkTableWidget, SIGNAL(cellClicked(int, int)),
          this, SLOT(rowSelectedSlot(int, int)));

  //buttons
  mToolSampleButton->setDisabled(true);
  connect(mToolSampleButton, SIGNAL(clicked()),
          this, SLOT(toolSampleButtonClickedSlot()));

  //toolmanager
  connect(mToolManager, SIGNAL(dominantToolChanged(const std::string&)),
          this, SLOT(dominantToolChangedSlot(const std::string&)));
  connect(mToolManager, SIGNAL(toolSampleAdded(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));
  connect(mToolManager, SIGNAL(toolSampleRemoved(double,double,double,unsigned int)),
          this, SLOT(toolSampledUpdateSlot(double, double, double,unsigned int)));

  //layout
  mVerticalLayout->addWidget(mLandmarkTableWidget);
  mVerticalLayout->addWidget(mToolSampleButton);
  mVerticalLayout->addWidget(mAvarageAccuracyLabel);
  this->setLayout(mVerticalLayout);

  ssc::ToolPtr dominantTool = mToolManager->getDominantTool();
  if(dominantTool.get() != NULL)
    this->dominantToolChangedSlot(dominantTool->getUid());
}
PatientRegistrationWidget::~PatientRegistrationWidget()
{}
void PatientRegistrationWidget::currentImageChangedSlot(ssc::ImagePtr currentImage)
{
  if(mCurrentImage == currentImage)
    return;

  //disconnect from the old image
  if(mCurrentImage)
  {
    disconnect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    disconnect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
              this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    mMessageManager->sendInfo("Disconnected from old image "+mCurrentImage->getUid());
  }

  mCurrentImage = currentImage;

  //connect to new image
  if (mCurrentImage) //Don't use image if deleted
  {
    connect(mCurrentImage.get(), SIGNAL(landmarkAdded(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
    connect(mCurrentImage.get(), SIGNAL(landmarkRemoved(double,double,double,unsigned int)),
            this, SLOT(imageLandmarksUpdateSlot(double,double,double,unsigned int)));
  }
  
  //get the images landmarks and populate the landmark table
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::imageLandmarksUpdateSlot(double notUsedX, double notUsedY, double notUsedZ, unsigned int notUsedIndex)
{
  //repopulate the tablewidget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::toolSampledUpdateSlot(double notUsedX, double notUsedY, double notUsedZ,unsigned int notUsedIndex)
{
  int numberOfToolSamples = mToolManager->getToolSamples()->GetNumberOfTuples();
  int numberOfActiveToolSamples = 0;
  RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
  while(it != landmarkActiveMap.end())
  {
    if(it->second.second)
      numberOfActiveToolSamples++;
    it++;
  }
  //TODO REMOVE just for debugging
  /*std::stringstream stream;
  stream<<"ActiveToolSamples: "<<numberOfActiveToolSamples<<", ToolSamples: "<< numberOfToolSamples;
  mMessageManager->sendWarning(stream.str());*/
  //END
  if(numberOfActiveToolSamples >= 3 && numberOfToolSamples >= 3)
  {
    this->doPatientRegistration();
    this->updateAccuracy();
  }
}
void PatientRegistrationWidget::toolVisibleSlot(bool visible)
{
  if(visible)
    mToolSampleButton->setEnabled(true);
  else
    mToolSampleButton->setEnabled(false);
}
void PatientRegistrationWidget::toolSampleButtonClickedSlot()
{  
  //TODO What if the reference frame isnt visible?
  ssc::Transform3DPtr lastTransform = mToolToSample->getLastTransform();
  if(lastTransform.get() == NULL)
  {
    mMessageManager->sendError("The last transform was NULL!");
    return;
  }

  vtkMatrix4x4Ptr lastTransformMatrix = lastTransform->matrix();
  double x = lastTransformMatrix->GetElement(0,3);
  double y = lastTransformMatrix->GetElement(1,3);
  double z = lastTransformMatrix->GetElement(2,3);

  if(mCurrentRow == -1)
    mCurrentRow = 0;
  unsigned int index = mCurrentRow+1;
  
  //TODO REMOVE just for debugging
  /*std::stringstream message;
  message<<"Sampling row "<<mCurrentRow<<" for LANDMARK: "<<index;
  mMessageManager->sendWarning(message.str());*/
  //END
  
  mToolManager->addToolSampleSlot(x, y, z, index);
}
void PatientRegistrationWidget::rowSelectedSlot(int row, int column)
{
  mCurrentRow = row;
  mCurrentColumn = column;
  
  //TODO REMOVE just for debugging
  std::stringstream stream;
  stream<<"You clicked cell: ("<<mCurrentRow<<","<<mCurrentColumn<<").";
  mMessageManager->sendInfo(stream.str());
  //END
}
void PatientRegistrationWidget::cellChangedSlot(int row, int column)
{
  if(column!=1) //can only make changes to the status (landmark active or not)
    return;

  Qt::CheckState state = mLandmarkTableWidget->item(row,column)->checkState();
  mRegistrationManager->setGlobalPointsActiveSlot(row, state);

}
void PatientRegistrationWidget::dominantToolChangedSlot(const std::string& uid)
{
  if(mToolToSample.get() != NULL && mToolToSample->getUid() == uid)
    return;

  ToolPtr newTool = ToolPtr(dynamic_cast<Tool*>(mToolManager->getDominantTool().get()));
  if(mToolToSample.get() != NULL)
  {
    if(newTool.get() == NULL)
      return;

    disconnect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
                this, SLOT(toolVisibleSlot(bool)));
  }

  mToolToSample = newTool;
  connect(mToolToSample.get(), SIGNAL(toolVisible(bool)),
              this, SLOT(toolVisibleSlot(bool)));

  //TODO: REMOVE
  //only for testing...
  ssc::ToolRep3DPtr toolRep3D_1 = mRepManager->getToolRep3DRep("ToolRep3D_1");
  toolRep3D_1->setTool(mToolToSample);
  View3D* view = mViewManager->get3DView("View3D_1");
  view->addRep(toolRep3D_1);

  //update button
  mToolSampleButton->setEnabled(mToolToSample->getVisible());
}
void PatientRegistrationWidget::showEvent(QShowEvent* event)
{
  QWidget::showEvent(event);
  this->populateTheLandmarkTableWidget(mCurrentImage);
}
void PatientRegistrationWidget::populateTheLandmarkTableWidget(ssc::ImagePtr image)
{
  if (!image)
  {
    mLandmarkTableWidget->clear();
    return;
  }
  //get globalPointsNameList from the RegistrationManager
  RegistrationManager::NameListType nameList = mRegistrationManager->getGlobalPointSetNameList();

  //get the landmarks from the image
  vtkDoubleArrayPtr landmarks =  image->getLandmarks();
  int numberOfLandmarks = landmarks->GetNumberOfTuples();

  //ready the table widget
  mLandmarkTableWidget->clear();
  mLandmarkTableWidget->setRowCount(0);
  mLandmarkTableWidget->setColumnCount(4);
  QStringList headerItems(QStringList() << "Name" << "Status" << "Patient coordinates" << "Accuracy");
  mLandmarkTableWidget->setHorizontalHeaderLabels(headerItems);
  mLandmarkTableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  mLandmarkTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

  //fill the table widget with rows for the landmarks
  int row = 0;
  for(int i=0; i<numberOfLandmarks; i++)
  {
    double* landmark = landmarks->GetTuple(i);
    std::map<int, double>::iterator it = mLandmarkRegistrationAccuracyMap.find(landmark[3]);
    double landmarkRegistrationAccuracy = it->second;

    if(landmark[3] > mLandmarkTableWidget->rowCount())
      mLandmarkTableWidget->setRowCount(landmark[3]);
    QTableWidgetItem* columnOne; //name
    QTableWidgetItem* columnTwo; //status
    QTableWidgetItem* columnThree; //patient coordinates
    QTableWidgetItem* columnFour; //accuracy

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
        columnOne = new QTableWidgetItem(tr("(%1, %2, %3)").arg(landmark[0]).arg(landmark[1]).arg(landmark[2]));
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
        columnFour = new QTableWidgetItem(tr("%1").arg(landmarkRegistrationAccuracy));
      }
      else
      {
        columnOne = new QTableWidgetItem();
        columnTwo = new QTableWidgetItem();
        columnThree = new QTableWidgetItem();
        columnFour = new QTableWidgetItem();
      }
      //check the mLandmarkActiveVector...
      RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
      RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(row+1);
      if(it != landmarkActiveMap.end())
      {
        if(!it->second.second)
          columnTwo->setCheckState(Qt::Unchecked);
        else
          columnTwo->setCheckState(Qt::Checked);
      }
      else
      {
        columnTwo->setCheckState(Qt::Checked);
        mRegistrationManager->setGlobalPointsActiveSlot(row, true);
      }
      //set flags and add the item to the table
      //TODO dosnt work
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate); //name should be editable and selectable
      columnTwo->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//status should be ?
      columnThree->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//tool coords. should be ?
      columnFour->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//accuracy should be ?*/
      mLandmarkTableWidget->setItem(row, 0, columnOne);
      mLandmarkTableWidget->setItem(row, 1, columnTwo);
      mLandmarkTableWidget->setItem(row, 2, columnThree);
      mLandmarkTableWidget->setItem(row, 3, columnFour);
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

    if(mLandmarkTableWidget->rowCount() == 0 ||
       index > mLandmarkTableWidget->rowCount()) //we need more rows
    {
      mLandmarkTableWidget->setRowCount(index);
      columnOne = new QTableWidgetItem();
      //set flags and add the item to the table
      //TODO dosnt work
      /*columnOne->setFlags(Qt::ItemIsSelectable &&
                          Qt::ItemIsEditable &&
                          !Qt::ItemIsDragEnabled &&
                          !Qt::ItemIsDropEnabled &&
                          !Qt::ItemIsUserCheckable &&
                          !Qt::ItemIsEnabled &&
                          !Qt::ItemIsTristate);//name should be selectable and editable*/ 
      mLandmarkTableWidget->setItem(row, 0, columnOne);
    }
    else //we have all the rows we need atm
    {
      columnOne = mLandmarkTableWidget->item(row, 0);
    }
    if(columnOne != NULL && !name.empty())
      columnOne->setText(QString(name.c_str()));
  }
  //get the patient coordinates from the toolmanager
  vtkDoubleArrayPtr toolsamples = mToolManager->getToolSamples();
  int numberOfToolSamples = toolsamples->GetNumberOfTuples();
 
  //TODO REMOVE for debugging
  /*std::stringstream stream;
  stream<<"Number of tools sampled: "<<numberOfToolSamples;
  mMessageManager->sendWarning(stream.str());*/
  //END

  //fill in toolsamples
  for(int i=0; i<numberOfToolSamples; i++)
  {
    double* toolSample = toolsamples->GetTuple(i);
    int row = toolSample[3]-1;
    QTableWidgetItem* columnThree = mLandmarkTableWidget->item(row, 2);
    if(columnThree == NULL)
    {
      mMessageManager->sendError("Couldn't find a cell in the table to put the toolsample in.");
    }
    else
    {
      columnThree->setText(tr("(%1, %2, %3)").arg(toolSample[0]).arg(toolSample[1]).arg(toolSample[2]));
    }
  }
}
void PatientRegistrationWidget::updateAccuracy()
{
  //ssc:Image masterImage = mRegistrationManager->getMasterImage();
  vtkDoubleArrayPtr globalPointset = mRegistrationManager->getGlobalPointSet();
  vtkDoubleArrayPtr toolSamplePointset = mToolManager->getToolSamples();

  ssc::Transform3DPtr rMpr = mToolManager->get_rMpr();

  int numberOfGlobalImagePoints = globalPointset->GetNumberOfTuples();
  int numberOfToolSamplePoints = toolSamplePointset->GetNumberOfTuples();

  // First reset the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    double* imagePoint = globalPointset->GetTuple(i);
    mLandmarkRegistrationAccuracyMap[imagePoint[3]] = 1000;
  }

  // Calculate and fill the accuracy table
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    for(int j=0; j < numberOfToolSamplePoints; j++)
    {
      double* targetPoint = globalPointset->GetTuple(i);
      double* sourcePoint = toolSamplePointset->GetTuple(j);
      
      if(sourcePoint[3] == targetPoint[3])
      {
        //check the mLandmarkActiveVector...
        RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
        RegistrationManager::NameListType::iterator it = landmarkActiveMap.find(sourcePoint[3]);
        if(it != landmarkActiveMap.end())
        {
          if(it->second.second)
          {
            // Calculate accuracy - Set mLandmarkAccuracy
            ssc::Vector3D sourcePointVector(sourcePoint[0],
                                            sourcePoint[1],
                                            sourcePoint[2]);
            ssc::Vector3D transformedPointVector = rMpr->coord(sourcePointVector);

            double xAccuracy = targetPoint[0] - transformedPointVector[0];
            double yAccuracy = targetPoint[1] - transformedPointVector[1];
            double zAccuracy = targetPoint[2] - transformedPointVector[2];

            //TODO REMOVE
            /*std::stringstream stream;
            stream<<"Landmark: "<<targetPoint[3]<<" ("<<targetPoint[0]<<","<<targetPoint[1]<<","<<targetPoint[2]<<")"<<std::endl;
            stream<<"Toolpoint: "<<sourcePoint[3]<<" ("<<sourcePoint[0]<<","<<sourcePoint[1]<<","<<sourcePoint[2]<<")"<<std::endl;
            mMessageManager->sendInfo(stream.str());*/
            //END
            
            mLandmarkRegistrationAccuracyMap[sourcePoint[3]] =
                sqrt(pow(xAccuracy,2) +
                     pow(yAccuracy,2) +
                     pow(zAccuracy,2));
          }
        }
      }
    }
  }

  // Calculate total registration accuracy
  mAverageRegistrationAccuracy = 0;
  RegistrationManager::NameListType landmarkActiveMap = mRegistrationManager->getGlobalPointSetNameList();
  RegistrationManager::NameListType::iterator it = landmarkActiveMap.begin();
  for (int i=0; i < numberOfGlobalImagePoints; i++)
  {
    if(it->second.second)
    {
      mAverageRegistrationAccuracy = mAverageRegistrationAccuracy +
                                    mLandmarkRegistrationAccuracyMap[i];
      it++;
    }
  }

  //make sure the accuracy is filled in the table widget
  this->populateTheLandmarkTableWidget(mCurrentImage);
}

void PatientRegistrationWidget::doPatientRegistration()
{
  mRegistrationManager->doPatientRegistration();
}
}//namespace cx
